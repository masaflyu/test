/*
����
pwm��off�Ǝ���pwm��ON�̊Ԃɂ�7.28us�ȏ�K�v
  pwm1    pwm2        pwm1    pwm2
__|---|___|--|________|---|___|--|__
       <->                 <->
       7.28us�ȏ�          7.28us�ȏ�
�v���s���̂ق��Aor��H��ʂ��āA�C���v�b�g�L���v�`��PIN�ɐڑ����Ȃ���΂Ȃ�Ȃ�
PWM�M���ɂ���ē����ɕ����s����ON�̏u�Ԃ������Ă͂Ȃ�Ȃ�
*/


#ifndef MEASURE_PWM_CAPT_H_
#define MEASURE_PWM_CAPT_H_

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include <avr/interrupt.h>



class MeasureRc{
	public:
	struct timeDuration{
		uint16_t startCount;
		uint16_t startTcnt;
		uint16_t endCount;
		uint16_t endTcnt;
	};
	timeDuration onTime, bufferOnTime;
	bool updateFlag;
	avrPin measurePin;

	static float bitToUsec;
	static uint8_t measureNum;
	static uint16_t nowCount;

	MeasureRc(avrPin pin);
	static void begin(uint16_t division);
	void end();
	float getHighUsec();
	float getLowUsec();
	bool isUpdate();
	bool isLowUpdate();
	void __pulseUpAction(uint16_t _upTcnt);
	void __pulseDownAction(uint16_t _downTcnt);

	static float timeDurationToUsec(timeDuration td);
	static void __countUp();
	static bool __isUpMode();
	static bool __isDownMode();
	static void __setUpMode();
	static void __setDownMode();
};
uint8_t MeasureRc::measureNum=0;
uint16_t MeasureRc::nowCount=0;
float MeasureRc::bitToUsec=0;
MeasureRc *objPtr[32];

MeasureRc::MeasureRc(avrPin pin){	//�R���X�g���N�^(�ϐ��̏������ƃ^�C�}�[�ݒ�)
	objPtr[measureNum] = this;
	measureNum++;
	measurePin = pin;
	cbi(*(measurePin.ddr), measurePin.pinNum);
	sbi(*(measurePin.port), measurePin.pinNum);
	nowCount=0;
	updateFlag=false;
}
void MeasureRc::begin(uint16_t division=256){
	Tm1Ctrl::setDivision(division);	//���x12.8us�@�v���\���Ԗ�55ks=15h
	division = Tm1Ctrl::getDivision();
	MeasureRc::bitToUsec = division/(F_CPU/1000000.0);	//1bit=12.8us
	Tm1Ctrl::enableCaptIrpt();
	Tm1Ctrl::enableOvfIrpt();//ovf���荞�݋���
	MeasureRc::__setUpMode();
	sei();
}
void MeasureRc::end(){
	Tm1Ctrl::disableCaptIrpt();
}
float MeasureRc::getHighUsec(){
	updateFlag=false;
	return MeasureRc::timeDurationToUsec(bufferOnTime);
}
inline bool MeasureRc::isUpdate(){
	return updateFlag;
}
float MeasureRc::timeDurationToUsec(timeDuration td){
	if(td.endCount>=td.startCount){
		return (((float)td.endCount        -(float)td.startCount)*65536.0 + ((float)td.endTcnt-(float)td.startTcnt)) * MeasureRc::bitToUsec;
	}
	else{
		return (((float)td.endCount+65536.0-(float)td.startCount)*65536.0 + ((float)td.endTcnt-(float)td.startTcnt)) * MeasureRc::bitToUsec;
	}
}

inline void MeasureRc::__pulseUpAction(uint16_t upTcnt){
	onTime.startCount = nowCount;
	onTime.startTcnt  = upTcnt;
}
inline void MeasureRc::__pulseDownAction(uint16_t downTcnt){
	onTime.endCount = nowCount;
	onTime.endTcnt  = downTcnt;
	bufferOnTime = onTime;	//�o�b�t�@�ɃR�s�[
	updateFlag = true;
}
inline void MeasureRc::__countUp(){
	MeasureRc::nowCount++;
}
inline void MeasureRc::__setUpMode(){
	sbi(TCCR1B,ICES1);
}
inline void MeasureRc::__setDownMode(){
	cbi(TCCR1B,ICES1);
}
inline bool MeasureRc::__isUpMode(){
	return checkBit(TCCR1B, ICES1);
}
inline bool MeasureRc::__isDownMode(){
	return !checkBit(TCCR1B, ICES1);
}


ISR(TIMER1_OVF_vect){
	MeasureRc::__countUp();
}

ISR(TIMER1_CAPT_vect){
	static uint8_t measuringObj=0;
	sbi(PORTD,4);
	//uint16_t _icr1 = ICR1;

	if(MeasureRc::__isDownMode()){	//�������������̂Ȃ�
		objPtr[measuringObj]->__pulseDownAction(ICR1);
		MeasureRc::__setUpMode();
	}
	else{	//�������������̂Ȃ�
		for(uint8_t i=0; i<MeasureRc::measureNum; i++){
			if( checkBit( *(objPtr[i]->measurePin.pin), objPtr[i]->measurePin.pinNum )==1 ){
				measuringObj = i;
				objPtr[measuringObj]->__pulseUpAction(ICR1);
				MeasureRc::__setDownMode();
				break;
			}
		}
	}
	cbi(PORTD,4);
}
#endif