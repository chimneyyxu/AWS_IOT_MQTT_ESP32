/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Attributes State Machine */
enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,        //1是pem key已准备好

    IDX_CHAR_B,            //pem1
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,            //pem1
    IDX_CHAR_VAL_C,

    IDX_CHAR_D,            //pem3
    IDX_CHAR_VAL_D,

    IDX_CHAR_E,           //key1
    IDX_CHAR_VAL_E, 

    IDX_CHAR_F,           //key2
    IDX_CHAR_VAL_F,

    IDX_CHAR_G,           //key3
    IDX_CHAR_VAL_G,

    IDX_CHAR_H,          //key4
    IDX_CHAR_VAL_H,

    IDX_CHAR_J,          //client_names
    IDX_CHAR_VAL_J,

    HRS_IDX_NB,
};

extern char iot_pem[1350];
extern char iot_pem_key[1800];
extern char iot_names[30];
extern char client_names[35];

void ble_start(void);