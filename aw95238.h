#ifndef _AW95238_H_
#define _AW95238_H_

#include "fsl_common.h"

#define AW95238_I2C_ADDRESS (0x5b)//101 10xx,从原理图上来看，AD0 AD1接的电源，即101 1011
#define KEYBOARD_I2C_ADDRESS AW95238_I2C_ADDRESS

/*config port0&1 for outout or input */
#define CONFIG_PORT0 0x04
#define CONFIG_PORT1 0x05

/*read port0&1 can get status of port0&1*/
#define INPUT_PORT0 0x00
#define INPUT_PORT1 0x01

/*config for port0&1 output state*/
#define OUTPUT_PORT0 0x02// 设为 0xFF
#define OUTPUT_PORT1 0X03// 设为 0xFF

/*config for led mode switch*/
#define LED_MODE_PORT0 0x12
#define LED_MODE_PORT1 0x13

/*config for led current */
#define LED_DIM_PORT1_0 0x20 //DIM0 对应p1_0
#define LED_DIM_PORT1_1 0x21 //DIM1 对应p1_1
#define LED_DIM_PORT0_0 0x24 //DIM4 对应p0_0
#define LED_DIM_PORT1_1 0x25 //DIM5 对应p0_1

/*config interrupts for port0&1*/
#define INT_PORT0 0x06
#define INT_PORT1 0x07

#define SW_RSTN 0x7F
#define AW95238_ID 0x10 //id_pin is read_only and value is 0x23H

#define BUTTON_DATA_LEN 2


#define kButton_Up 0x
#define kButton_Down 0x
#define kButton_Left 0x
#define kButton_Right 0x
#define kButton_Start_Pause 0x
#define kButton_Back 0x
#define kButton_Home 0x
#define kButton_Volume_Up 0x
#define kButton_Volume_Down 0x
#define kButton_Channel_Up 0x
#define kButton_Channel_Down 0x
#define kButton_Mic 0x



typedef enum _button_event {
  kButton_Up = 0,
  kButton_Down,
  kButton_Left,
  kButton_Right,
  kButton_Start_Pause,
  kButton_Back,
  kButton_Home,
  kButton_Volume_Up,
  kButton_Volume_Down,
  kButton_Channel_Up,
  kButton_Channel_Down,
  kButton_Mic,
} button_event_t;

typedef struct _aw95238_handle {
  I2C_Type *base;
  i2c_master_transfer_t xfer;
  uint8_t button_buf[BUTTON_DATA_LEN];
}keyborad_handle_t;

/*
typedef struct _aw95238_button_data {
  button_event_t button_event;
} keyboard_button_data_t;
*/

keyborad_handle_t button_handle;


status_t  AW95238_Init(keyborad_handle_t *handle, I2C_Type *base);

status_t AW95238_Denit(keyborad_handle_t *handle);

status_t AW95238_GetSingleButton(keyborad_handle_t *handle, button_event_t *button_event);
