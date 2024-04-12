/////////////////////////////////////////////////////////////////CANMotor用/////////////////////////////////////////////////////////////////
#include "mbed.h"
#include "CANMotor.h"
#include "PS3_SBDBT5V.h"

enum wheel{
    FR,
    FL,
    BR,
    BL,
    TOTAL_WHEEL
};

UnbufferedSerial pc(USBTX, USBRX, 115200);
CAN can(p30, p29);
CANMotorManager mng(can);
PS3 ps3(p27, p26);
CANMotor motor[TOTAL_WHEEL] = {
    CANMotor(can,mng,0x00,0),
    CANMotor(can,mng,0x01,0),
    CANMotor(can,mng,0x02,0),
    CANMotor(can,mng,0x03,0),
};

void up();
void down();
void right();
void left();
void right_rotation();
void left_rotation();
void ps3_get_data();

int val;
float duty_cycle = 0;
int data[8];
int state[4] = {};

int main(){
    duty_cycle = 0.5;
    ps3.attach(ps3_get_data);
    while (true) {
        if (ps3.check_connection()){
            if (val == 1){
                if (PS3::UP){
                    up();
                }else if (PS3::DOWN){
                    down();
                }else if (PS3::RIGHT){
                    right();
                }else if (PS3::LEFT){
                    left();
                }else if (PS3::R1){
                    right_rotation();
                }else if (PS3::L1){
                    left_rotation();
                }
            }
        }
        mng.write_all(duty_cycle);
    }
}

void ps3_get_data(){
    val = ps3.get_data(data);
}

void up(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CW;
    state[BR] = Motor::CCW;
    state[BL] = Motor::CW;
}

void down(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CCW;
    state[BR] = Motor::CW;
    state[BL] = Motor::CCW;
}

void right(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CCW;
    state[BR] = Motor::CCW;
    state[BL] = Motor::CW;
}

void left(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CW;
    state[BR] = Motor::CW;
    state[BL] = Motor::CCW;
}

void right_rotation(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CW;
    state[BR] = Motor::CW;
    state[BL] = Motor::CW;
}

void left_rotation(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CCW;
    state[BR] = Motor::CCW;
    state[BL] = Motor::CCW;
}