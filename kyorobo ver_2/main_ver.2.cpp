#include "mbed.h"
#include "CANMotor.h"

UnbufferedSerial pc(USBTX, USBRX, 115200);
UnbufferedSerial ps3(p9, p10, 2400);  
CAN can(p30, p29);
CANMotorManager mng(can);
CANMotor crawler[4] = {CANMotor(can, mng, 0x0C, 0),
                       CANMotor(can, mng, 0x0D, 1),
                       CANMotor(can, mng, 0x0E, 2),
                       CANMotor(can, mng, 0x0F, 3) };
float duty_cycle = 0.0f;
int state[4] = {3};
int data[8] = {0};
int i;

void reference();
void front();
void back();
void left();
void right();
void turn_left();
void turn_right();
void brake();

int main(){
    duty_cycle = 0.5;

    while (mng.connect_all()){
        ThisThread::sleep_for(2000ms);
    }
    while (true) {
        if (ps3.readable()){ // ps3の信号解析
            for (i = 0; i < 8; i++) {
                data[i] = ps3.read(&data[i], 1);
            }
            reference();
        }

        if (pc.readable()){
            char getc;
            pc.read(&getc, 2);
            switch(getc){
                case 'w': front(); printf("front\r\n"); break;
                case 'a': left(); printf("left\r\n"); break;
                case 's': back(); printf("back\r\n"); break;
                case 'd': right(); printf("right\r\n"); break;
                case 'q': turn_left(); printf("turn_left\r\n"); break;
                case 'e': turn_right(); printf("turn_right\r\n"); break;
                default : brake(); printf("BRAKE"); break;
            }
        }
    }
}

void front(){
    crawler[0].state(Motor::CW);
    crawler[1].state(Motor::CCW);
    crawler[2].state(Motor::CCW);
    crawler[3].state(Motor::CW);
}

void back(){
    crawler[0].state(Motor::CCW);
    crawler[1].state(Motor::CW);
    crawler[2].state(Motor::CW);
    crawler[3].state(Motor::CCW);
}

void left(){
    crawler[0].state(Motor::CCW);
    crawler[1].state(Motor::CW);
    crawler[2].state(Motor::CCW);
    crawler[3].state(Motor::CW);
}

void right(){
    crawler[0].state(Motor::CW);
    crawler[1].state(Motor::CCW);
    crawler[2].state(Motor::CW);
    crawler[3].state(Motor::CCW);
}

void turn_left(){
    crawler[0].state(Motor::CCW);
    crawler[1].state(Motor::CCW);
    crawler[2].state(Motor::CCW);
    crawler[3].state(Motor::CCW);    
}

void turn_right(){
    crawler[0].state(Motor::CW);
    crawler[1].state(Motor::CW);
    crawler[2].state(Motor::CW);
    crawler[3].state(Motor::CW); 
}

void brake(){
    crawler[0].state(Motor::Brake);
    crawler[1].state(Motor::Brake);
    crawler[2].state(Motor::Brake);
    crawler[3].state(Motor::Brake);
}

void reference() {
    if (data[0] == 0x80) {
        if (data[1] == 0x00) {
            if (data[2] == 0x01) {
                printf("↑\r\n");
                front(); printf("front\r\n");
            } else if (data[2] == 0x02) {
                printf("↓\r\n");
                printf("back\r\n");
            } else if (data[2] == 0x04) {
                printf("→\r\n");
                printf("right\r\n");
            } else if (data[2] == 0x08) {
                printf("←\r\n");
                left(); printf("left\r\n");
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
            turn_left(); printf("turn_left\r\n");
        } else if (data[1] == 0x04) {
            printf("L2\r\n");
        } else if (data[1] == 0x08) {
            printf("R1\r\n");
            turn_right(); printf("turn_right\r\n");
        } else if (data[1] == 0x10) {
            printf("R2\r\n");
        }
    } else {
        printf("データが異なります\r\n");
    }
}
