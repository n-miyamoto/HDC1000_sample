/*
 * File:   set_uart.h
 * Author: Naoy_Miyamoto
 *
 * Created on 2015/05/13, 2:00
 *
 */

#ifndef SET_UART1_H
#define	SET_UART1_H

#ifdef	__cplusplus
extern "C" {
#endif

    /****************************
     * void uart_init(void);
     * initialize uart
     * port setting -> rx:RP5 tx:RP0
     *****************************/
    void uart1_init(void);

    /****************************
     * int read_uart1(char *cmd,int sz);
     * return ret<0 : TIMEOUT or error
     * return ret > 0 : string length 
     *****************************/
    int read_uart1(char *cmd, int sz);

    /******************************
     * write_uart1(char* cmd);
     ******************************/
    void write_uart1(char *cmd);

    /****************************
     * void uart_finit(void);
     * initialize uart
     * port setting -> rx:RP5 tx:RP0
     *****************************/
    void uart1_finit(void);

    /****************************
     * void clear_read_buf_uart1(void);
     * Initialize receive buffer of UART1.
     *****************************/
    void clear_read_buf_uart1(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SET_UART_H */
