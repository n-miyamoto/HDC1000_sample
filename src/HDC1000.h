/* 
 * File:   HDC1000.h
 * Author: NAOYA
 *
 * Created on 2017/04/05, 14:53
 */

#ifndef HDC1000_H
#define	HDC1000_H

#ifdef	__cplusplus
extern "C" {
#endif


#ifndef _XTAL_FREQ
    // Unless already defined assume 8MHz system frequency
    // This definition is required to calibrate __delay_us() and __delay_ms()
#define _XTAL_FREQ 16000000   // 使用するPIC等により動作周波数値を設定する
#endif

#define HDC1000_ADRS 0x40     // HDC1000のI2Cアドレス
#define HDC1000_ID      0x1000     // HDC1000のID

    // デバイスのレジスタアドレス
#define DEVICE_ID_ADRS  0xFF       // HDC1000のデバイスID格納アドレス
#define CONFIG_ADRS     0x02       // Configurationレジスタのアドレス
#define TEMP_ADRS       0x00       // 温度読出しのレジスタのアドレス
#define HUMI_ADRS       0x01       // 湿度読出しのレジスタのアドレス

    // Configurationレジスタの設定
#define CONFIG_DT_H     0b10000000 // 温度(14bit)･湿度(14bit)個別に読出し
#define CONFIG_DT_L     0b00000000 // 予約の為に全て０




    // 関数のプロトタイプ宣言
    int HDC_Init(int id, int address);
    int HDC_Receive(char reg_adrs, int len, unsigned char *data);
    int HDC_Send(char reg_adrs, unsigned char *data);
    int HDC_Read();

    double HDC_GetTemp();
    double HDC_GetHumi();

    unsigned int HDC_GetTempRaw();
    unsigned int HDC_GetHumiRaw();



#ifdef	__cplusplus
}
#endif

#endif	/* HDC1000_H */

