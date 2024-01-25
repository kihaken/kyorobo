//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////ロボマスの電源とモーターの間にヒューズを挟む//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// m2006は小さいほう(0) m3508は大きいほう(1)
// 一つのCANのGPIOピン(?)で同種類のモーター8個まで制御可能
// 速度制御の時は同時に４つまでにしておくこと
// ロボマスは制御信号が100ms来ないと自動で停止

#include "mbed.h"
#include "rbms.h"

#define CRAWLER 3000
#define ORB 1500
#define KEY 2000
#define DATA_SIZE 8

UnbufferedSerial pc(USBTX, USBRX, 115200);
UnbufferedSerial ps3(p10, p11, 2400);
CAN can(p30, p29);
CANMessage msg;
rbms crawler(can, 0, 4);
rbms orb(can, 0, 1);
rbms key(can, 0, 1);


void reference();
void front();
void back();
void left();
void right();
void turn_left();
void turn_right();
void brake();
void orb_drop();
void key_catch();
void key_release();

int i;
int _orb[1] = {0}; 
int _key[1] = {0};
int _crawler[4] = {0};
short rotation = 0;
short speed = 0;
int data[8] = {0};


int main(){
    _orb[0] =  3000;
    while (true) {  
        if (ps3.readable()) {
            for (i = 0; i < DATA_SIZE; i++) {
                data[i] = ps3.read(&data[i], 1);
            }
            reference();
        }
        if (pc.readable()) {
            char getc;
            pc.read(&getc, 2);
            switch(getc){
                case 'w': front(); printf("front\r\n"); break;
                case 'a': left(); printf("left\r\n"); break;
                case 's': back(); printf("back\r\n"); break;
                case 'd': right(); printf("right\r\n"); break;
                case 'q': turn_left(); printf("turn_left\r\n"); break;
                case 'e': turn_right(); printf("turn_right\r\n"); break;
                case 'x': orb_drop(); printf("orb_drop\r\n"); break;
                case 'z': key_catch(); printf("key_catch\r\n"); break;
                case 'c': key_release(); printf("key_release\r\n"); break;
            }
            crawler.rbms_send(_crawler);
            orb.rbms_send(_orb);
            key.rbms_send(_key);
        }
        
        ThisThread::sleep_for(25ms);
    }
}

void front(){
    _crawler[0] =  CRAWLER;
    _crawler[1] = -CRAWLER;
    _crawler[2] = -CRAWLER;
    _crawler[3] =  CRAWLER;
}

void back(){
    _crawler[0] = -CRAWLER;
    _crawler[1] =  CRAWLER;
    _crawler[2] =  CRAWLER;
    _crawler[3] = -CRAWLER;
}

void left(){
    _crawler[0] = -CRAWLER;
    _crawler[1] =  CRAWLER;
    _crawler[2] = -CRAWLER;
    _crawler[3] =  CRAWLER;
}

void right(){
    _crawler[0] =  CRAWLER;
    _crawler[1] = -CRAWLER;
    _crawler[2] =  CRAWLER;
    _crawler[3] = -CRAWLER;
}

void turn_left(){
    _crawler[0] = -CRAWLER;
    _crawler[1] = -CRAWLER;
    _crawler[2] = -CRAWLER;
    _crawler[3] = -CRAWLER; 
}

void turn_right(){
    _crawler[0] = CRAWLER;
    _crawler[1] = CRAWLER;
    _crawler[2] = CRAWLER;
    _crawler[3] = CRAWLER; 
}

void brake(){
    _crawler[0] = 0;
    _crawler[1] = 0;
    _crawler[2] = 0;
    _crawler[3] = 0;
}

void orb_drop(){
    _orb[0] = ORB;
    // ここに角度取得も含めたプログラムを記述
    orb.rbms_read(msg, &rotation, &speed);
    if (rotation == 60) {
        _orb[0] = 0;
    }
}

void key_catch(){
    _key[0] = KEY;
}

void key_release(){
    _key[0] = -KEY;
}

void reference() {
    if (data[0] == 0x80) {
        if (data[1] == 0x00) {
            if (data[2] == 0x01) {
                printf("↑\r\n");
            } else if (data[2] == 0x02) {
                printf("↓\r\n");
                orb_drop();
                printf("orb_drop\r\n");
            } else if (data[2] == 0x04) {
                printf("→\r\n");
                key_release();
                printf("key_release\r\n");
            } else if (data[2] == 0x08) {
                printf("←\r\n");
                key_catch();
                printf("key_catch\r\n");
            } else if (data[2] == 0x10) {
                printf("△\r\n");
                front();
                printf("front\r\n");
            } else if (data[2] == 0x20) {
                printf("×\r\n");
                back();
                printf("back\r\n");
            } else if (data[2] == 0x40) {
                printf("○\r\n");
                right();
                printf("right\r\n");
            } else if (data[2] == 0x03) {
                printf("START\r\n");
            } else if (data[2] == 0x0c) {
                printf("SELECT\r\n");
            } else if (data[2] == 0x00) {
                if ((data[3] == 0x40) && (data[4] == 0x40) && (data[5] == 0x40) && (data[6] == 0x40) && (data[7] == 0x00)) {
                    printf("neutral\r\n");
                }
            }
        } else if (data[1] == 0x01) {
            printf("□\r\n");
            left();
            printf("left\r\n");
        } else if (data[1] == 0x02) {
            printf("L1\r\n");
            turn_left();
            printf("turn_left\r\n");
        } else if (data[1] == 0x04) {
            printf("L2\r\n");
        } else if (data[1] == 0x08) {
            printf("R1\r\n");
            turn_right();
            printf("turn_right");
        } else if (data[1] == 0x10) {
            printf("R2\r\n");
        }
    } else {
        printf("The data is different\r\n");
    }
}