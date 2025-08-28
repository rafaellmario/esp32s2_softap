

#include <stdio.h>

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "driver/gpio.h"

#include "user_wifi_ap.h"
#include "nvs_flash.h"


#define BUILTIN_LED GPIO_NUM_21 

static const char* TAG = "MAIN";

void vTimerBlinkCallBack(TimerHandle_t xTimer)
{   

    static bool led_status = 0;
     gpio_set_level(BUILTIN_LED,(led_status^=true));
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG,"Setting up GPIO ...");
    gpio_config_t ioConfig = 
    {
        .pin_bit_mask = (1ULL << BUILTIN_LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&ioConfig);
    gpio_set_level(BUILTIN_LED,false);

    ESP_LOGI(TAG,"DONE");

    TimerHandle_t timerBlink = xTimerCreate("Led Blink",pdMS_TO_TICKS(1000),pdTRUE,
    NULL,vTimerBlinkCallBack);
    xTimerStart(timerBlink,0); // StartTimer 

    wifi_init_softap();

    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // vTaskDelete(NULL); // Delete this taks

}
