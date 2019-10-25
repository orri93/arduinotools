#ifndef FDS_MODBUS_TEST_SLAVE_PIN_UNO_H_
#define FDS_MODBUS_TEST_SLAVE_PIN_UNO_H_

/*
*  ARDUINO UNO
*/
/* Serial: 0 (RX) and 1 (TX). Used to receive (RX) and transmit (TX) TTL serial data.
 */
/* PWM: 3, 5, 6, 9, 10, and 11. Provide 8-bit PWM output with the analogWrite function. */

/*
 * MAX485 Wiring   Max485    Arduino UNO
 *                 1 RO      0 RX
 *                 2 RE      2 DI
 *                 3 DE      2 DI
 *                 4 DI      1 TX
 *                 5 GND       GND
 *                 6 A
 *                 7 B
 *                 8 VCC       5V
 */


#define PIN_RS485_MODBUS_RX     0
#define PIN_RS485_MODBUS_TX     1
#define PIN_RS485_MODBUS_TE     2

#define PIN_COIL_LED_1          LED_BUILTIN

#define PIN_INPUT_REGISTRY_1   A0
#define PIN_INPUT_REGISTRY_2   A1

#endif
