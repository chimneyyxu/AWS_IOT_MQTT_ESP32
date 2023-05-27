/* 
    ESP_IDF 4.4 用于 连接AWS MQTT  
    1.使用 http_tls 连接AWS lamble函数 注册 MQTT 并返回 pem ,key, client_id ,iot_name 
    lamble函数在 iot_creat.ipynb 要复制到AWS lamble

    2.使用 esp-aws-iot 库连接mqtt
    3.使用 ble 向 app 端 传递 pem ,key ,iot_name 
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_storage.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "iot_creat.h"
#include "mqtt_demo_mutual_auth.h"
#include "ble.h"
static const char *TAG = "MQTT_EXAMPLE";

/*
 * Prototypes for the demos that can be started from this project.  Note the
 * MQTT demo is not actually started until the network is already.
 */

void gpio_init()
{
    gpio_config_t io_conf = {};
    // disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = LEDNUMS;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    // configure GPIO with the given settings
    gpio_config(&io_conf);
}

void app_main()
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    
    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //esp_storage_init();

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ble_start();  

    gpio_init();

    ESP_ERROR_CHECK(example_connect());

    //使用 http 向aws mqtt 注册设备 并存储 pem ,key, client_id ,iot_name 
    xTaskCreate(&https_request_task, "https_get_task", 8192, NULL, 5, NULL);

    //检查是否已注册 aws mqtt
    while(1){     
        if(creat_iot !=0 || esp_storage_get("iot_name", (void *)iot_names, sizeof(iot_names))==ESP_OK){ 
       // if(creat_iot !=0){           
            esp_storage_get("pem", (void *)iot_pem, sizeof(iot_pem));
            esp_storage_get("key", (void *)iot_pem_key, sizeof(iot_pem_key));
            esp_storage_get("client_id", (void *)client_names, sizeof(client_names));
            esp_storage_get("iot_name", (void *)iot_names, sizeof(iot_names));
           // ESP_LOGW("PEM","%s",iot_pem);
            aws_iot_demo_main(0,NULL);    //进入 aws mqtt  主程序
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
  
}
