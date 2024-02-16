// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////ヒューズを挟むこと！！！！///////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// // m2006は小さいほう(0) m3508は大きいほう(1)
// // 一つのCANで同種類のモーター8個まで制御可能
// // 速度制御の時は同時に４つまでにしておくこと
// // 角度取得はThreadを立てて個別にcan.read(msg)



#include "mbed.h"
#include "robo_ms.h"
#include "rote_robo_ms.h"
#include "rbms.h"

#define BUFFER_MAX 4

CAN can(p30, p29);
CANMessage msg, buffer[BUFFER_MAX];
rbms rbMs(can, 0, 1);
UnbufferedSerial pc(USBTX, USBRX, 115200);
rote_robo_ms rote_robo_ms_motor0(can, rbMs, 0, 16);
rote_robo_ms rote_robo_ms_motor1(can, rbMs, 0, 16);
Thread thread1, thread2, thread3;

void motor0_func();
void motor1_func();
void can_receive();
int motor[2] = {0};

int main(){
    thread1.start(motor0_func);
    thread2.start(motor1_func);
    thread3.start(can_receive);
    rote_robo_ms_motor0.set_static_reset(0);
    motor[0] = 2000; motor[1] = 1000;
    while(true){
        ThisThread::sleep_for(10ms);
    }
}

void motor0_func(){
    while (1){
    // printf("%d\n",rote_robo_ms_motor0.x);
        rote_robo_ms_motor0.rote_robo_ms_update(buffer, BUFFER_MAX);
        rote_robo_ms_motor0.rote_robo_ms_update_write(&motor[0], 2);
        rbMs.rbms_send(motor);
        printf("motor0_rote:%ld\r\n",rote_robo_ms_motor0.get_rote_sum());
        ThisThread::sleep_for(2ms);
    }  
}

void motor1_func(){
    while(1){
        rote_robo_ms_motor1.rote_robo_ms_update(buffer, BUFFER_MAX);
        rote_robo_ms_motor1.rote_robo_ms_update_write(&motor[1], 2);
        rbMs.rbms_send(motor);
        printf("motor1_rote:%ld\r\n\n\n",rote_robo_ms_motor1.get_rote_sum());
        ThisThread::sleep_for(2ms);
    }
}

void can_receive(){
    while(1){
        if(can.read(msg)){
            for(int i = 1; i < BUFFER_MAX; i++){
                buffer[i - 1] = buffer[i];
            }
            buffer[BUFFER_MAX - 1] = msg;
        }
    }
}

// #include "mbed.h"
// #include "robo_ms.h"
// #include "rote_robo_ms.h"
// #include "rbms.h"

// #define BUFFER_MAX 4

// UnbufferedSerial pc(USBTX, USBRX, 115200);
// CANMessage  _msg[BUFFER_MAX];
// CAN can(p30, p29);
// robo_ms robo(can);
// rote_robo_ms rote(can, robo, 0, 12);
// rbms m2006(can, 0, 1);
// Thread thread2;

// int _m2006 = 0; // 実際に制御するのは1つだけ
// long rotation = 0;
// short speed = 0;
// int cnt = 0;

// // Thread用関数
// void thread_read();

// int main(){ // モーターの長さ7540mmは100回転の時の回転長
//     _m2006 = 0;
//     thread2.start(thread_read);
//     while (true) {
//         m2006.rbms_send(&_m2006);
//         ThisThread::sleep_for(25ms);
//     }
// }

// void thread_read(){
//     while(true){
//         rote.rote_robo_ms_update(_msg, BUFFER_MAX);
//         rotation = rote.get_rote_sum();
//         //cnt = rotation / 
//         printf("rotation:%ld\r\n", rotation);
//     }
// }





// // #include "mbed.h"
// // #include "rbms.h"

// // UnbufferedSerial pc(USBTX, USBRX, 115200);
// // CANMessage  _msg;
// // CAN can(p30, p29);
// // rbms m2006(can, 0, 1);
// // Thread thread1, thread2, thread3;

// // int _m2006 = 0; // 実際に制御するのは1つだけ
// // short rotation = 0;
// // short speed = 0;

// // // Thread用関数
// // void thread_get();
// // void thread_read();
// // void thread_display();

// // int main(){ // モーターの長さ7540mmは100回転の時の回転長
// //     _m2006 = 0;
// //     thread1.start(thread_get);
// //     thread2.start(thread_read);
// //     thread3.start(thread_display);
// //     while (true) {
// //         m2006.rbms_send(&_m2006);
// //         ThisThread::sleep_for(25ms);
// //     }
// // }

// // void thread_get(){
// //     while(true){
// //         can.read(_msg);
// //     }
// // }

// // void thread_read(){
// //     while(true){
// //         m2006.rbms_read(_msg, &rotation, &speed);
// //     }
// // }

// // void thread_display(){
// //     while(true){
// //         printf("rotation:%hd\r\n", rotation);
// //     }
// // }

//         // robomas.rote_robo_ms_update(&_msg, 4);
//         // rotation = robomas.get_rote_sum();
//         // printf("rotation:%ld\r\n", rotation);

//         // m2006.can_data_read(_msg, &rote, &spd);
//         // deltaR = rote - tmpR;
//         // rotation += deltaR;
//         // cnt = rotation / 360;
//         // tmpR = rote;

//         // // rotation
//         // msg[0] = (_msg.data[0] << 8) | (_msg.data[1] & 0xff);
//         // // speed
//         // msg[1] = (_msg.data[2] << 8) | (_msg.data[3] & 0xff);
//         // // torpue
//         // msg[2] = (_msg.data[4] << 8) | (_msg.data[5] & 0xff);
//         // // temperature
//         // msg[3] = _msg.data[6];

//         // rotation = msg[0] / 8192 * 360;

//         // int msgnum = 0x202; // 初期値0x202
//         // for (int i = 0; i < 4; i++)
//         // {
//         //     if (msg[i].id == msgnum)
//         //         _msg = msg[i];
//         // }


