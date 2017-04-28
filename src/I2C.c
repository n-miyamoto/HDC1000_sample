
/*******************************************************************************
 *
 *******************************************************************************/
#include <xc.h>
#include "I2C.h"

void I2C_init(void) {
    //I2Cの設定
    unsigned int config1 = 0x8000;
    unsigned int config2 = 157;
    OpenI2C1(config1, config2);
    IdleI2C1();
}

void I2C_start(void) {
    StartI2C1();
    //IFS1bits.MI2C1IF = 0;
    IdleI2C1();
}

void I2C_restart(void) {
    RestartI2C1();
    IdleI2C1();
}

void I2C_stop(void) {
    StopI2C1();
    //IFS1bits.MI2C1IF = 0;
    IdleI2C1();
}

int I2C_send(unsigned char data) {
    int ans = MasterWriteI2C1(data);
    while (I2C1STATbits.TBF) {
        //データの送信終了待ち
    }
    while (I2C1STATbits.ACKSTAT);
    //IFS1bits.MI2C1IF = 0;
    IdleI2C1();
    return ans;
}

unsigned char I2C_receive(int ack) {
    unsigned char data = MasterReadI2C1();
    if (ack)
        AckI2C1();
    else
        NotAckI2C1();
    IdleI2C1();
    return data;
}

int I2C_puts(unsigned int len, unsigned char *data) {
    int i;
    int n = 0;
    for (i = 0; i < len; ++i) {
        if (I2C_send(data[i]) == 0) {
            ++n;
        }
    }
    if (n == len) {
        return 0;
    }
    else {
        return -1;
    }
}

int I2C_gets(unsigned int len, unsigned char *data) {
    return MastergetsI2C1(len, data, 152);
}