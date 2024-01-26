//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////ヒューズを挟むこと！！！！///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// m2006は小さいほう(0) m3508は大きいほう(1)
// 一つのCANで同種類のモーター8個まで制御可能
// 速度制御の時は同時に４つまでにしておくこと
#include "mbed.h"
#include "rbms.h"

const short stop_angle = 359; // 例として20回転

UnbufferedSerial pc(USBTX, USBRX, 115200);
CANMessage msg;

CAN can(p30, p29);
rbms m2006(can, 0, 1); 

int _m2006[1] = {0}; 
short rotation = 60;
short speed = 0;

int main(){
    _m2006[0] =  100;
    while (true) {
        can.read(msg);
        m2006.rbms_send(_m2006);
        if (m2006.rbms_read(msg, &rotation, &speed)) {
            // rbms_readが正常にデータを取得できた場合
            printf("send_succeed | rotation:%hd°\r\n", rotation);
            if (rotation == stop_angle){ // 回転を止めることはできたが、printf("rbms has...の表示ができなかった。でもOKの出力ができなかった
                _m2006[0] = 0;
                printf("OK");
                m2006.rbms_send(_m2006);
                printf("rbms has stopped : rotated %d°\r\n", stop_angle);
            }
        } else {
            // rbms_readがエラーを返した場合
            printf("Error reading rbms data\r\n");
        }
        ThisThread::sleep_for(25ms);
    }
    // while (true) {  
    //     can.read(msg);
    //     m2006.rbms_send(_m2006);
    //     m2006.rbms_read(msg, &rotation, &speed);
    //     printf("send_succeed | rotation:%hd°\r\n", rotation);
    //
    //     if (rotation == stop_angle){ // 指定した回転数（度）だけ回ると止まる
    //         _m2006[0] = 0;
    //         m2006.rbms_send(_m2006);
    //         printf("rbms has stopped : rotated %d°\r\n", stop_angle);
    //     }
    //     ThisThread::sleep_for(25ms);
    // }
}
