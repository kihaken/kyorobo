// mbedOS 6.15
// クローラー制御のみしか記述していない とりあえず危なくない範囲でぶち回す
// PS3_SBDBT5V.hの使い方をまだ習得していないのでpcからの制御のみ

#include "mbed.h"
#include "rbms.h"

UnbufferedSerial pc(USBTX, USBRX, 115200);
CAN can(p30, p31);
rbms crawler(can, 0, 4);

int i;
char getc;
int crawler_state[4] = {0};

void front();
void back();
void left();
void right();
void brake();

int main(){
    while(true){
        for (i = 0; i < 4; i++){
            if (crawler.rbms_send(crawler_state[])){
                printf("Send\r\n");
            }
        }

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
        }
    }
}

void front(){
    crawler_state[0] =  1000;
    crawler_state[1] = -1000;
    crawler_state[2] = -1000;
    crawler_state[3] =  1000;
}

void back(){
    crawler_state[0] = -1000;
    crawler_state[1] =  1000;
    crawler_state[2] =  1000;
    crawler_state[3] = -1000;
}

void left(){
    crawler_state[0] = -1000;
    crawler_state[1] =  1000;
    crawler_state[2] = -1000;
    crawler_state[3] =  1000;
}

void right(){
    crawler_state[0] =  1000;
    crawler_state[1] = -1000;
    crawler_state[2] =  1000;
    crawler_state[3] = -1000;
}

void brake(){
    crawler_state[0] = 0;
    crawler_state[1] = 0;
    crawler_state[2] = 0;
    crawler_state[3] = 0;
}