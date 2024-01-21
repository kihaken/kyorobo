// CANMotor.h の branches は main を選ぶこと
// でないとインクルードエラーが吐かれる

#include "mbed.h"
#include "CANMotor.h"
#include "rbms.h"

UnbufferedSerial pc(USBTX, USBRX, 115200);
UnbufferedSerial ps3(p9, p10, 2400);
CAN can(p30, p29);
CANMotorManager _mng(can);
CANMotor crawler[4] = { CANMotor(can, _mng, 0x0C, 0),
                        CANMotor(can, _mng, 0x0D, 1),
                        CANMotor(can, _mng, 0x0E, 2),
                        CANMotor(can, _mng, 0x0F, 3), };
rbms orb(can, 0, 1);
rbms key(can, 0, 1);

int i;
int data[8] = {0};
float duty_cycle = 0.0f;
int state[4] = {0};
int _orb[1] = {0};
int _key[1] = {0};

void reference();
void front();
void back();
void left();
void right();
void front_left();
void front_right();
void back_left();
void back_right();
void turn_left();
void turn_right();
void brake();
void down();
void grab();
void release();

int main(){
    duty_cycle = 0.5;

    while (true){
        if (ps3.readable()) {
            for (i = 0; i < 8; i++) {
                data[i] = ps3.read(&data[i], 1);
            }
            reference();
        }

        if (pc.readable()){
            char getc  = 0;
            pc.read(&getc, 2);
            switch(getc){
                case 'w': front(); printf("front\r\n"); break;
                case 'a': left(); printf("left\r\n"); break;
                case 'x': back(); printf("back\r\n"); break;
                case 'd': right(); printf("right\r\n"); break;
                case 'q': front_left(); printf("front_left\r\n"); break;
                case 'e': front_right(); printf("front_right\r\n"); break;
                case 'z': back_left(); printf("back_left\r\n"); break;
                case 'c': back_right(); printf("back_right\r\n"); break;
                case 'r': turn_left(); printf("turn_left\r\n"); break;
                case 't': turn_right(); printf("turn_right\r\n"); break;
                case ' ': brake(); printf("brake\r\n"); break;
                case 'v': down(); printf("orb fall_down\r\n"); break;
                case 'f': grab(); printf("key catch\r\n"); break;
                case 'g': release(); printf("key release\r\n"); break;
            }
            _mng.write_all();
        }
    }
    ThisThread::sleep_for(25ms);
}

void front(){
    state[0] = Motor::CW;
    state[1] = Motor::CCW;
    state[2] = Motor::CCW;
    state[3] = Motor::CW;
    _orb[0] = 0;
    _key[0] = 0;
}

void back(){
    state[0] = Motor::CCW;
    state[1] = Motor::CW;
    state[2] = Motor::CW;
    state[3] = Motor::CCW;
    _orb[0] = 0;
    _key[0] = 0;
}

void left(){
    state[0] = Motor::CCW;
    state[1] = Motor::CW;
    state[2] = Motor::CCW;
    state[3] = Motor::CW;
    _orb[0] = 0;
    _key[0] = 0;
}

void right(){
    state[0] = Motor::CW;
    state[1] = Motor::CCW;
    state[2] = Motor::CW;
    state[3] = Motor::CCW;
    _orb[0] = 0;
    _key[0] = 0;
}

void front_left(){
    state[0] = Motor::Brake;
    state[1] = Motor::CCW;
    state[2] = Motor::Brake;
    state[3] = Motor::CW;  
    _orb[0] = 0;
    _key[0] = 0;
}

void front_right(){
    state[0] = Motor::CW;
    state[1] = Motor::Brake;
    state[2] = Motor::CCW;
    state[3] = Motor::Brake; 
    _orb[0] = 0;
    _key[0] = 0;
}

void back_left(){
    state[0] = Motor::CCW;
    state[1] = Motor::Brake;
    state[2] = Motor::CW;
    state[3] = Motor::Brake;  
    _orb[0] = 0;
    _key[0] = 0;
}

void back_right(){
    state[0] = Motor::Brake;
    state[1] = Motor::CW;
    state[2] = Motor::Brake;
    state[3] = Motor::CCW;  
    _orb[0] = 0;
    _key[0] = 0;
}

void turn_left(){
    state[0] = Motor::CCW;
    state[1] = Motor::CCW;
    state[2] = Motor::CCW;
    state[3] = Motor::CCW; 
    _orb[0] = 0;
    _key[0] = 0;
}

void turn_right(){
    state[0] = Motor::CW;
    state[1] = Motor::CW;
    state[2] = Motor::CW;
    state[3] = Motor::CW; 
    _orb[0] = 0;
    _key[0] = 0;
}

void brake(){
    state[0] = Motor::Brake;
    state[1] = Motor::Brake;
    state[2] = Motor::Brake;
    state[3] = Motor::Brake;
    _orb[0] = 0;
    _key[0] = 0;
}

void down(){
    _orb[0] = 250;
}

void grab(){
    _key[0] = 300;
}

void release(){
    _key[0] = -300;
}

void reference() {
    if (data[0] == 0x80) {
        if (data[1] == 0x00) {
            if (data[2] == 0x01) {
                printf("↑\r\n");
            } else if (data[2] == 0x02) {
                printf("↓\r\n");
            } else if (data[2] == 0x04) {
                printf("→\r\n");
            } else if (data[2] == 0x08) {
                printf("←\r\n");
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
        } else if (data[1] == 0x04) {
            printf("L2\r\n");
        } else if (data[1] == 0x08) {
            printf("R1\r\n");
        } else if (data[1] == 0x10) {
            printf("R2\r\n");
        }
    } else {
        printf("The data is different\r\n");
    }
}
