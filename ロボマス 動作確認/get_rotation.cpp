//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////ヒューズを挟むこと！！！！///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// m2006は小さいほう(0) m3508は大きいほう(1)
// 一つのCANで同種類のモーター8個まで制御可能
// 速度制御の時は同時に４つまでにしておくこと

#include "mbed.h"
#include "rbms.h"

#define STOP_ANGLE 1080

UnbufferedSerial pc(USBTX, USBRX, 115200);
CAN can(p30, p29);
CANMessage msg;
rbms m2006(can, 0, 1); 
int _m2006[1] = {0}; 
short rotation = 0;
short speed = 0;

int main(){
    _m2006[0] =  3000;
    while (true) {  
        m2006.rbms_send(_m2006);
        m2006.rbms_read(msg, &rotation, &speed);
        printf("send_succeed | rotation:%hd°", rotation);
        // 指定した回転数（度）だけ回ると止まる
        // if (rotation == STOP_ANGLE){
        //     _m2006[0] = 0;
        //     m2006.rbms_send(_m2006);
        //     printf("rbms has stopped : rotated %d°\r\n", STOP_ANGLE);
        // }
        ThisThread::sleep_for(25ms);
    }
}