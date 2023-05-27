#ifndef MQTT_DEMO_MUTUAL_AUTH_H_
#define MQTT_DEMO_MUTUAL_AUTH_H_
#include "driver/gpio.h"

#define LED1 GPIO_NUM_18
#define LED2 GPIO_NUM_17
#define LED3 GPIO_NUM_16

#define LEDNUMS ((1ULL<<LED1)|(1ULL<<LED2)|(1ULL<<LED3))

int aws_iot_demo_main( int argc,char ** argv );


#endif