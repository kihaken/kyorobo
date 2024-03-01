/*////////注意//////////
・orbを動かすとき(R2)のボタンを触るとき60度の値を読み取って回転が止まるまで絶対にボタンは離さない
→離したら値がおかしくなって次から正常に動作しなくなります
//////////////////////*/

// エンコーダーのプログラムに割り込みフラグを追加
// orbの昇降に関するプログラムの追加

#include "mbed.h"
#include "CANMotor.h"
#include "PS3_SBDBT5V.h"

UnbufferedSerial pc(USBTX,USBRX,115200);
PS3 ps3(p27,p26);
CAN can(p30,p29);

enum wheel{
    FR,  
    FL,
    BR,
    BL,
    A,   //orb
    B,   //orb_up,down
    C,   //key_open,close
    D,   //key_up,down

    TOTAL_WHEEL,
};

CANMotorManager mng(can);
const short int total_motor=8;
CANMotor motor[total_motor]={
    CANMotor(can,mng,0x00,0),  //FR
    CANMotor(can,mng,0x01,0),  //FL
    CANMotor(can,mng,0x02,0),  //BR
    CANMotor(can,mng,0x03,0),  //BL
    CANMotor(can,mng,0x04,0),  //A  orb
    CANMotor(can,mng,0x05,0),  //B  orb_up,down
    CANMotor(can,mng,0x06,0),  //C  key_open,close
    CANMotor(can,mng,0x07,0)   //D  key_up,down
};

int state[total_motor]={
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake
};

//InterruptIn a(p10);  //エンコーダ―
//InterruptIn b(p6);   //エンコーダ―

void up();
void down();
void right();
void left();
void right_rotation();
void left_rotation();

void orb();
void orb_up();
void orb_down();
void key_open();
void key_close();
void key_up();
void key_down();


void Brake();

//void a_slit();  //エンコーダ―
//void b_slit();  //エンコーダ―


void ps3_get_data();
DigitalOut led1(LED1);

// int k;  //エンコーダーのときにつかう変数
int val;
int data[PS3::MAX_BUTTON];

// int analog_data[4];

float duty_cycle=0.5;

//volatile int passed_slit = 0;
//float angle = 0;
//volatile bool interrupt_flag = false;

// int i;  //angleの値を入れるための変数

int main(){
    ps3.attach(&ps3_get_data);

    while(true){
        duty_cycle=0.5;

/*
        a.rise(a_slit);
        a.fall(a_slit);
        b.rise(b_slit);
        b.fall(b_slit);

        // if (interrupt_flag){
        //     angle = 0.45f * passed_slit;
        //     printf("angle : %d.%d\r\n",(int)angle,(int)((angle - (int)angle) * 100.0f));
        //     interrupt_flag = false;
        //     ThisThread::sleep_for(10ms);
        // } 
*/
        if(ps3.check_connection()){
            if(val==1){
                if(data[PS3::UP]){
                    up();
                }else if(data[PS3::DOWN]){
                    down();
                }else if(data[PS3::RIGHT]){
                    right();
                }else if(data[PS3::LEFT]){
                    left();
                }else if(data[PS3::R1]){
                    right_rotation();
                }else if(data[PS3::L1]){
                    left_rotation();
                }else if(data[PS3::TRIANGLE]){
                    key_up();
                }else if(data[PS3::TRIANGLE] && data[PS3::L2]){
                    orb_up();
                }else if(data[PS3::CIRCLE]){
                    key_open();
                }else if(data[PS3::CROSS]){
                    key_down();
                }else if(data[PS3::CROSS] && data[PS3::L2]){
                    orb_down();
                }else if(data[PS3::SQUARE]){
                    key_close();
                }else if(data[PS3::R2]){
                    duty_cycle=0.3;
                    orb();
                    /*if(k==0){
                        int i=angle;
                        i+=60;
                        k++;
                    }
                    if(angle>i){
                        Brake();
                        k=0;
                    }else{
                        orb();
                    }*/ 
                }
            }else if(val==-1){
                Brake();
            }
        }

        for(int i=0;i<total_motor;i++){
            motor[i].duty_cycle(duty_cycle);
            motor[i].state(state[i]);
        }
        mng.write_all();
        led1=!led1;
        ThisThread::sleep_for(50ms);
    }
}

void ps3_get_data(){
    val = ps3.get_data(data);
}

void up(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CCW;
    state[BR] = Motor::CW;
    state[BL] = Motor::CCW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
void down(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CW;
    state[BR] = Motor::CCW;
    state[BL] = Motor::CW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
void right(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CCW;
    state[BR] = Motor::CW;
    state[BL] = Motor::CW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
void left(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CW;
    state[BR] = Motor::CCW;
    state[BL] = Motor::CCW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
void right_rotation(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CCW;
    state[BR] = Motor::CCW;
    state[BL] = Motor::CCW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
void left_rotation(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CW;
    state[BR] = Motor::CW;
    state[BL] = Motor::CW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;   
    state[D] = Motor::Brake; 	
}
void orb(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[BR] = Motor::Brake;
    state[BL] = Motor::Brake;
    state[A] = Motor::CW;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
void orb_up(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[BR] = Motor::Brake;
    state[BL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::CW;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
void orb_down(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[BR] = Motor::Brake;
    state[BL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::CCW;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
void key_open(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[BR] = Motor::Brake;
    state[BL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::CW;
    state[D] = Motor::Brake;
}
void key_close(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[BR] = Motor::Brake;
    state[BL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::CCW;
    state[D] = Motor::Brake;
}
void key_up(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[BR] = Motor::Brake;
    state[BL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::CW;
}
void key_down(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[BR] = Motor::Brake;
    state[BL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::CCW;
}
void Brake(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[BR] = Motor::Brake;
    state[BL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
}
// void a_slit(){
//     interrupt_flag = true;
//     if (a != b){
//         passed_slit++;
//     } else {
//         passed_slit--;
//     }
// }
// void b_slit(){
//     interrupt_flag = true;
//     if (a == b){
//         passed_slit++;
//     } else {
//         passed_slit--;
//     }
// }