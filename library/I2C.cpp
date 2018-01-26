#ifndef I2C_H_
#define I2C_H_


#include ".\BIT_CTRL.cpp"
#include ".\TRANSCEIVER_QUEUE.cpp"
#include ".\RING_ARRAY.cpp"
#include <avr/interrupt.h>	//���荞�݃w�b�_


class I2c{
	public:
	class I2cTransceiver : public TransceiverQueue{
		public:
		volatile bool comuniFlag;
		RingArray snycLen;

		I2cTransceiver() : TransceiverQueue(){
			comuniFlag=false;
			snycLen.setBufferNum(20);
		}
		int16_t rxOut(){//overRide
			int16_t data = receiver.firstOut();
			if(data != -1){
				snycLen.overWrite( snycLen.look(0)-1 ,0);
				if(snycLen.look(0)==0){
					snycLen.firstOut();
				}
			}
			return data;
		}
		int16_t readSnycData(char* dataAdrs){	//�߂�l��1�ʐM�f�[�^��
			int16_t lastLen = snycLen.firstOut();
			if(lastLen != -1){
				for(int16_t i=0; i<lastLen; i++){
					*(dataAdrs+i) = receiver.firstOut();
				}
			}
			return lastLen;
		}
	};
	I2cTransceiver master, slave;

	volatile uint8_t opponetAddress;
	volatile uint16_t masterGetDataLen;		//master�v���f�[�^��
	volatile uint16_t masterSnycDataNum;	//master�̗݌v�ʐM�f�[�^��

	I2c(){	//�R���X�g���N�^
		slave.init(50,50);
		master.init(50,50);
	}
	void begin(uint8_t myAddress, uint32_t speed=400000){
		setMyAddress(myAddress);
		setSpeed(speed);
		//	sbi(TWAR, TWGCE);   //��ČĂяo�����o����
		sbi(TWCR, TWIE);	//���荞�݋���
		sbi(TWCR, TWEN);	//TWI���싖��
		setTWCR(0,0,0,1);
		TWAMR=0x00;	//�}�X�N
		sei();
	}
	void stop(){
		cbi(TWCR, TWEN);
		cbi(TWCR, TWIE);
	}
	void setSpeed(uint32_t speed){
		int bunsyu=1;
		setDivision(1);
		TWBR = uint8_t( (F_CPU/speed-16)/(2*bunsyu) );
	}
	void setDivision(uint8_t division){
		switch(division){
			case 1:
			changebyte(TWSR, 0x00, 0x03);
			break;
			case 4:
			changebyte(TWSR, 0x01, 0x03);
			break;
			case 16:
			changebyte(TWSR, 0x02, 0x03);
			break;
			default:	//case 64:
			changebyte(TWSR, 0x03, 0x03);
			break;
		}
	}
	void setMyAddress(uint8_t address){
		TWAR = (address&0x7F)<<1;
	}
	char getTWDR(){
		return TWDR;
	}
	//TWSTA=�J�n������������	TWSTO=��~������������
	//TWINT=TWI���荞�ݗv���t���O	TWEA =TWI���싖��
	void setTWCR(char twsta, char twsto, char twint, char twea){
		char twcr = (twint<<TWINT)| (twea<<TWEA)| (twsta<<TWSTA)| (twsto<<TWSTO);//| (1<< TWEN);
		changebyte(TWCR, twcr, 0xF0);
	}
	uint8_t getTWSR(){
		return (TWSR&0xF8);
	}

//MASTER+WRITE(�f�[�^���M��)
	void beginTransmission(uint8_t address){
		opponetAddress = (address<<1) | 0x00;	//LSB=0��Write�ݒ�
		master.transmitter.doEmpty();
	}
//MASTER+READ(�f�[�^�v����)
	void requestFrom(uint8_t address, uint16_t count){
		opponetAddress = (address<<1) | 0x01;	//LSB=0��Read�ݒ�
		masterGetDataLen = count;
	}
//MASTER+(WRITE_READ)���M�J�n
	uint16_t endTransmission(bool wait=true){
		while( master.comuniFlag || slave.comuniFlag );
		master.comuniFlag = true;
		setTWCR(1,0,1,1);	//�J�n�������M
		if(wait){
			while(master.comuniFlag);	//�ʐM�I���܂Ń��[�v
			return masterSnycDataNum;
		}
		return 0;
	}
//<<<<<MASTER
};
I2c i2c;



ISR(TWI_vect){
	static uint8_t slaveLenNum=0;
	char twsr=i2c.getTWSR();

	switch(twsr){
//MASTER>>>>>
		case 0x08:	//�J�n�������M����
		case 0x10:	//�đ��J�n�������M����
			TWDR = i2c.opponetAddress;	//�X���[�u�A�h���X�Z�b�gSLA+W
			i2c.setTWCR(0,0,1,1);
			i2c.masterSnycDataNum=0;
		break;

//MASTER+WRITE(�f�[�^���M��)>>>>>
		case 0x18:	//�A�h���X���M��ACK��M
		case 0x28:	//�f�[�^���M��ACK����M
			if( !(i2c.master.transmitter.isEmpty()) ){	//��łȂ��Ȃ�
				TWDR = i2c.master.txOut();
				i2c.masterSnycDataNum++;
				i2c.setTWCR(0,0,1,1);
			}
			else{
				i2c.setTWCR(0,1,1,1);	//STOP���M
				i2c.master.comuniFlag = false;
			}
		break;
		case 0x20:	//�A�h���X���M��NACK��M
		case 0x30:	//�f�[�^���M��NACK���M
			i2c.setTWCR(0,1,1,1);	//STOP���M
			i2c.master.comuniFlag = false;
		break;
//<<<<<MASTER+WRITE(�f�[�^���M��)

//MASTER+READ(�f�[�^�v����)>>>>>
		case 0x40:	//�A�h���X���M+ACK��M
			i2c.setTWCR(0,0,1,1);	//ACK�ŉ����\��
		break;
		case 0x50:	//�f�[�^��M��ACK��������
			i2c.master.rxIn(TWDR);
			i2c.masterSnycDataNum++;
			if(i2c.masterSnycDataNum < i2c.masterGetDataLen){
				i2c.setTWCR(0,0,1,1);	//ACK����
			}
			else{
				i2c.setTWCR(0,0,1,0);	//NACK����
			}
		break;
		case 0x58:	//NACK��������
		case 0x48:	//�A�h���X���M�������Ȃ�
			i2c.setTWCR(0,1,1,1);	//STOP���M
			i2c.master.comuniFlag = false;
		break;
//<<<<<MASTER+READ(�f�[�^�v����)

//SLAVE+READ(�f�[�^���󂯎�鑤)>>>>>
		case 0x60:	//SLA+W����M��ACK��������
		case 0x70:	//��ČĂяo����M��ACK����
			i2c.slave.comuniFlag = true;
			slaveLenNum=0;
			i2c.setTWCR(0,0,1,1);	//ACK�����\��
		break;
		case 0x80:	//�f�[�^��M��ACK����
		case 0x90:	//��ČĂяo���Ńf�[�^��M��ACK����
			i2c.slave.rxIn(i2c.getTWDR());
			slaveLenNum++;
		 	i2c.setTWCR(0,0,1,1);	//ACK��M�\��
		break;
		case 0x88:	//�f�[�^��M��NACK����
		case 0x98:	//��ČĂяo���Ńf�[�^��M��NACK����
			i2c.slave.rxIn(i2c.getTWDR());
			slaveLenNum++;
			i2c.setTWCR(0,0,1,1);	//�ʐM�I��
			i2c.slave.comuniFlag = false;
		break;
		case 0xA0:	//��~����or�đ��������o
			i2c.setTWCR(0,0,1,1);	//�ʐM�I��
			i2c.slave.snycLen.lastIn(slaveLenNum);
			i2c.slave.comuniFlag = false;
		break;
		case 0x68:	//���A�h���X��M���ɒ���s�ށ�ACK����
		case 0x78:	//��ČĂяo���Œ���s�ށ�ACK����
			i2c.setTWCR(0,0,1,0);	//NACK�����\��
		break;
//<<<<<SLAVE+READ(�f�[�^���󂯎�鑤)

//SLAVE+WRITE(�f�[�^��ǂ܂�鑤)>>>>>
		case 0xA8:	//SLA+R����M��ACK����
		case 0xB0:	//���A�h���X��M(SLA+R)���ɒ���s�ށ�ACK����
			i2c.slave.comuniFlag = true;
		case 0xB8:	//�f�[�^���M��ACK����
			if( !(i2c.slave.transmitter.isEmpty()) ){	//�ŏI�f�[�^�𑗐M���̔��f
				TWDR = i2c.slave.txOut();
				i2c.setTWCR(0,0,1,1);	//ACK��M�\��
			}
			else{
				i2c.setTWCR(0,0,1,0);	//NACK��M�\��
			}
		break;
		case 0xC0:	//�f�[�^���M��NACK��M
		case 0xC8:	//�ŏI�f�[�^�o�C�g���M�����̂Ɂ�ACK��M
			i2c.setTWCR(0,0,1,1);	//�ʐM�I��
			i2c.slave.comuniFlag = false;
		break;
//<<<<<SLAVE+WRITE(�f�[�^��ǂ܂�鑤)

//��O>>>>>
		case 0x38:	//�A�h���X���Mor�f�[�^���M���p�X����
			i2c.setTWCR(1,0,1,1);	//�\�ɂȂ莟��J�n
		break;
		case 0x00:	//�s���ȊJ�n����/��~�����ł��޽�ُ�
		default:
			i2c.setTWCR(0,1,1,1);	//STOP���M
		break;
//<<<<<��O
	}
}
#endif