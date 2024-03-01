/*///////////////////////////////////////注意///////////////////////////////////////////
・orbを動かすとき(R2)のボタンを触るとき60度の値を読み取って回転が止まるまで絶対にボタンは離さない
→離したら値がおかしくなって次から正常に動作しなくなります
・オーブが落ちそうになったらボタンから手を放す！！！
→たまにオーブが詰まるから
//////////////////////////////////////////////////////////////////////////////////////*/

// mbed-OS-6.15
// リミットスイッチなしに変更
// エンコーダーなしに変更(割り込みフラグの追加)
// itidasi(), itidasi2()をコメントアウト
// NEUTRAL時にBrake()を実行するようにした
// solenoidもコメントアウト
// itidasi,itidasi2関数の定義
//コントローラ配置の修正
// オーブのdudy比を0.3に変更

#include "mbed.h"
#include "CANMotor.h"
#include "PS3_SBDBT5V.h"
// #include "CANSolenoid.h"  

UnbufferedSerial pc(USBTX,USBRX,115200);
PS3 ps3(p27,p26);
CAN can(p30,p29);
// CANSolenoid solenoid(can, 0);

enum wheel{
    FR,   // キャタピラ左
    FL,   // キャタピラ右
    A,    // key_up, down
    B,    // all_up, down
    C,    // orb
    D,    // itidasi
    TOTAL_WHEEL,
};

CANMotorManager mng(can);
const short int total_motor=6;
CANMotor motor[total_motor]={
    CANMotor(can,mng,0x00,0),  //FR　キャタピラ左
    CANMotor(can,mng,0x01,0),  //FL　キャタピラ右
    CANMotor(can,mng,0x02,0),  //BR　key_up, down
    CANMotor(can,mng,0x03,0),  //BL　all_up, down
    CANMotor(can,mng,0x04,0),  //A   orb
    CANMotor(can,mng,0x05,0),  //B   itidasi
};

int state[total_motor]={
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
    Motor::Brake,
};

// InterruptIn a(p10);  //エンコーダ―
// InterruptIn b(p6);   //エンコーダ―

void up();
void down();
void right_rotation();
void left_rotation();

void orb();//モーターを60°回すためのプログラム
void key_up();
void key_down();
void allup();//機構上げる
void alldown();//機構下げる
void itidasi();//位置だし右ボタン
void itidasi2();//位置だし左ボタン

void Brake();//ブレーキ

// void a_slit();  //エンコーダ―
// void b_slit();  //エンコーダ―


void ps3_get_data();
DigitalOut led1(LED1);

// int k;//エンコーダーのときにつかう変数
// volatile bool interrupt_flag = false;
int val;
int data[PS3::MAX_BUTTON];

int analog_data[4];

float duty_cycle=0.5;

//volatile  int passed_slit = 0;
// float angle = 0;

// int i;

int main(){
    ps3.attach(&ps3_get_data);

    while(true){
        duty_cycle=0.5;
        

        // a.rise(a_slit);
        // a.fall(a_slit);
        // b.rise(b_slit);
        // b.fall(b_slit);
        // if (interrupt_flag){
        //     angle = 0.45f * passed_slit;
        //     printf("angle : %d.%d\r\n",(int)angle,(int)((angle - (int)angle) * 100.0f));
        //     interrupt_flag = false;
        //     ThisThread::sleep_for(10ms);
        // }                                                                                

        if(ps3.check_connection()){
            if(val==1){
                if(data[PS3::UP]){
                    up();
                }else if(data[PS3::DOWN]){
                    down();
                }else if(data[PS3::RIGHT]){
                    right_rotation();
                }else if(data[PS3::LEFT]){
                    left_rotation();
                }else if(data[PS3::R1]){
                    itidasi();
                }else if(data[PS3::L1]){
                    itidasi2();
                }else if(data[PS3::TRIANGLE]){
                    duty_cycle = 0.99f;
                    allup();
                }else if(data[PS3::CROSS]){
                    duty_cycle = 0.99f;
                    alldown();
                }else if(data[PS3::CIRCLE]){
                    key_up();
                }else if(data[PS3::SQUARE]){
                    key_down();
                }else if(data[PS3::R2]){
                    duty_cycle = 0.3;
                    orb();
                //     if(k==0){
                //         int i=angle;
                //         i+=60;
                //         k++;
                //     }
                //     if(angle>i){
                //         Brake();
                //         k=0;
                //     }else{
                //         orb();//エンコーダーで計測する
                //     }  
                }
            }else{
                Brake(); // PS3::NEUTRALの時は常にブレーキ
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
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}
void down(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}

void right_rotation(){
    state[FR] = Motor::CCW;
    state[FL] = Motor::CCW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}
void left_rotation(){
    state[FR] = Motor::CW;
    state[FL] = Motor::CW;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}

void key_up(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::CW;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}
void key_down(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::CCW;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}
void allup(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::CW;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}
void alldown(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::CCW;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);//内容変更する
}
void orb(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::CW;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}

void itidasi(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::CW;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}

void itidasi2(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::CCW;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}

void Brake(){
    state[FR] = Motor::Brake;
    state[FL] = Motor::Brake;
    state[A] = Motor::Brake;
    state[B] = Motor::Brake;
    state[C] = Motor::Brake;
    state[D] = Motor::Brake;
//    solenoid.write(CANSolenoid::B1, CANSolenoid::ON);
}

// void a_slit(){
//     interrupt_flag = true;
//     if (a != b){
//         passed_slit++;
//     } else {
//         passed_slit--;
//     }
// }
//
// void b_slit(){
//     interrupt_flag = true;
//     if (a == b){
//         passed_slit++;
//     } else {
//         passed_slit--;
//     }
// }
