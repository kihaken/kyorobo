// 上から順に
// B相 (p6)
// 5v  (VU)
// A相 (p27)
// GND (GND)
// につなぐ

#include "mbed.h"

UnbufferedSerial pc(USBTX, USBRX, 115200);
InterruptIn a(p27);
InterruptIn b(p6);

void a_slit();
void b_slit();

int passed_slit = 0;
float angle = 0;

int main(){
    while (true){
        a.rise(a_slit);
        a.fall(a_slit);
        b.rise(b_slit);
        b.fall(b_slit);
        angle = 0.45f * passed_slit; // 1割り込みごとに進む角度ｘ通過したスリット数
        printf("angle : %d.%d\r\n",(int)angle, (int)((angle - (int)angle) * 100.0f));
        ThisThread::sleep_for(10ms);
    }
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




////////////////////////////////////////////////////////////////////////////////////////
//   　　↑                                                                            //
//   　　|          _____________           _____________            __________       //
//   　　|       ↑  |           |  ↓     ↑  |           |  ↓      ↑  |                //
//   　　|      a.rise()     a.fall()   a.rise()     a.fall()    a.rise()             //
//   　　| A相      |           |           |           |            |                //
//   　　|          |           |           |           |            |                //
//   　　|          |           |           |           |            |                //
//  電 　|     _____            _____________           _____________                 //
//  圧 　|                                                                            //
//   　　|                                                                            //
//  [v]　|                                                                            //
//   　　|                _____________           _____________           _______     //
//   　　|             ↑  |           |  ↓     ↑  |           |  ↓     ↑  |           //
//   　　|            b.rise()     b.fall()   b.rise()     b.fall()   b.rise()        //
//   　　| B相            |           |           |           |           |           //
//   　　|                |           |           |           |           |           //
//   　　|                |           |           |           |           |           //
//   　　|     ____________            ____________           _____________           //
//   　　|                                                                            //
//   　　|                                                                            //
//   　　|-----------------------------------------------------------------------→    //
//   　　                                 時間　[ms]                                  //
////////////////////////////////////////////////////////////////////////////////////////
