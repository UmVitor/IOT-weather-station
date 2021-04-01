#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>

#include "WIFI_API.h"
#include "HTTP_REQUEST_API.h"
#include "dht11.h"

uint32_t wind_speed_ADC()
{
    uint32_t raw_adc = 0;
    float VelVentoMedida;
    uint32_t VelVentoMedidaTruncada_KmPorHora;

    raw_adc = adc1_get_raw(ADC1_CHANNEL_0);

    VelVentoMedida = 0.926 * ((3.3 / 4096.0) * raw_adc);
    VelVentoMedidaTruncada_KmPorHora = (int)(VelVentoMedida * 3.6);

    //ESP_LOGI("ADC CAL", "Velocidade: %f , vel: %u", VelVentoMedida, VelVentoMedidaTruncada_KmPorHora); //Mostra a leitura calibrada no Serial Monitor
    return VelVentoMedidaTruncada_KmPorHora;
}

uint32_t rain_sensor_ADC()
{
    uint32_t raw_adc = 0;
    raw_adc = adc1_get_raw(ADC1_CHANNEL_3);
    if (raw_adc == 0)
    {
        return (0);
    }
    else
    {
        if (raw_adc < 500)
        {
            return (1);
        }
        else
        {
            if (raw_adc < 1500)
            {
                return (2);
            }
            else
            {
                return (3);
            }
        }
    }
}

/* FreeRTOS event group to signal when we are connected*/

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */


void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    adc1_config_width(ADC_WIDTH_BIT_12);                        //set the resolution of ADC1
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); //set the atenuation

    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11); //set the atenuation

    DHT11_init(GPIO_NUM_4); // Inicia o GPIO

    while(1)
    {
        uint32_t send_data[] = {wind_speed_ADC(), rain_sensor_ADC(), DHT11_read().humidity,DHT11_read().temperature};
        http_get_task(send_data);

        for (int countdown = 15; countdown >= 0; countdown--)
        {
            ESP_LOGI(TAG2, "%d... ", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    
    
}
