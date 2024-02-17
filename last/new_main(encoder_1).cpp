// ここではkeyのリミットスイッチは両端につける予定 (エンコーダー1個使用, リミットスイッチ4個使用)
// これはトルク制御 (たぶん速度制御できる余裕ない)

// SBDBT5V pin配置 (自分でもよくわかってない)
// 上から
// GND（とりあえずGNDは確定、？)
// VCC
// p10(TX)
// p5(Digital)

// Rotary Encoder pin配置
// 上から
// p6(B相)
// VCC
// p27(A相)

// GND

#include "mbed.h"
#include "rbms.h"
#include "PS3conOS6.h"

#define CRAWLER 3000 // torque
#define ORB 2000
#define KEY_X 800
#define KEY_Y 1000
#define DATA_SIZE 8 // PS3
#define ORB_ROTE 60 // エンコーダーに指定する回転角

UnbufferedSerial pc(USBTX, USBRX, 115200);
PS3 ps3(p10, p5);
// ロボマス関係
CAN can(p30, p29); 
rbms crawler(can, 0, 4);
rbms orb(can, 0, 1);
rbms key(can, 0, 2);
InterruptIn a(p27);
InterruptIn b(p6);
// 基準点, 目標点
DigitalIn limit_x[2] = { DigitalIn(p18), DigitalIn(p20) }, 
          limit_y[2] = { DigitalIn(p21), DigitalIn(p23) };

// void reference_pc();
void reference_ps3();
void front(); // 足回り
void back();
void left();
void right();
void turn_left();
void turn_right();
void all_brake();
void orb_drop(); // 回収機構
void key_catch();
void key_release();
void key_up();
void key_down();
// センサの値を更新
void encoder_update(); // エンコーダー
void angle_reset();
void a_slit();
void b_slit();
void limit_update();
void ps3_get_data();

int i = 0;
int _orb[1] = {0}; 
int _key[2] = {0}; // ツメ, 昇降
int _crawler[4] = {0}; // 左前, 右前, 右後, 左後
int data[8] = {0}; // PS3
// int analog_data[8]; // joystic
int passed_slit = 0; // エンコーダー
float angle = 0; 
int val = -1; // ボタンが押されてない時
volatile bool interrupt_flag = false; // 割り込みフラグ

int main(){
    angle_reset();
    ps3.attach(ps3_get_data);    
    a.rise(a_slit); // 割り込みを設定
    a.fall(a_slit);
    b.rise(b_slit);
    b.fall(b_slit);
    while(1){
        limit_update(); // リミットスイッチの状態をチェック        
        if(interrupt_flag) { // フラグが立ったら
            interrupt_flag = false;
            encoder_update();
        }
        // reference_pc();
        if(ps3.check_connection()){
            printf("connecting...");
     		if(val == 1) {		// ボタンが押されているとき
        		reference_ps3();	// 処理
        	// ps3.get_analog(analog_data); // joystic
        	} else if(val == -1) {	// 全てのボタンが離されたとき
        		all_brake();
                printf("all_brake\r\n"); // 処理
        	} else {	// ボタン操作が行われていなかったとき
        			// 処理
            }
     	} else {
     		printf("disconnected\r\n");
     	}
         // コントローラによる操作は割り込みで行われるため常時行う処理を書く(CANMotorの書き込みなど)
        reference_ps3();
        // 指定した角度分回っていたら止める(brakeで上書き)
        // printf("angle : %d.%d\r\n",(int)angle, (int)((angle - (int)angle) * 100.0f));
        if (angle == ORB_ROTE) _orb[0] = 0;
        crawler.rbms_send(_crawler); // 制御信号の送信
        orb.rbms_send(_orb);
        key.rbms_send(_key);
        ThisThread::sleep_for(10ms);
    }
}

void front(){
    _crawler[0] =  CRAWLER;
    _crawler[1] = -CRAWLER;
    _crawler[2] = -CRAWLER;
    _crawler[3] =  CRAWLER;
}

void back(){
    _crawler[0] = -CRAWLER;
    _crawler[1] =  CRAWLER;
    _crawler[2] =  CRAWLER;
    _crawler[3] = -CRAWLER;
}

void left(){
    _crawler[0] = -CRAWLER;
    _crawler[1] =  CRAWLER;
    _crawler[2] = -CRAWLER;
    _crawler[3] =  CRAWLER;
}

void right(){
    _crawler[0] =  CRAWLER;
    _crawler[1] = -CRAWLER;
    _crawler[2] =  CRAWLER;
    _crawler[3] = -CRAWLER;
}

void turn_left(){
    _crawler[0] = -CRAWLER;
    _crawler[1] = -CRAWLER;
    _crawler[2] = -CRAWLER;
    _crawler[3] = -CRAWLER; 
}

void turn_right(){
    _crawler[0] = CRAWLER;
    _crawler[1] = CRAWLER;
    _crawler[2] = CRAWLER;
    _crawler[3] = CRAWLER; 
}

void all_brake(){
    _crawler[0] = 0;
    _crawler[1] = 0;
    _crawler[2] = 0;
    _crawler[3] = 0;
    _orb[0] = 0;
    _key[0] = 0;
    _key[1] = 0;
}

void orb_drop(){
    _orb[0] = ORB;
}

void key_catch(){
    _key[0] = KEY_X;
}

void key_release(){
    _key[0] = -KEY_X;
}

void key_up(){
    _key[1] = KEY_Y;
}

void key_down(){
    _key[1] = -KEY_Y;
}

void a_slit(){
    interrupt_flag = true;
    if (a.read() != b.read()){
        passed_slit++; // a相が立ち上がった時、b相の信号と異ったら正転
    } else {
        passed_slit--;
    }
}

void b_slit(){
    interrupt_flag = true;
    if (a.read() == b.read()){
        passed_slit++; // b相が立ち上がった時、a相の信号と同じだったら正転
    } else {
        passed_slit--;
    }
}

void encoder_update(){
    a.rise(a_slit);
    a.fall(a_slit);
    b.rise(b_slit);
    b.fall(b_slit);
    angle = 0.45f * passed_slit; // 1割り込みごとに進む角度ｘ割り込みが行われた回数
    ThisThread::sleep_for(10ms);
}

void angle_reset(){
    angle = 0;
}

void limit_update(){
    if (limit_x[0].read()) {_key[0] = 0; angle_reset(); printf("Limit_x[0]!!!!");}
    if (limit_x[1].read()) {_key[0] = 0; angle_reset(); printf("Limit_x[1]!!!!");}
    if (limit_y[0].read()) {_key[1] = 0; angle_reset(); printf("Limit_y[0]!!!!");}
    if (limit_y[1].read()) {_key[1] = 0; angle_reset(); printf("Limit_y[1]!!!!");}
    
}

// void reference_pc(){
//     if (pc.readable()) {
//         char getc;
//         pc.read(&getc, 2);
//         switch(getc){
//             case 'w': front(); printf("front\r\n"); break;
//             case 'a': left(); printf("left\r\n"); break;
//             case 's': back(); printf("back\r\n"); break;
//             case 'd': right(); printf("right\r\n"); break;
//             case 'q': turn_left(); printf("turn_left\r\n"); break;
//             case 'e': turn_right(); printf("turn_right\r\n"); break;
//             case 'x': orb_drop(); printf("orb_drop\r\n"); break;
//             case 'h': key_catch(); printf("key_catch\r\n"); break;
//             case 'f': key_release(); printf("key_release\r\n"); break;
//             case 't': key_up(); printf("key_up\r\n"); break;
//             case 'g': key_down(); printf("key_down\r\n"); break;
//             default : crawler_brake(); break;
//         }
//     }
// }

 void ps3_get_data()
 {
 	ps3.get_data(data);
 }

void reference_ps3() {
    //  クローラー    
    if (data[PS3::UP]){
        front();
        printf("front\r\n");
    }
    if (data[PS3::DOWN]){
        back();
        printf("back\r\n");
    }
    if (data[PS3::LEFT]){
        left();
        printf("left\r\n");
    }
    if (data[PS3::RIGHT]){
        right();
        printf("right\r\n");
    }
    if (data[PS3::L1 && PS3::LEFT]){
        turn_left();
        printf("turn left\r\n");
    }
    if (data[PS3::L1 && PS3::RIGHT]){
        turn_right();
        printf("turn right\r\n");
    }
    // オーブ
    if (data[PS3::R1 && PS3::CROSS]){
        orb_drop();
        printf("orb_drop\r\n");
    }
    // キー
    if (data[PS3::TRIANGLE]){
        key_up();
        printf("key_up\r\n");
    }
    if (data[PS3::CROSS]){
        key_down();
        printf("key_down\r\n");
    }
    if (data[PS3::SQUARE]){
        key_catch();
        printf("key_catch\r\n");
    }
    if (data[PS3::CIRCLE]){
        key_release();
        printf("key_release\r\n");
    }
}
