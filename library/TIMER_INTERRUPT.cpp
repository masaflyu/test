//���얢�m�F
#ifndef TIMER_INTERRUPT_H_
#define TIMER_INTERRUPT_H_

#include <avr/interrupt.h>	//���荞�݃w�b�_
#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"

//ISR TIMER0_COMPA_vect()
namespace Tm0Irpt{
	//irpt_bit(OCR0A)*������/CLK���ƂɃ^�C�}�[���荞�݂𔭐�������ݒ�
	void setOvf(uint16_t division=0){
		if(division!=0){
			Tm0Ctrl::setDivision(division);
		}
		Tm0Ctrl::enableOvfIrpt();	//���/����0��ꊄ�荞�݋���
		sei();
	}
	void setBitA(uint8_t irpt_bit, uint16_t division=0){
		if(division!=0){
			Tm0Ctrl::setDivision(division);
		}
		Tm0Ctrl::setWGM(0b010);
		Tm0Ctrl::enableCompaIrpt();	//���/����0��rA���荞�݋���
		TCNT0 = 0;	//�J�E���g�l(�}�C�R���������Ă��鐔������)
		OCR0A = irpt_bit;	//irpt_bit�l
		sei();
		//���荞�݊Ԋu = irpt_bit(OCR0A) * ������ / CLK
	}
	void setUsecA(uint32_t usec){
		uint8_t irpt_bit;
		uint16_t division;
		Tm0Ctrl::getTopDivision(usec, &irpt_bit, &division);
		Tm0Irpt::setBitA(irpt_bit, division);
		sei();
	}
}

//ISR TIMER1_COMPA_vect()
namespace Tm1Irpt{
	void setOvf(uint16_t division=0){
		if(division!=0){
			Tm1Ctrl::setDivision(division);
		}
		Tm1Ctrl::enableOvfIrpt();	//���/����1��ꊄ�荞�݋���
		sei();
	}
	void enableCapt(uint16_t division=0){
		if(division!=0){
			Tm1Ctrl::setDivision(division);
		}
		Tm1Ctrl::enableCaptIrpt();
		sei();
	}
	void setBitA(uint8_t irpt_bit, uint16_t division=0){
		if(division!=0){
			Tm1Ctrl::setDivision(division);
		}
		Tm1Ctrl::setWGM(0b0100);
		Tm1Ctrl::enableCompaIrpt();		//���/����1��rA���荞�݋���
		TCNT1 = 0;	//�J�E���g�l(�}�C�R���������Ă��鐔������)
		OCR1A = irpt_bit;	//irpt_bit�l
		sei();
		//���荞�݊Ԋu = irpt_bit(OCR0A) * ������ / CLK
	}
	void setUsecA(uint32_t usec){
		uint16_t irpt_bit, division;
		Tm1Ctrl::getTopDivision(usec, &irpt_bit, &division);
		Tm1Irpt::setBitA(irpt_bit, division);
		sei();
	}
}

namespace Tm2Irpt{
	void setOvf(uint16_t division=0){
		if(division!=0){
			Tm2Ctrl::setDivision(division);
		}
		Tm2Ctrl::enableOvfIrpt();	//���/����0��ꊄ�荞�݋���
		sei();
	}
	void setBitA(uint8_t irpt_bit, uint16_t division=0){
		if(division!=0){
			Tm2Ctrl::setDivision(division);
		}
		Tm2Ctrl::setWGM(0b010);
		Tm2Ctrl::enableCompaIrpt();	//���/����0��rA���荞�݋���
		TCNT2 = 0;	//�J�E���g�l(�}�C�R���������Ă��鐔������)
		OCR2A = irpt_bit;	//irpt_bit�l
		//���荞�݊Ԋu = irpt_bit(OCR0A) * ������ / CLK
		sei();
	}
	void setUsec(uint32_t usec){
		uint8_t irpt_bit;
		uint16_t division;
		Tm2Ctrl::getTopDivision(usec, &irpt_bit, &division);
		Tm2Irpt::setBitA(irpt_bit, division);
		sei();
	}
}

#endif