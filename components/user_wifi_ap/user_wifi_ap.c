/*
 * User defined WiFi library
 * Made with ESP IDF V5.4
 * Author: Rafael M. Silva 
 * email: rafaellmario@unifei.edu.br
 */

#include "user_wifi_ap.h"

static esp_netif_t* esp_netif_ap;
static const char* TAG = "WIFI-AP";

// =====================================================================================================
// Setup for WiFi Access Point (AP) Mode
// --------------------------------------------------------------------------------
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    // Get the WiFi events
    if(event_base == WIFI_EVENT) 
    {
        switch(event_id) 
        {
            case WIFI_EVENT_AP_START: // Soft-AP start
                ESP_LOGI(TAG,"Access Point Started."); 
                break;
            case WIFI_EVENT_AP_STOP: // Soft-AP stop
                ESP_LOGI(TAG,"Access Point Stopped."); 
                break;
            case WIFI_EVENT_AP_STACONNECTED:  // A station connected to Soft-AP
                // Get the device connection information 
                wifi_event_ap_staconnected_t* conn = (wifi_event_ap_staconnected_t*)event_data;
                ESP_LOGI(TAG,"Device Connected: MAC ="MACSTR", AID = %ld",
                        MAC2STR(conn->mac), (long)conn->aid);
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                // Get the device diconnection information 
                wifi_event_ap_stadisconnected_t* disc = (wifi_event_ap_stadisconnected_t*)event_data;
                ESP_LOGI(TAG,"Device Disconnected: MAC="MACSTR", AID=%ld",
                        MAC2STR(disc->mac), (long)disc->aid);
                break;
            default: // When a undentified 
                    ESP_LOGW(TAG,"Unhandled WiFi event: %ld",(long)event_id);
        }
    }
    else if(event_base == IP_EVENT && event_id == IP_EVENT_AP_STAIPASSIGNED)
    {
        ip_event_ap_staipassigned_t* ip = (ip_event_ap_staipassigned_t*) event_data;
        ESP_LOGI(TAG,"IP assigned to station: "IPSTR,IP2STR(&ip->ip));
    }
}

// --------------------------------------------------------------------------------
// Start softAP operation 
void wifi_init_softap(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_ap = esp_netif_create_default_wifi_ap();

    // Register event handler for Wi-Fi and IP events
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Configure static IP for SoftAP
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, 192, 168, 1, 1);
    IP4_ADDR(&ip_info.gw, 192, 168, 1, 1);
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);

    // Stop DHCP server first if running
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(esp_netif_ap)); 
    // Define the AP default IP 
    ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &ip_info));
    // Restart DHCP server with new config
    ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap)); 

    // Register an event handler for WIFI events
    ESP_ERROR_CHECK( esp_event_handler_instance_register(
        WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_event_handler,
        NULL, NULL));
    
    // Register an event handler for IP events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,IP_EVENT_AP_STAIPASSIGNED,&wifi_event_handler,
        NULL,NULL));
    
    // Set operation to Access Point
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    
    // Configure access point credentials and parameters
    wifi_config_t ap_config = 
    {
        .ap = 
        {
            .ssid = AP_SSID,
            .ssid_len = 0,
            .channel = 1,
            .password = AP_PASSWORD,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK   
        }
    };


    // If AP password is null, define an open AP
    if(strlen((char*)ap_config.ap.password) == 0)
    {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    // Apply the configuration 
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&ap_config));

    // Start the WiFi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG,"WiFi Access Point initialied. SSID:%s Password:%s",
            ap_config.ap.ssid, ap_config.ap.password);
    
    return;
}

// --------------------------------------------------------------------------------
// Disconnect and disable SOFTAP
esp_err_t wifi_disconnect_ap(void)
{   
    esp_err_t err = ESP_OK;

    err = esp_wifi_stop();
    esp_netif_destroy(esp_netif_ap);

    return err;
}
