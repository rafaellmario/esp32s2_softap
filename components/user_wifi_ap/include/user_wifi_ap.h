/*
 * User defined WiFi library
 * Made with ESP IDF V5.4
 * Author: Rafael M. Silva 
 * email: rafaellmario@unifei.edu.br
 */

#ifndef USER_WIFI_AP_H
#define USER_WIFI_AP_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "lwip/ip4_addr.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include <string.h>

#define AP_SSID "ESP32_ACESSPOINT"
#define AP_PASSWORD "00000000"

void wifi_init_softap(void);
esp_err_t wifi_disconnect_ap(void);

#endif