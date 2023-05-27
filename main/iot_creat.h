#ifndef IOT_CREAT_H_
#define IOT_CREAT_H_

#define WEB_SERVER "dgse52byjk4rtohou37yg4tjpa0uvnrw.lambda-url.us-east-1.on.aws"
#define WEB_PORT "443"
#define WEB_URL "https://dgse52byjk4rtohou37yg4tjpa0uvnrw.lambda-url.us-east-1.on.aws"

#define SERVER_URL_MAX_SZ 256

// extern char *my_pem;
// extern char *my_pem_key;
// extern char iot_name[30];
// extern char client_name[35];
extern uint8_t creat_iot;
void https_request_task(void *pvparameters);


#endif