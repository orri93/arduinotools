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
 * 
 * Display Wiring  Display   Arduino UNO
 *                 1 SDA     A4 (SDA)
 *                 2 SCL     A5 (SCL)
 *                 3 VCC     5V
 *                 4 GND     GND
 */


#define PIN_RS485_MODBUS_RX     0
#define PIN_RS485_MODBUS_TX     1
#define PIN_RS485_MODBUS_TE     2

#define PIN_LED_MODBUS_READ     9  /* PWM */
#define PIN_LED_MODBUS_WRITE   10  /* PWM */

#define PIN_COIL_LED_1          3  /* PWM */
#define PIN_COIL_LED_2         11  /* PWM */

#define PIN_INDICATION_I2C_SDA A4
#define PIN_INDICATION_I2C_SCL A5

#define PIN_DISCRETE_INPUTS_1   4
#define PIN_DISCRETE_INPUTS_2   7

#define PIN_INPUT_REGISTRY_1   A0
#define PIN_INPUT_REGISTRY_2   A1

#define PIN_HOLDING_REGISTRY_1  5  /* PWM */
#define PIN_HOLDING_REGISTRY_2  6  /* PWM */




#endif
