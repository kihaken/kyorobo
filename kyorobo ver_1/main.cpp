#include "mbed.h"
#include "rbms.h"

#define  TIRE 150
#define CRAWLER 200
#define UP_DOWN 150
#define KEY 150
#define FALL_DOWN 150
#define BRAKE 0
#define DATA_SIZE 8


UnbufferedSerial pc(USBTX, USBRX, 115200);
UnbufferedSerial ps3(p9, p10, 2400);    
CAN can(p30, p29);
rbms tire(can, 0, 2);
rbms crawler(can, 0, 2);
rbms up_down(can, 1, 2); // とりあえず書いといた
rbms key(can, 0, 1); // 機構単体の昇降が増えるかも
rbms drop_down(can, 0, 1);

int _tire[2] = {0}; // 左, 右
int _crawler[2] = {0};
int _up_down[2] = {0};
int _key[1] = {0};
int _drop_down[1] = {0};
int data[DATA_SIZE];
int i;

void reference();
void front();
void back();
void left();
void right();
void up();
void down();
void grab();
void release();
void fall_down();
void brake();


int main() {
    while (true) {
        // if (ps3.readable()) { // ps3の信号解析
        //     for (i = 0; i < DATA_SIZE; i++) {
        //         data[i] = ps3.read(&data[i], 1);
        //     }
        //     reference();
        // }else{
        //     brake();
        // }

        if(pc.readable()){ // pcの信号解析
            char getc;
            pc.read(&getc, 2);
            switch(getc){
                case 'w': printf("wwwwww"); front(); break;
                case 'a': left(); break;
                case 's': back(); break;
                case 'd': right(); break;
                case 'q': up(); break;
                case 'e': down(); break;
            }
        }else{
            brake();
        }

        // ロボマスの制御
        if (tire.rbms_send(_tire)) printf("send_tire\r\n");
        if (crawler.rbms_send(_crawler)) printf("send_crawler\r\n");
        if (up_down.rbms_send(_up_down)) printf("send_up_down");

        ThisThread::sleep_for(25ms);
    }
}


void front(){
    _crawler[0] =  CRAWLER;
    _crawler[1] = -CRAWLER;
    printf("front\r\n");
}

void back(){
    _crawler[0] = -CRAWLER;
    _crawler[1] =  CRAWLER;
    printf("back\r\n");
}

void left(){
    _tire[0] =  TIRE;
    _tire[1] = -TIRE;
    printf("left\r\n");
}

void right(){
    _tire[0] = -TIRE;
    _tire[1] =  TIRE;
    printf("right\r\n");
}

void up(){
    _up_down[0] =  UP_DOWN;
    _up_down[1] = -UP_DOWN;
    printf("up\r\n");
}

void down(){
    _up_down[0] = -UP_DOWN;
    _up_down[1] =  UP_DOWN;
    printf("down\r\n");
}

void grab(){
	_key[0] = -KEY;
    printf("grab\r\n");
}

void release(){
	_key[0] = KEY;
    printf("release\r\n");
}

void fall_down(){
	_drop_down[0] = FALL_DOWN;
    printf("falldown\r\n");
}

void  brake(){
	_tire[0] = 0; // 左, 右
	_tire[1] = 0;
	_crawler[0] = 0;
	_crawler[1] = 0;
	_up_down[0] = 0;
	_up_down[1] = 0;
	_key[0] = 0;
	_drop_down[0] = 0;
    printf("BRAKE\r\n");
}

void reference() {
    if (data[0] == 0x80) {
        if (data[1] == 0x00) {
            if (data[2] == 0x01) {
                printf("↑\r\n");
                front();
            } else if (data[2] == 0x02) {
                printf("↓\r\n");
                back();
            } else if (data[2] == 0x04) {
                printf("→\r\n");
                right();
            } else if (data[2] == 0x08) {
                printf("←\r\n");
                left();
            } else if (data[2] == 0x10) {
                printf("△\r\n");
            } else if (data[2] == 0x20) {
                printf("×\r\n");
            } else if (data[2] == 0x40) {
                printf("○\r\n");
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
        } else if (data[1] == 0x02) {
            printf("L1\r\n");
            up();
        } else if (data[1] == 0x04) {
            printf("L2\r\n");
        } else if (data[1] == 0x08) {
            printf("R1\r\n");
            down();
        } else if (data[1] == 0x10) {
            printf("R2\r\n");
        }
    } else {
        printf("データが異なります\r\n");
    }
}
