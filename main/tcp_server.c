#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "include/tcp_server.h"
#include "include/common.h"

static const char *TAG = "TCP_SERVER";

static bool process_cmd(uint8_t *buffer, int *length)
{
    command_t evt;
    adc_message_t adc;

    int len = *length;

    uint8_t *args = NULL;
    uint8_t cmd = 0;
    uint8_t current_byte = 0;
    int args_counter = 0;
    uint32_t check_sum = 0;
    uint32_t received_check_sum = buffer[len - 1];

    for (int i = 0; i < *length - 1; i++)
    {
        printf("%02X", buffer[i]);
        check_sum += buffer[i];
    }

    ESP_LOGI(TAG, "\n%X  | %X", check_sum & 0xFF, received_check_sum);

    if ((check_sum & 0xFF) != received_check_sum)
    {
        return false;
    }

    if (buffer[0] != 0x01)
    {
        return false;
    }


    cmd = buffer[1];
    args_counter = (int)buffer[2];

    ESP_LOGI(TAG, "VALID WITH %d", args_counter);

    if (args_counter > 0)
    {
        args = malloc(sizeof(uint8_t) * args_counter);
        for (int i = 3; i < 3 + args_counter; i++)
        {
            args[i - 3] = buffer[i];
            printf("%02X", args[i - 3]);
        }
    }


    evt.cmd = cmd;
    memcpy(evt.args, args, len - 4);

    if (cmd == TURN_OFF || cmd == TURN_ON || cmd == TOGGLE)
    {
        xQueueSend(led_cmd_q, &evt, pdMS_TO_TICKS(10));
        strcpy((char*)buffer, "ACK");
        *length =  strlen((char*)buffer);
        return true;
    }

    else if (cmd == LOOP_BACK)
    {
        if (args)
        {
            free(args);
        }
        return true;
    }

    else if(cmd == ADC_READ)
    {
        while(xQueueReceive(adc_read_q, &adc, pdMS_TO_TICKS(200)) == pdFAIL)
        {
            vTaskDelay(1);
        }

        if (args)
        {
            free(args);
        }

        sprintf((char*)buffer, "%04u", adc.reading);
        *length =  strlen((char*)buffer);

        return true;
    }

    return false;
}

static void do_retransmit(const int sock)
{
    int len;
    uint8_t rx_buffer[128];

    do
    {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0)
        {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        }
        else if (len == 0)
        {
            ESP_LOGW(TAG, "Connection closed");
        }
        else
        {
            rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
            for(int i = 0; i < len; i++)
            {
                printf("%02X", rx_buffer[i]);
            }
            printf("\n");
            if (process_cmd(rx_buffer, &len))
            { // send() can return less bytes than supplied length.
                // Walk-around for robust implementation.
                int to_write = len;
                while (to_write > 0)
                {
                    int written = send(sock, rx_buffer + (len - to_write), to_write, 0);
                    if (written < 0)
                    {
                        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    }
                    to_write -= written;
                }
            }
            else
            {
                len = 0;
            }
        }
    } while (len > 0);
}

void tcp_server(void *args)
{
    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = 0;
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;

    if (addr_family == AF_INET)
    {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1)
    {
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET)
        {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }

        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

        do_retransmit(sock);

        shutdown(sock, 0);
        close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}