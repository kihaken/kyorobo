// mbedOS 6.15
// クローラー制御のみしか記述していない
// PS3_SBDBT5V.hの使い方をまだ習得していないのでpcからの制御のみ

#include "mbed.h"
#include "CANMotor.h"

UnbufferedSerial pc(USBTX, USBRX, 115200);
CAN can(p30, p31);
CANmotorManager _mng(can);
CAMNotor crawler[4] = { CANMotor(can, _mng, 0x0C, 0),
                        CANMotor(can, _mng, 0x0D, 1),
                        CANMotor(can, _mng, 0x0E, 2),
                        CANMotor(can, _mng, 0x0F, 3), }

int i;
char getc;
int state[4] = {0};

void front();
void back();
void left();
void right();
void brake();

int main(){
    while(true){
        if (pc.readable()){
            getc = 0;
            pc.read(&getc, 2);
            switch(getc){
                case 'w': front(); printf("Front\r\n"); break;
                case 'a': left(); printf("Left\r\n"); break;
                case 's': back(); printf("Back\r\n"); break;
                case 'd': right(); printf("Right\r\n"); break;
                case '0': brake(); printf("Brake\r\n"); break;
            }
            crawler.write_all();
        }
    }
}

void front(){
    state[0] = Motor::CW;
    state[1] = Motor::CCW;
    state[2] = Motor::CCW;
    state[3] = Motor::CW;
}

void back(){
    state[0] = Motor::CCW;
    state[1] = Motor::CW;
    state[2] = Motor::CW;
    state[3] = Motor::CCW;
}

void left(){
    state[0] = Motor::CCW;
    state[1] = Motor::CW;
    state[2] = Motor::CCW;
    state[3] = Motor::CW;
}

void right(){
    state[0] = Motor::CW;
    state[1] = Motor::CCW;
    state[2] = Motor::CW;
    state[3] = Motor::CCW;
}

void brake(){
    state[0] = Motor::Brake;
    state[1] = Motor::Brake;
    state[2] = Motor::Brake;
    state[3] = Motor::Brake;
}
