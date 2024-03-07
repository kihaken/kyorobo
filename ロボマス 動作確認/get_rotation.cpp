// m2006は小さいほう(0) m3508は大きいほう(1)
// 一つのCANで同種類のモーター8個まで制御可能
// 速度制御の時は同時に４つまでにしておくこと
// 角度取得はThreadを立ててcan.read(msg)

#include "mbed.h"
#include "robo_ms.h"
#include "rote_robo_ms.h"
#include "rbms.h"

#define BUFFER_MAX 4
#define GIAR_RATIO 36 // m2006のギア比
#define MOTOR1_MAX 120 // 120周 
#define MOTOR2_MAX 1800 // 1800周

CAN can(p30, p29);
CANMessage msg, buffer_motor1[BUFFER_MAX], buffer_motor2[BUFFER_MAX];
rbms test(can, 0, 2);
UnbufferedSerial pc(USBTX, USBRX, 115200);
rote_robo_ms rote_robo_ms_motor1(can, test, 0, 16);
rote_robo_ms rote_robo_ms_motor2(can, test, 1, 16);
Thread thread1, thread2, thread_can;

void set_all_static_reset();
void motor1_func();
void motor2_func();
void can_receive();

int motor[2] = {0};
long motor1_angle = 0; 
long motor2_angle = 0;
long motor1_rotation = 0;
long motor2_rotation = 0;

int main(){
    motor[0] = 1000;
    motor[1] = 2000;
    thread1.start(motor1_func);
    thread2.start(motor2_func);
    thread_can.start(can_receive);
	set_all_static_reset();
    while(true){
        ThisThread::sleep_for(10ms);
    }
}

void set_all_static_reset(){
	rote_robo_ms_motor1.set_static_reset(0);
	rote_robo_ms_motor2.set_static_reset(0);
}

void motor1_func(){
    while (true){
        rote_robo_ms_motor1.rote_robo_ms_update(buffer_motor1, BUFFER_MAX); // motor1の角度の更新
        test.rbms_send(&motor[0]); // 制御信号の送信

        motor1_angle = rote_robo_ms_motor1.get_rote_sum();
        motor1_rotation = rote_robo_ms_motor1.get_rote_sum() / GIAR_RATIO; 
        printf("angle:%ld, motor_rote:%ld\r\n",motor1_angle ,motor1_rotation);
        if (motor1_rotation == MOTOR1_MAX){
            motor[0] = 0;
            printf("Brake!!! Motor1 has rotated:%d°\r\n", MOTOR1_MAX);
        }
        ThisThread::sleep_for(2ms);
    }  
}

void motor2_func(){
    while (true){
        rote_robo_ms_motor2.rote_robo_ms_update(buffer_motor2, BUFFER_MAX); // motor2の角度の更新
        test.rbms_send(&motor[1]); // 制御信号の送信

        motor2_angle = rote_robo_ms_motor2.get_rote_sum();
        motor2_rotation = rote_robo_ms_motor2.get_rote_sum() / GIAR_RATIO; 
        printf("angle:%ld, motor_rote:%ld\r\n",motor2_angle ,motor2_rotation);
        if (motor2_rotation == MOTOR2_MAX){
            motor[1] = 0;
            printf("Brake!!! Motor2 has rotated:%d°\r\n", MOTOR2_MAX);
        }
        ThisThread::sleep_for(2ms);
    }  
}

void can_receive(){
    while(true){
        if(can.read(msg)){
            if(msg.id == 0x201){
                for(int i = 1; i < BUFFER_MAX; i++){
                    buffer_motor1[i - 1] = buffer_motor1[i];
                }
                buffer_motor1[BUFFER_MAX - 1] = msg;
            }
            if(msg.id == 0x202){
                for(int i = 1; i < BUFFER_MAX; i++){
                    buffer_motor2[i - 1] = buffer_motor2[i];
                }
                buffer_motor2[BUFFER_MAX - 1] = msg;
            }
        }
    }
}


