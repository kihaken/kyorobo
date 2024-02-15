// ここではkeyのリミットスイッチは基準点のみつける予定 (エンコーダーを3個使用)
// これはトルク制御 (たぶん速度制御できる余裕ない)

#include "mbed.h"
#include "rbms.h"

#define CRAWLER 3000 // torque
#define ORB 2000
#define KEY_X 800
#define KEY_Y 1000
#define DATA_SIZE 8 // PS3
#define ORB_ROTE 60 // エンコーダーに指定する回転角
#define KEY_X_ROTE 2000 
#define KEY_Y_ROTE 15000

UnbufferedSerial pc(USBTX, USBRX, 115200);
UnbufferedSerial ps3(p10, p11, 2400);
CAN can(p30, p29); // ロボマス関係
rbms crawler(can, 0, 4);
rbms orb(can, 0, 1);
rbms key(can, 0, 2);
InterruptIn a(p27); // エンコーダー用のピン宣言
InterruptIn b(p6);
// 基準点, 目標点
DigitalIn limit_x[2] = {DigitalIn(p15), 
						DigitalIn(p16)};
DigitalIn limit_y[2] = {DigitalIn(p17),
						DigitalIn(p18)};

void reference_pc();
void reference_ps3();
void front(); // 足回り
void back();
void left();
void right();
void turn_left();
void turn_right();
void brake();
void orb_drop(); // 回収機構
void key_catch();
void key_release();
void key_up();
void key_down();
void encoder(float *angle); // エンコーダー
void angle_reset();
void a_slit();
void b_slit();
void limit_update(DigitalIn limit, Limit_Type type);

int i = 0;
int _orb[1] = {0}; 
int _key[2] = {0}; // ツメ, 昇降
int _crawler[4] = {0}; // 左前, 右前, 右後, 左後
int data[8] = {0}; // PS3
float angle = 0; 
int passed_slit = 0; // エンコーダー
enum Encoder_Type{ // どの機構に関するエンコーダーか
	None,
    orb_rote,
    key_x_rote,
    key_y_rote
} encoder_type;
encoder_type = None;
enum Limit_Type{
	none,
	x_zero,
	x_goal,
	y_zero,
	y_goal
}limit_type;
limit_type = none;

int main(){
    angle_reset();
    while(1){
        // リミットスイッチの状態をチェック
        for(i = 0;i < 2;i++){
			limit_update(limit_x[i],limit_type);
			limit_update(limit_y[i],limit_type);
		}
        encoder(&angle); // 角度取得後、angleに格納
        reference_pc();
        reference_ps3();
        // 指定した角度分回っていたら止める(brakeで上書き)
        switch (encoder_type){
            case orb_rote : if (angle == ORB_ROTE) {_orb[0] = 0; break;}
            case key_x_rote : if (angle == KEY_X_ROTE) {_key[0] = 0; break;}
            case key_y_rote : if (angle == KEY_Y_ROTE)  {_key[1] = 0; break;}
        }
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

void brake(){
    _crawler[0] = 0;
    _crawler[1] = 0;
    _crawler[2] = 0;
    _crawler[3] = 0;
}

void orb_drop(){
    _orb[0] = ORB;
    encoder_type = orb_rote;
}

void key_catch(){
    _key[0] = KEY_X_ROTE;
    encoder_type = key_x_rote;
    limit_type = x_goal;
}

void key_release(){
    _key[0] = -KEY_X_ROTE;
    encoder_type = key_x_rote;
    limit_type = x_zero;
}

void key_up(){
    _key[1] = KEY_Y_ROTE;
    encoder_type = key_y_rote;
    limit_type = y_goal;
}

void key_down(){
    _key[1] = -KEY_Y_ROTE;
    encoder_type = key_y_rote;
    limit_type = y_zero;
}

void encoder(float *angle){
    void a_slit();
    void b_slit();

    int passed_slit = 0; // スリットをいくつ通過したか

    a.rise(a_slit);
    a.fall(a_slit);
    b.rise(b_slit);
    b.fall(b_slit);
    *angle = 0.45f * passed_slit; // 1割り込みごとに進む角度ｘ割り込みが行われた回数
    printf("angle : %d.%d\r\n",(int)angle, (int)((*angle - (int)angle) * 100.0f));
}

void a_slit(){
    if (a != b){
        passed_slit++; // a相が立ち上がった時、b相の信号と異ったら正転
    } else {
        passed_slit--;
    }
}

void b_slit(){
    if (a == b){
        passed_slit++; // b相が立ち上がった時、a相の信号と同じだったら正転
    } else {
        passed_slit--;
    }
}

void angle_reset(){
    angle = 0;
}

void limit_update(DigitalIn limit, Limit_Type type)
{
	if (type == none) return;
	switch (type){
		case x_zero : 
		case x_goal : if (limit) {_key[0] = 0; angle_reset(); break;}
		case y_zero : 
		case y_goal : if (limit) {_key[1] = 0; angle_reset(); break;}
	}
}

void reference_pc(){
    if (pc.readable()) {
        char getc;
        pc.read(&getc, 2);
        switch(getc){
            case 'w': front(); printf("front\r\n"); break;
            case 'a': left(); printf("left\r\n"); break;
            case 's': back(); printf("back\r\n"); break;
            case 'd': right(); printf("right\r\n"); break;
            case 'q': turn_left(); printf("turn_left\r\n"); break;
            case 'e': turn_right(); printf("turn_right\r\n"); break;
            case 'x': orb_drop(); printf("orb_drop\r\n"); break;
            case 'h': key_catch(); printf("key_catch\r\n"); break;
            case 'f': key_release(); printf("key_release\r\n"); break;
            case 't': key_up(); printf("key_up\r\n"); break;
            case 'g': key_down(); printf("key_down\r\n"); break;
        }
    }
}

void reference_ps3() {
    if (ps3.readable()) {
        for (i = 0; i < DATA_SIZE; i++) {
            data[i] = ps3.read(&data[i], 1);
        }
    }
    if (data[0] == 0x80) {
        if (data[1] == 0x00) {
            if (data[2] == 0x01) {
                printf("↑\r\n");
                key_up();
                printf("Key_up\r\n");
            } else if (data[2] == 0x02) {
                printf("↓\r\n");
                key_down();
                printf("Key_down\r\n");
            } else if (data[2] == 0x04) {
                printf("→\r\n");
                key_catch();
                printf("key_catch\r\n");
            } else if (data[2] == 0x08) {
                printf("←\r\n");
                key_release();
                printf("key_release\r\n");
            } else if (data[2] == 0x10) {
                printf("△\r\n");
                front();
                printf("front\r\n");
            } else if (data[2] == 0x20) {
                printf("×\r\n");
                back();
                printf("back\r\n");
            } else if (data[2] == 0x40) {
                printf("○\r\n");
                right();
                printf("right\r\n");
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
            left();
            printf("left\r\n");
        } else if (data[1] == 0x02) {
            printf("L1\r\n");
            turn_left();
            printf("turn_left\r\n");
        } else if (data[1] == 0x04) {
            printf("L2\r\n");
        } else if (data[1] == 0x08) {
            printf("R1\r\n");
            turn_right();
            printf("turn_right");
        } else if (data[1] == 0x10) {
            printf("R2\r\n");
        }
    } else {
        printf("The data is different\r\n");
    }
}
