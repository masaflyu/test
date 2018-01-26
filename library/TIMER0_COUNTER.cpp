//���얢�m�F
#ifndef TIMER0_COUNTER_H_
#define TIMER0_COUNTER_H_

#include <avr/interrupt.h>	//���荞�݃w�b�_
#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"

namespace Tm0Cntr{
	volatile uint32_t count=0;
	void begin(int division){
		Tm0Ctrl::setDivision(division);
		Tm0Ctrl::enableOvfIrpt();
		count=0;
		TCNT0=0;
		sei();
	}
	void stop(){

	}
	uint32_t getUsec(){
		return (count*256+TCNT0)*Tm0Ctrl::getDivision() / (F_CPU/1000000);
	}
	uint32_t getMsec(){
		return (count*256+TCNT0)*Tm0Ctrl::getDivision() / (F_CPU/1000);
	}
	uint32_t getSec(){
		return (count*256+TCNT0)*Tm0Ctrl::getDivision() / (F_CPU);
	}
	inline void upCount(){
		count++;
	}
}
ISR(TIMER0_OVF_vect){
	sei();
	Tm0Cntr::upCount();
}
#endif