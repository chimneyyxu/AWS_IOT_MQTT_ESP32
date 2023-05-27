/* HTTPS GET Example using plain mbedTLS sockets
 *
 * Contacts the howsmyssl.com API via TLS v1.2 and reads a JSON
 * response.
 *
 * Adapted from the ssl_client1 example in mbedtls.
 *
 * SPDX-FileCopyrightText: 2006-2016 ARM Limited, All Rights Reserved
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2015-2022 Espressif Systems (Shanghai) CO LTD
 */

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp_storage.h"
#include "cJSON.h"
#include "iot_creat.h"
#include "esp_tls.h"
#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

/* Constants that aren't configurable in menuconfig */
char my_pem[1350];
char my_pem_key[1800];
char iot_name[30];
char client_name[35];
uint8_t creat_iot = 0;

static const char *TAG = "example";
static char iot_creat_post[200];
/* Root cert for howsmyssl.com, taken from server_root_cert.pem

   The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
//截取字符串[n:m]
void sub_str(char *dst,char *src,int n,int m)

{

    char *p=src;
    char *q=dst;
    int len=strlen(src);
    if(m>len || m == -1) m=len;
    int i = (m-n);
    while(n--)  (p++);
    while(i--) *(q++)=*(p++);
    *(q++)='\0';

//return dst;

}

static void https_get_request(esp_tls_cfg_t cfg, const char *WEB_SERVER_URL, const char *REQUEST)
{
    char buf[3500];
    int ret, len;

    struct esp_tls *tls = esp_tls_conn_http_new(WEB_SERVER_URL, &cfg);

    if (tls != NULL) {
        ESP_LOGI(TAG, "Connection established...");
    } else {
        ESP_LOGE(TAG, "Connection failed...");
        goto exit;
    }

    size_t written_bytes = 0;
    do {
        ret = esp_tls_conn_write(tls,
                                 REQUEST + written_bytes,
                                 strlen(REQUEST) - written_bytes);
        if (ret >= 0) {
            ESP_LOGI(TAG, "%d bytes written", ret);
            written_bytes += ret;
        } else if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", ret, esp_err_to_name(ret));
            goto exit;
        }
    } while (written_bytes < strlen(REQUEST));

    ESP_LOGI(TAG, "Reading HTTP response...");
    uint8_t num = 0;
    do {
        len = sizeof(buf) - 1;
        bzero(buf, sizeof(buf));
        ret = esp_tls_conn_read(tls, (char *)buf, len);
        num++;
        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE  || ret == ESP_TLS_ERR_SSL_WANT_READ) {
            continue;
        }

        if (ret < 0) {
            ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        }

        if (ret == 0) {
            ESP_LOGI(TAG, "connection closed");
            break;
        }

        len = ret;

        ESP_LOGW(TAG, "%d bytes read", len);
        if(num == 1){
            char code[5];
            sub_str(code,buf,9,13);
            int c = atoi(code);
            ESP_LOGW(TAG, "code:%d", c);
            if(c==200){
                ESP_LOGW(TAG, "code:OK");
                if(len>1000){
                    char* pa = strstr(buf, "{\"pem\":");
                    //ESP_LOGW(TAG, "co:%s",pa);
                    // for (int i = 0; i < len; i++) {
                    // putchar(buf[i]);
                    // }
                    //putchar('\n'); // JSON output doesn't have a newline at end
                    cJSON *pJsonRoot = cJSON_Parse(pa);
                //如果是否json格式数据
                    if (pJsonRoot !=NULL)
                    {                                
                       char* my_pems = cJSON_GetObjectItem(pJsonRoot, "pem")->valuestring;
                       char* my_pem_keys = cJSON_GetObjectItem(pJsonRoot, "key")->valuestring;
                        ESP_LOGW("pem_len","%d",strlen(my_pems));              
                        ESP_LOGW("pem_key_len","%d",strlen(my_pem_keys));

                        esp_storage_set("pem",(void *)my_pems, sizeof(my_pem));             
                        esp_storage_set("key",(void *)my_pem_keys, sizeof(my_pem_key));
                        esp_storage_set("client_id",(void *)client_name, sizeof(client_name));
                        esp_storage_set("iot_name",(void *)iot_name, sizeof(iot_name));

                        cJSON_Delete(pJsonRoot); 
                        creat_iot = 1; 
                    }
                    break;
                }
            }else{
                ESP_LOGW(TAG, "code:Bab");
                break;
            }
        }      
        /* Print response directly to stdout as it is read */
        if(len>1000){
            char* pa = strstr(buf, "{\"pem\":");
            cJSON *pJsonRoot = cJSON_Parse(pa);
                //如果是否json格式数据
            if (pJsonRoot !=NULL)
            {
               char* my_pems = cJSON_GetObjectItem(pJsonRoot, "pem")->valuestring;
               char* my_pem_keys = cJSON_GetObjectItem(pJsonRoot, "key")->valuestring;

                ESP_LOGW("pem_len","%d",strlen(my_pems));              
                ESP_LOGW("pem_key_len","%d",strlen(my_pem_keys));

                esp_storage_set("pem",(void *)my_pems, sizeof(my_pem));             
                esp_storage_set("key",(void *)my_pem_keys, sizeof(my_pem_key));
                esp_storage_set("client_id",(void *)client_name, sizeof(client_name));
                esp_storage_set("iot_name",(void *)iot_name, sizeof(iot_name));
                cJSON_Delete(pJsonRoot);  
                creat_iot= 1;
            }
            // for (int i = 0; i < len; i++) {
            //     putchar(buf[i]);
            // }
            // putchar('\n'); // JSON output doesn't have a newline at end
        }
       
    } while (1);

exit:
    esp_tls_conn_delete(tls);
}

static void https_get_request_using_crt_bundle(void)
{
   
    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_BT);
    ESP_LOG_BUFFER_HEX("mac:", mac, 6);
    sprintf(iot_name,"%x-%x-%x-%x-%x-%xabc10",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    sprintf(client_name,"id_%s",iot_name);
    static char body[50];
    sprintf(body,"{\"name\":\"%s\"}",iot_name);

    sprintf(iot_creat_post, "POST / HTTP/1.1\r\n"
 "Host: dgse52byjk4rtohou37yg4tjpa0uvnrw.lambda-url.us-east-1.on.aws\r\n"
 "Content-Type: application/json\r\n"
 "Content-Length: %d\r\n\r\n%s",strlen(body),body);
    
    ESP_LOGW("body",":%s",iot_creat_post);
    ESP_LOGI(TAG, "https_request using crt bundle");
    esp_tls_cfg_t cfg = {
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    https_get_request(cfg, WEB_URL, iot_creat_post);
}

void https_request_task(void *pvparameters)
{
    ESP_LOGI(TAG, "Start https_registered_mqtt");

    https_get_request_using_crt_bundle();

    ESP_LOGI(TAG, "Finish https_registered_mqtt");

    vTaskDelete(NULL);
}

