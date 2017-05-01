
/*******************************************************************************
 *
 *******************************************************************************/
#ifdef __XC16
#include <xc.h>
#include <i2c.h>
#endif

#include "I2C.h"

#ifdef EMURATE_PC
#include <stdio.h>
#endif



void I2C_init(void) {
    //I2Cの設定
    unsigned int config1 = 0x8000;
    unsigned int config2 = 157;
#ifdef __XC16
    OpenI2C1(config1, config2);
    IdleI2C1();
#elif defined EMURATE_PC
    printf(">>>I2C_init\r\n");
#endif
    
}

void I2C_start(void) {
#ifdef __XC16
    StartI2C1();
    //IFS1bits.MI2C1IF = 0;
    IdleI2C1();
#elif defined EMURATE_PC
    printf(">>>I2C_start\r\n");
#endif
}

void I2C_restart(void) {
#ifdef __XC16
    RestartI2C1();
    IdleI2C1();
#elif defined EMURATE_PC
    printf(">>>I2C_restart\r\n");
#endif
}

void I2C_stop(void) {
#ifdef __XC16
    StopI2C1();
    //IFS1bits.MI2C1IF = 0;
    IdleI2C1();
#elif defined EMURATE_PC
    printf(">>>I2C_stop\r\n");
#endif
}

int I2C_send(unsigned char data) {
#ifdef __XC16
    int ans = MasterWriteI2C1(data);
    while (I2C1STATbits.TBF) {
        //データの送信終了待ち
    }
    while (I2C1STATbits.ACKSTAT);
    //IFS1bits.MI2C1IF = 0;
    IdleI2C1();
#elif defined EMURATE_PC
    
    printf(">>>I2C_send. data:%u\r\n",data);
    int ans = 0;
#endif
    return ans;
}

unsigned char I2C_receive(int ack) {
#ifdef __XC16
    unsigned char data = MasterReadI2C1();
    if (ack)
        AckI2C1();
    else
        NotAckI2C1();
    IdleI2C1();
#elif defined EMURATE_PC
    printf(">>>I2C_receive. ack:%d\r\n",ack);
    unsigned char data = 0;
#endif
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
#ifdef __XC16
    return MastergetsI2C1(len, data, 152);
#elif defined EMURATE_PC
    printf(">>>I2C_gets. len:%u\r\n",len);
    return 1;
#endif
}


#ifdef EMURATE_PC
int main(void){
    I2C_init();
    I2C_start();
    I2C_stop();
    I2C_receive(ACK);
    int length = 3;
    unsigned char test[3]={1,10,0xFF};
    I2C_puts(3,test);
    I2C_gets(3,test);
    return 0;
}
#endif