#ifndef _FDS_RS485_USB_PIN_UNO_H_
#define _FDS_RS485_USB_PIN_UNO_H_

// 4->RE (MAX485 PIN 2), 7->DE (MAX485 PIN 3)
/* Driver Enable set to HIGH to enable transmitting */
//#define PIN_RS485_RE    8
/* Receiver Enable set to LOW to enable receive */
//#define PIN_RS485_DE    7

// Same pin for RE and DE 7->RE (MAX485 PIN 2), 7->DE (MAX485 PIN 3)
#define PIN_RS485_TE    8

// RX(10)->RO (MAX845 PIN 1), TX(11)->DI (MAX845 PIN 4)
#define PIN_SOFT_RX_RO  10
#define PIN_SOFT_TX_DI   9

#define PIN_INDICATOR_LED_DIRECTION_A 5
#define PIN_INDICATOR_LED_DIRECTION_B 6

#endif
