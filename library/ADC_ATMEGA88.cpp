#ifndef ADC_ATMEGA88_H_
#define ADC_ATMEGA88_H_

namespace Adc{
	void EnableInterrupt(){
		sbi(ADCSRA,3);	//A/D�ϊ��������荞�݋���
	}
	inline void setREFS(char refs){
		changebyte(ADMUX, refs<<6, 0b11000000);
	}
	inline void setMUX(char mux){
		changebyte(ADMUX, mux, 0b00001111);
	}
	inline void setADPS(char adps){
		changebyte(ADCSRA, adps, 0b00000111);
	}
	unsigned int __doAdc(){
		if( checkbit(ADCSRA,7)==0 ){	//A/D��������Ă��Ȃ��A
			sbi(ADCSRA,7);		//A/D����
			__doAdc();	//����N����ADC�������̂��߂̋��
		}
		sbi(ADCSRA,6);	//A/D�ϊ��J�n
		while( checkbit(ADCSRA,4)==0 );	//�ϊ������܂őҋ@
		sbi(ADCSRA,4);	//A/D�ϊ��������荞�ݗv���׸�̃N���A�i1�̏������݁j
		return ADCW;
	}

	unsigned int getBit(unsigned char channel){	//�����悻88�������ԕK�v(8MHz�Ȃ�11us)
		unsigned int result;
		setMUX(channel);
		setREFS(0b01);
		setADPS(0b111);
		ADCSRB=0x00;
		changebyte(DIDR0, 1<<channel, 0b00111111);	//�޼��ٓ��͋֎~(ADC5D�`0D)
		result = __doAdc();
		DIDR0 = 0x00;
		return result;
	}
	float getVolt(unsigned char channel, float refVolt=5.0){
		return getBit(channel) * refVolt / 1024;
	}

	float getVolt1v1(unsigned char channel, unsigned char tryNum=100){
		int before=-1, now=0;
		setMUX(0b1110);
		setREFS(0b01);
		setADPS(0b111);
		ADCSRB=0x00;
		for(int i=0; i<tryNum; i++){	//���ʂ����肷��܂Ń��[�v
			now = __doAdc();
			if(before == now){
				break;
			}
			else{
				before = now;
			}
		}
		return getBit(channel) * 1.1 / now;
	}

	//ADC�߰�:channel(0~7),���荞�ݕ�����:division(2,4,8,16,32,64,128)
	//�w����߰Ăɓ��͂��ꂽ��۸ޒl��ǂސݒ�
	void ADC_single_init(char channel, int division){
		//������ݒ�
		switch(division){
			case 2:
				division=0b000;
			break;
			case 4:
				division = 0b010;
			break;
			case 8:
				division = 0b011;
			break;
			case 16:
				division = 0b100;
			break;
			case 32:
				division = 0b101;
			break;
			case 64:
				division = 0b110;
			break;
			case 128:
			default:
				division = 0b111;
			break;
		}

		ADMUX = 0b01000000 | channel;
	//	          ||||++++--A/D���ّI��(MUX3�`0)
	//	          |||+------�\��
	//	          ||+-------0:AD�ϊ����ʂ��E�����I��--1:��
	//	          ++--------��d���I��(REFS1�`0)

		/*
		A/D���ّI��(AMUX3�`0)
		-----------------------------------------
		|0000|0001|0010|0011|0100|0101|0110|0111|
		|----|----|----|----|----|----|----|----|
		| PC0| PC1| PC2| PC3| PC4| PC5| PC6| PC7|
		|----|----|----|----|----|----|----|----|
		|ADC0|ADC1|ADC2|ADC3|ADC4|ADC5|ADC6|ADC7|
		-----------------------------------------
		*/

		ADCSRA = 0b11100000 | division;
	//	           |||||+++-- A/D�ϊ��ۯ��I��(ADPS2�`0)
	//	           ||||+----- A/D�ϊ��������荞�݋���
	//	           |||+------ A/D�ϊ��������荞�ݗv���׸�
	//	           ||+------- A/D�ϊ������N������
	//	           |+-------- A/D�ϊ��J�n
	//	           +--------- A/D����

		/*
		A/D�ϊ��ۯ��I��(ADPS2�`0)
		--------------------------------------------------
		| 000 | 001 | 010 | 011 | 100 | 101 | 110 |  111 |
		|-----|-----|-----|-----|-----|-----|-----|------|
		| CK/2| CK/2| CK/4| CK/8|CK/16|CK/32|CK/64|CK/128|
		--------------------------------------------------
		*/

		ADCSRB = 0b0000000;
	//	           ||||+++--A/D�ϊ������N���v���I��(ADTS2�`0)
	//	           ||++-----�\��
	//	           |+-------��۸ޔ�r�푽�d�틖��
	//	           +--------�\��

		/*
		A/D�ϊ������N���v���I��(ADTS2�`0)
		---------------------------------
		| 02| 01| 00|                   |
		|---|---|---|-------------------|
		| 0 | 0 | 0 |�A���ϊ�����       |
		| 0 | 0 | 1 |��۸ޔ�r��        |
		| 0 | 1 | 0 |�O�����荞�ݗv��0  |
		| 0 | 1 | 1 |���/����0��rA��v |
		| 1 | 0 | 0 |���/����0���      |
		| 1 | 0 | 1 |���/����1��rB��v |
		| 1 | 1 | 0 |���/����1���      |
		| 1 | 1 | 1 |���/����1�ߊl�v��  |
		---------------------------------
		*/
		DIDR0 = 0b00111111;
	//	          ||++++++--1:�޼��ٓ��͋֎~(ADC5D�`0D)
	//	          ++--------�\��
		ADCW = 0;
		//ADCH,ADCL(ADCW)
		//ADCW=AD�ϊ����ʂ̒l������0�`1024(10bit)
	}
}
#endif