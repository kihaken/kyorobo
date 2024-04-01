// m2006�͏������ق�(0) m3508�͑傫���ق�(1)
// ���CAN�œ���ނ̃��[�^�[8�܂Ő���\
// ���x����̎��͓����ɂS�܂łɂ��Ă�������
// �p�x�擾��Thread�𗧂Ă�can.read(msg)

#include "mbed.h"
#include "robo_ms.h"
#include "rote_robo_ms.h"
#include "rbms.h"

CAN can(p30, p29);
CANMessage msg;
rbms test(can, 0, 2);
UnbufferedSerial pc(USBTX, USBRX, 115200);
rote_robo_ms rote_robo_ms_motor1(can, test, 0, 16);
rote_robo_ms rote_robo_ms_motor2(can, test, 1, 16);
Thread thread1;

int motor[1] = 0;
void motor1_func();

int main() {
	motor[0] = 1000;
	thread1.start(motor1_func);
	while (true) {
		ThisThread::sleep_for(10ms);
	}
}

void motor1_func() {
	test.rbms_send(motor);
	ThisThread::sleep_for(2ms);
}