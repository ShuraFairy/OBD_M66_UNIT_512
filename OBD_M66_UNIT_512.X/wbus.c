/* 
 * File:   wbus.h
 * Author: LAB1
 *
 * Created on 27 февраля 2018 г., 22:36
 */

#include "wbus.h"

extern unsigned char __attribute__ ((far)) UART3_Tx_buf[32];
extern unsigned char __attribute__ ((far)) WBUS_Rx_copy_buf[40];
extern unsigned int U3_num_W_tmp;

void send_wbus_start(unsigned int time) { //start for [time] as Timer sender (0x34)
        UART3_Tx_buf[0] = 0x34;
        UART3_Tx_buf[1] = 0x03;
        UART3_Tx_buf[2] = 0x21; 
        //UART3_Tx_buf[3] = 0x1E; //time 30min
        UART3_Tx_buf[3] = time; //time in minutes
        //UART3_Tx_buf[4] = 0x08; //CRC for 30min
        UART3_Tx_buf[4] = wbus_CRC_calc(4);
        Nop();Nop();Nop();
        start_UART3_transmission(5);
}
void send_wbus_stop(void) {
        UART3_Tx_buf[0] = 0x34;
        UART3_Tx_buf[1] = 0x02;
        UART3_Tx_buf[2] = 0x10;
        UART3_Tx_buf[3] = 0x26;
        start_UART3_transmission(4);
}
void send_wbus_wakeup(void) { //send 34 04 44 21 00 55 response 43 03 C4 00 84
        UART3_Tx_buf[0] = 0x34;
        UART3_Tx_buf[1] = 0x04;
        UART3_Tx_buf[2] = 0x44;
        UART3_Tx_buf[3] = 0x21;
        UART3_Tx_buf[4] = 0x00;
        UART3_Tx_buf[5] = 0x55;
        start_UART3_transmission(6);
}
void send_wbus_op_state_request(void) { //0xf4 0x03 0x50 0x07 0xa0 
        UART3_Tx_buf[0] = 0xF4;
        UART3_Tx_buf[1] = 0x03;
        UART3_Tx_buf[2] = 0x50;
        UART3_Tx_buf[3] = 0x07;
        UART3_Tx_buf[4] = 0xA0;
        start_UART3_transmission(5);
}
void send_wbus_temp_status_request(void) { //0xf4 0x03 0x50 0x05 0xa2 
        UART3_Tx_buf[0] = 0xF4;
        UART3_Tx_buf[1] = 0x03;
        UART3_Tx_buf[2] = 0x50;
        UART3_Tx_buf[3] = 0x05;
        UART3_Tx_buf[4] = 0xA2;
        start_UART3_transmission(5);
}
void send_wbus_onoff_subsystems_request(void) {
        UART3_Tx_buf[0] = 0xF4;
        UART3_Tx_buf[1] = 0x03;
        UART3_Tx_buf[2] = 0x50;
        UART3_Tx_buf[3] = 0x03;
        UART3_Tx_buf[4] = 0xA4;
        start_UART3_transmission(5);
}
void send_wbus_errors_request(void) {
        UART3_Tx_buf[0] = 0xF4;
        UART3_Tx_buf[1] = 0x03;
        UART3_Tx_buf[2] = 0x56;
        UART3_Tx_buf[3] = 0x01;
        UART3_Tx_buf[4] = 0xA0;
        start_UART3_transmission(5);
}
void send_wbus_errors_clear(void) {
        UART3_Tx_buf[0] = 0xF4;
        UART3_Tx_buf[1] = 0x03;
        UART3_Tx_buf[2] = 0x56;
        UART3_Tx_buf[3] = 0x03;
        UART3_Tx_buf[4] = 0xA2;
        start_UART3_transmission(5);
}
unsigned char wbus_CRC_calc(unsigned char length) { //calc CRC for UART3_Tx_buf for [length] elements of array
    unsigned char wbus_CRC = 0;
    unsigned char i = 0;
    for (i = 0; i < length; i++) {
        wbus_CRC = wbus_CRC ^ UART3_Tx_buf[i]; //XOR all elements of packet 
    }
    return wbus_CRC;
}
unsigned char wbus_CRC_check(unsigned char length) {
    unsigned char wbus_CRC = 0;
    unsigned char i = 0;
    for (i = 0; i < length-1; i++) {
        wbus_CRC = wbus_CRC ^ WBUS_Rx_copy_buf[i]; //XOR all elements of packet 
    }
    if (wbus_CRC == WBUS_Rx_copy_buf[length-1]) return 1;
    else
        return 0;
}