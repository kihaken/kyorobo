///////////////////////////////////////////////////////////////ロボマス用/////////////////////////////////////////////////////////////////

#include "mbed.h"
#include "rbms.h"
// #include "rote_robo_ms.h"
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
PS3 ps3(p27, p26);
rbms motor(can, 1, 4);

void up();
void down();
void right();
void left();
void right_rotation();
void left_rotation();
void ps3_get_data();

int val;
int data[8];

int state[4] = {};

int main(){
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
            }else if(val == 0){
                Brake();
            }
        }
        m3508.rbms_send(motor);
    }
}

void ps3_get_data(){
    val = ps3.get_data(data);
}

void up(){
    state[FR] = -3000;
    state[FL] =  3000;
    state[BR] = -3000;
    state[BL] =  3000;
}

void down(){
    state[FR] =  3000;
    state[FL] = -3000;
    state[BR] =  3000;
    state[BL] = -3000;
}

void right(){
    state[FR] =  3000;
    state[FL] = -3000;
    state[BR] = -3000;
    state[BL] =  3000;
}

void left(){
    state[FR] = -3000;
    state[FL] =  3000;
    state[BR] =  3000;
    state[BL] = -3000;
}

void right_rotation(){
    state[FR] =  3000;
    state[FL] =  3000;
    state[BR] =  3000;
    state[BL] =  3000;
}

void left_rotation(){
    state[FR] = -3000;
    state[FL] = -3000;
    state[BR] = -3000;
    state[BL] = -3000;
}

void Brake(){
    state[FR] = 0;
    state[FL] = 0;
    state[BR] = 0;
    state[BL] = 0;
}
