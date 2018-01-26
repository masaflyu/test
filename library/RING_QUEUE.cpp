#ifndef RING_QUEUE_H_
#define RING_QUEUE_H_

#include <stdlib.h>

class Queue{
	protected:
	public:
	volatile char* buffer;
	volatile uint16_t head, tail;
	volatile uint16_t bufferNum;

	public:
	//�R���X�g���N�^�E�f�g���N�^>>>>>
	Queue(){
		head=0;
		tail=0;
		bufferNum=0;
	}
	Queue(uint16_t _bufferNum){
		head=0;
		tail=0;
		setBufferNum(_bufferNum);
	}
	~Queue(){
		free((void*)buffer);
	}
	//<<<<<�R���X�g���N�^�E�f�g���N�^

	void setBufferNum(uint16_t _bufferNum){
		bufferNum = _bufferNum+1;
	//	buffer = new char[bufferNum];	//���I�������m��
		buffer = (char*)malloc(sizeof(char)*bufferNum);	//���I�������m��
	}
	void init(uint16_t _bufferNum){
		setBufferNum(_bufferNum);
	}

	//�����o�����f�[�^�A��Ȃ�-1
	int16_t out(){
		char data;
		if( !isEmpty() ){	//buffer����łȂ�
			data = buffer[head];
			head = (head+1) % bufferNum;
			return data;
		}
		return -1;
	}
	//return=����
	bool in(char data){	//�ǉ�
		if(isFull()){	//buffer��full
			// out();	//�Â��f�[�^���̂Ă�
			// buffer[tail]=data;
			// tail = (tail+1) % bufferNum;
			return false;
		}
		else{
			buffer[tail]=data;
			tail = (tail+1) % bufferNum;
			return true;
		}
	}
	bool isEmpty(){
		return head == tail;
	}
	bool isFull(){
		return head == (tail+1)%bufferNum;
	}
	void doEmpty(){
		head=tail;
	}
	uint16_t getBufferLen(){//���L�f�[�^��
		return (tail-head+bufferNum)%bufferNum;
	}
	//�l���Ȃ��Ƃ���0
	uint16_t untilValueLen(char value){
		for(uint16_t i=0; i<getBufferLen(); i++){
			if(buffer[(head+i)%bufferNum]==value){
				return i+1;
			}
		}
		return 0;
	}

	uint16_t copyBuffer(char* array){
		uint16_t len = getBufferLen();
		for(uint16_t i=0; i<len; i++){
			*(array+i) = buffer[(head+i) % bufferNum];
		}
		return len;
	}
};
#endif