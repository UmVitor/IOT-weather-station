
#include "HTTP_REQUEST_API.h"

void http_get_task(uint32_t *paramters)
{
    const char *TEMPLATE = "GET "
                           "/update?api_key=INSERT_HERE_YOUR_API_KEY&field1=%i&field2=%i&field3=%i&field4=%i"
                           " HTTP/1.0\r\n"
                           "Host: " WEB_SERVER ":" WEB_PORT "\r\n"
                           "User-Agent: esp-idf/1.0 esp32\r\n"
                           "\r\n";

    char *REQUEST[sizeof(TEMPLATE) + 40];
    sprintf(REQUEST, TEMPLATE, paramters[0], paramters[1], paramters[2], paramters[3]);

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if (err != 0 || res == NULL)
    {
        ESP_LOGE(TAG2, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
    }

    /* Code to print the resolved IP.
        Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG2, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if (s < 0)
    {
        ESP_LOGE(TAG2, "... Failed to allocate socket.");
        freeaddrinfo(res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG2, "... allocated socket");

    if (connect(s, res->ai_addr, res->ai_addrlen) != 0)
    {
        ESP_LOGE(TAG2, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }

    ESP_LOGI(TAG2, "... connected");
    freeaddrinfo(res);

    if (write(s, REQUEST, strlen(REQUEST)) < 0)
    {
        ESP_LOGE(TAG2, "... socket send failed");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG2, "... socket send success");

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                   sizeof(receiving_timeout)) < 0)
    {
        ESP_LOGE(TAG2, "... failed to set socket receiving timeout");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG2, "... set socket receiving timeout success");

    /* Read HTTP response */
    do
    {
        bzero(recv_buf, sizeof(recv_buf));
        r = read(s, recv_buf, sizeof(recv_buf) - 1);
        for (int i = 0; i < r; i++)
        {
            putchar(recv_buf[i]);
        }
    } while (r > 0);

    ESP_LOGI(TAG2, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
    close(s);
}
