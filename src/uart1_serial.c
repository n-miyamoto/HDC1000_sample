//
//  Copyright(c) 2017, SenSprout Inc.All rights reserved.
//

#include "uart1_serial.h"

#ifdef __XC32
#include <plib.h>
#endif

#ifdef __XC16
#include <xc.h>
#include "uart.h"
#include "delay.h"
#endif

#if defined(EMURATE_PC) || defined(TEST)
#include <stdio.h>
#include <string.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif
#if defined(unix) || defined(__APPLE__)
#include <pthread.h>
#include <unistd.h>
#endif

#define SERIAL_READ_BUF 80  //byte
#define SERIAL_SLEEP 20     //[ms]
#define SERIAL_TIMEOUT 3000 //[ms]
#define UART_CLK8MHZ_9600BPS 103 //set baurate to 9600
#define UART_CLK8MHZ_115200BPS 8 //set baurate to 115200  <-using

/*************************
 *global variable
 *************************/
static int serial_read_length = 0;
static char serial_read_buf[SERIAL_READ_BUF] = "";
static unsigned char serial_read_enter = 0;

/*************************
 *prototype
 *************************/
static void add_to_serial_buf(char chr);
static void serial_sleep(void);
static void uart1_device_init(void);
static void uart1_device_finit(void);

#ifdef __XC16
void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void);
#endif

/******************************
 * void uart_init(void);
 * initialize uart
 * port setting -> rx:RP5 tx:RP0
 *******************************/
void uart1_init(void) {
    serial_read_buf[0] = 0;
    serial_read_length = 0;
    serial_read_enter = 0;

    uart1_device_init();
}

/******************************
 * void uart_device_init(void);
 * initialize uart
 * port setting -> rx:RP5 tx:RP0
 *******************************/
static void uart1_device_init(void) {
#ifdef __XC16
    //port settings
    RPINR18bits.U1RXR = 2; //rx UART1 RP2
    RPOR1bits.RP3R = 3; //tx UART1
    CloseUART1();

    unsigned int config1 = UART_EN & // UART enable
            UART_IDLE_CON & // operete in Idle mode
            UART_DIS_WAKE & // Don't wake up in sleep mode
            UART_IrDA_DISABLE &
            UART_DIS_LOOPBACK & // don't loop back
            UART_NO_PAR_8BIT & // No parity bit, 8bit
            UART_1STOPBIT & //Stop bit
            UART_MODE_SIMPLEX & //no flow control
            UART_UEN_00 &
            UART_UXRX_IDLE_ONE & // U1RX idle 1
            UART_BRGH_SIXTEEN & // invalid high speed transport
            UART_DIS_ABAUD; // disable audo baud

    unsigned int config2 = UART_INT_TX_BUF_EMPTY & // TXD interrupt on
            UART_IrDA_POL_INV_ZERO & // U1TX idle clear
            UART_TX_ENABLE & // Enable TXD
            UART_INT_RX_CHAR & // RXD interrupt on
            UART_ADR_DETECT_DIS &
            UART_RX_OVERRUN_CLEAR;


    OpenUART1(config1, config2, UART_CLK8MHZ_115200BPS);
    ConfigIntUART1(UART_RX_INT_EN & UART_TX_INT_DIS);
    IEC0bits.U1RXIE = 1; //enable interrupt
#endif
}

/******************************
 * void uart_device_finit(void);
 * finalize uart
 * port setting -> rx:RP5 tx:RP0
 *******************************/
static void uart1_device_finit(void) {
#ifdef __XC16
    IEC0bits.U1RXIE = 0; //disable interrupt
    CloseUART1();
#endif 
}

/******************************
 * void uart1_finalize(void);
 * finalize uart
 * port setting -> rx:RP5 tx:RP0
 *******************************/
void uart1_finit(void) {
    uart1_device_finit();
}

/****************************
 * _U1RXInterrupt(void);
 * UART receive interrupt
 * not completed
 *****************************/
#ifdef XC16

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;
    char chr = ReadUART1();
    add_to_serial_buf(chr);
}
#endif

/****************************
 * add_to_serial_buf(void)
 *****************************/
static void add_to_serial_buf(char chr) {
    if (serial_read_enter)return;
    if (chr == '\r' || chr == '\n') {
        if (serial_read_length > 0)serial_read_enter = 1;
        return;
    }
    if (serial_read_length + 1 >= SERIAL_READ_BUF)return;

    serial_read_buf[serial_read_length] = chr;
    serial_read_buf[serial_read_length + 1] = 0;
    serial_read_length += 1;
    return;
}

/******************************
 * read_uart1(char* cmd, int size);
 * 0<timeout
 * 1>readlength
 ******************************/
int read_uart1(char *cmd, int sz) {
#ifdef EMURATE_PC
    cmd[0] = 0;
    fgets(cmd, sz, stdin);
    return strlen(cmd);
#else
    int ct = 0;
    int tmp = 0, i;
    cmd[0] = 0;
    char *p, *q;
    int ret = -1;
    while (ct < SERIAL_TIMEOUT / SERIAL_SLEEP) {
        if (tmp != serial_read_length) {
            ct = 0;
            tmp = serial_read_length;

        }
        if (serial_read_enter)break;
        serial_sleep();
        ct++;
    }
    if (serial_read_enter == 0)goto end; //TIMEOUT
    tmp = sz - 1;
    if (serial_read_length < tmp)tmp = serial_read_length;
    for (i = 0, p = cmd, q = serial_read_buf; i < tmp; i++)*p++ = *q++;
    *p = 0;
    ret = tmp;
end:
    serial_read_buf[0] = 0;
    serial_read_length = 0;
    serial_read_enter = 0;
    return ret;
#endif
}

/******************************
 * write_uart1(char* cmd, int size);
 * 0< error
 * 1> write length
 ******************************/
void write_uart1(char *cmd) {
#ifdef __XC16
    putsUART1((unsigned int *) cmd);
#endif
#ifdef EMURATE_PC
    printf("write_uart1>>%s<<\n", cmd);
#endif
}

/********************************
 *serial_sleep();
 ********************************/
void serial_sleep(void) {
#ifdef _WIN32
    Sleep(SERIAL_SLEEP);
#endif
#if defined(unix) || defined(__APPLE__)
    sleep(SERIAL_SLEEP / 1000);
#endif
#ifdef __XC16
    delay_ms(SERIAL_SLEEP);
#endif
}

void clear_read_buf_uart1(void) {
    serial_read_buf[0] = 0;
    serial_read_length = 0;
    serial_read_enter = 0;
#ifdef EMURATE_PC
    printf("__UART1 recv buffer cleared__\n");
#endif
}


/*****************************
 * TEST CODE
 * return -1: error
 * return 0 :GOOD 
 *****************************/
#ifdef TEST

static void* dummy_intrpt_thread(void *args) {
    while (1) {
        char chr = getchar();
        add_to_serial_buf(chr);
    }
    return NULL;
}

static int check(char *answer, char *input_line) {
    int i;
    for (i = 0; i < SERIAL_READ_BUF; i++) {
        if (input_line[i] == 0)break;
        else add_to_serial_buf(input_line[i]);
    }

    char result[SERIAL_READ_BUF];
    if (serial_read_enter == 0) return -1;

    return strcmp(answer, serial_read_buf);
}

static int check_serial_read(char *answer) {
    printf("please enter [%s+CRLF]\r\n", answer);
#ifdef _WIN32
    Sleep(5);
#else
    sleep(5);
#endif
    char chr[SERIAL_READ_BUF];
    printf("please enter [dummy+CRLF]\r\n");
#ifdef _WIN32
    Sleep(5);
#else
    sleep(5);
#endif
    if (0 > read_uart1(chr, SERIAL_READ_BUF)) return -1;
    return strcmp(answer, chr);
}

static int check_serial_read_TIMEOUT(void) {
    char chr[SERIAL_READ_BUF];
    if (0 > read_uart1(chr, SERIAL_READ_BUF)) return 0;
    else return -1;
}

void test_uart1_serial(void) {

    //test CRLF
    printf("test CRLF:");
    if (check("test_line1", "test_line1\r\n") == -1) printf("NG\r\n");
    else printf("OK\r\n");
    //test CR
    printf("test CR:");
    if (check("test_line1", "test_line1\n") == -1) printf("NG\r\n");
    else printf("OK\r\n");
    //test CR
    printf("test LF:");
    if (check("test_line1", "test_line1\r") == -1) printf("NG\r\n");
    else printf("OK\r\n");
    //test
    printf("test 2 lines CRLF:");
    if (check("test_line2", "test_line2\r\ntest_line1\r\n") == -1) printf("NG\r\n");
    else printf("OK\r\n");
    //test
    printf("test 2 lines CR:");
    if (check("test_line2", "test_line2\rtest_line1\r\n") == -1) printf("NG\r\n");
    else printf("OK\r\n");
    //test
    printf("test 2 lines LF:");
    if (check("test_line2", "test_line2\ntest_line1\r\n") == -1) printf("NG\r\n");
    else printf("OK\r\n");

    printf("\r\ntest_read_serial\r\n");



    //test read_serial
    char dummy[SERIAL_READ_BUF];
    read_uart1(dummy, SERIAL_READ_BUF);

    int result = 1;
    if (-1 == check_serial_read("test")) result = -1;
    printf("test read_serial:");
    if (result == -1)printf("NG\r\n");
    else printf("OK\r\n");

    //test read_serial
#ifdef _WIN32
    Sleep(5);
#else
    sleep(5);
#endif
    result = 1;
    if (-1 == check_serial_read("test2")) result = -1;
    printf("test read_serial:");
    if (result == -1)printf("NG\r\n");
    else printf("OK\r\n");


    //test TIMEOUT 
    printf("test TIMEOUT:");
    if (check_serial_read_TIMEOUT() == -1) printf("NG\r\n");
    else printf("OK\r\n");

}

#ifdef STANDALONE

int main(void) {
#ifdef _WIN32
    HANDLE th;
    unsigned long id;
    th = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) dummy_intrpt_thread, NULL, 0, &id);
#else
    pthread_t th;
    pthread_create(&th, NULL, dummy_intrpt_thread, (void *) NULL);
#endif

    printf("--START TEST CODE--\r\n");
    test_uart1_serial();
    printf("--FINISH TEST--\r\n");
    while (1);
}
#endif
#endif
