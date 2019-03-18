/* 
 * File:   wbus.h
 * Author: LAB1
 *
 * Created on 27 февраля 2018 г., 22:36
 */
#include <p33EP256MU806.h>

#define WBUS_SPEED 2400
#define WEBASTO_t_offset 50
/* Function Prototype 	*/

void send_wbus_start(unsigned int time);
void send_wbus_stop(void);
void send_wbus_wakeup(void);
void send_wbus_temp_status_request(void);
void send_wbus_op_state_request(void);
void send_wbus_onoff_subsystems_request(void);
void send_wbus_errors_request(void);
void send_wbus_errors_clear(void);
unsigned char wbus_CRC_calc(unsigned char length);
unsigned char wbus_CRC_check(unsigned char length);