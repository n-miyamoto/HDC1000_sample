/* 
 * File:   I2C.h
 * Author: NAOYA
 *
 * Created on 2017/04/05, 14:59
 */

#ifndef MYI2C_H
#define	MYI2C_H

#ifdef	__cplusplus
extern "C" {
#endif

    // 定数の定義
#define ACK   0
#define NOACK 1
#define RW_0  0
#define RW_1  1

#include <i2c.h>

void I2C_init(void);
void I2C_start(void);
void I2C_restart(void);
void I2C_stop(void);
// I2C_send
// returns 0 if succeed
int I2C_send(unsigned char data);
// I2C_receive
// returns received value
unsigned char I2C_receive(int ack);
// I2C_puts
// returns 0 if succeed
int I2C_puts(unsigned int len, unsigned char *data);
// I2C_gets
// returns 0 if succeed
int I2C_gets(unsigned int len, unsigned char *data);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

