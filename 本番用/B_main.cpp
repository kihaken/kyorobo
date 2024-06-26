// 使ってるピン番号 (p6, p10) p15, (予備としてp16, p18) p20, p26, p27, p29, p30
// 使ってるボタン UP, DOWN, RIGHT, LEFT, E1, L1, R2, TRIANGLE, CROSS, CIRCLE, SQUARE

#include "mbed.h"
#include "CANMotor.h"
#include "PS3_SBDBT5V.h"

UnbufferedSerial pc(USBTX, USBRX, 115200);
PS3 ps3(p27, p26);
CAN can(p30, p29);
DigitalIn limit15(p15);
DigitalIn limit20(p20);

enum wheel{
    FR,
    FL,
    A,
    B,
    C,
    D,
    TOTAL_WHEEL
};

CANMotorManager mng(can);
const int total_motor = TOTAL_WHEEL;
CANMotor motor[total_motor] = {
    CANMotor(can,mng,0x00,0),
    CANMotor(can,mng,0x01,0),
    CANMotor(can,mng,0x02,0),
    CANMotor(can,mng,0x03,0),
    CANMotor(can,mng,0x04,0),
    CANMotor(can,mng,0x05,0),
};

int state[total_motor] = {
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
};

void up();
void down();
void right_rotation();
void left_rotation();

void orb();
void key_up();
void key_down();
void allup();
void alldown();
void itidasi();
void itidasi2();

void Brake();

void ps3_get_data();
DigitalOut led1(LED1);

int val;
int data[PS3::MAX_BUTTON];

int analog_data[4];

float duty_cycle = 0.6;

int main(){
    ps3.attach(&ps3_get_data);
    while(true){
        duty_cycle = 0.6;

        if(ps3.check_connection()){
            if(val == 1){
                if(PS3::UP){
                    duty_cycle = 0.5;
                    up();
                }else if(PS3::DOWN){
                    duty_cycle = 0.5;
                    down();
                }else if(PS3::RIGHT){
                    right_rotation();
                }else if(PS3::LEFT){
                    left_rotation();
                }

                else if(PS3::R1){
                    itidasi();
                }else if(PS3::L1){
                    itidasi2();
                }else if(PS3::R2){
                    duty_cycle = 0.3;
                    orb();
                }
                
                else if(PS3::TRIANGLE){
                    if(limit15){
                        Brake();
                    }else{
                        allup();
                    }
                }else if(PS3::CROSS){
                    if(limit20){
                        Brake();
                    }else{
                        alldown();
                    }
                }else if(PS3::CIRCLE){
                    key_up();
                }else if(PS3::SQUARE){
                    key_down();
                }
            }else if(val==-1){
                Brake();
            }

            for(int i = 0; i < total_motor; i++){
                motor[i].duty_cycle(duty_cycle);
                motor[i].state(state[i]);
            }

            mng.write_all();
            printf("write\r\n");
            led1 = !led1;
            ThisThread::sleep_for(10ms);
        }
    }
}

void ps3_get_data(){
    val = ps3.get_data(data);
}

void up(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CCW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}

void down(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}

void right_rotation(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CCW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}

void left_rotation(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}

void orb(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::CW;
    state[D] = Motor::Brake;
}

void key_up(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::CW;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}

void key_down(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::CCW;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}

void allup(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::CW;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}

void alldown(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::CCW;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}

void itidasi(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::CW;
}

void itidasi2(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::CCW;
}

void Brake(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}