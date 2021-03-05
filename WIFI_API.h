#ifndef WIFI_API_H_
#define WIFI_API_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_tls.h"
#include "esp_crt_bundle.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define EXAMPLE_ESP_MAXIMUM_RETRY 3


#define EXAMPLE_ESP_WIFI_SSID "INSERT_HERE_YOUR_SSID"
#define EXAMPLE_ESP_WIFI_PASS "INSERT_HERE_YOU_PASSWORD"

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static const char *TAG = "wifi station";

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data);

void wifi_init_sta(void);

#endif  // WIFI_API_H_
