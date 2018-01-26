#ifndef RING_ARRAY_H_
#define RING_ARRAY_H_

#include <stdlib.h>

template <class freeType>
class RingArray{
	protected:
	volatile freeType* buffer;
	//head=�f�[�^�������Ă���擪
	//tail=�f�[�^�������ĂȂ�����
	volatile uint16_t head, tail;
	volatile uint16_t bufferNum;

	public:
	//�R���X�g���N�^�E�f�g���N�^>>>>>
	RingArray(){
		head=0;
		tail=0;
		bufferNum=0;
	}
	RingArray(uint16_t _bufferNum){
		head=0;
		tail=0;
		setBufferNum(_bufferNum);
	}
	~RingArray(){
		free((void*)buffer);
	}
	//<<<<<�R���X�g���N�^�E�f�g���N�^

	void setBufferNum(uint16_t _bufferNum){
		bufferNum = _bufferNum+1;
	//	buffer = new char[bufferNum];	//���I�������m��
		buffer = (freeType*)malloc(sizeof(freeType)*bufferNum);	//���I�������m��
	}
	void init(uint16_t _bufferNum){
		setBufferNum(_bufferNum);
	}

	//�擪�ɑ}��
	bool firstIn(freeType data){
		if(isFull()){	//buffer��full
			// out();	//�Â��f�[�^���̂Ă�
			// buffer[tail]=data;
			// tail = (tail+1) % bufferNum;
			return false;
		}
		else{
			head = (head+(bufferNum-1)) % bufferNum;
			buffer[head]=data;
			return true;
		}
	}
	//return=����
	bool lastIn(freeType data){	//�ǉ�
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

	//�����o�����f�[�^�A��Ȃ�-1
	int16_t firstOut(){
		freeType data;
		if( !isEmpty() ){	//buffer����łȂ�
			data = buffer[head];
			head = (head+1) % bufferNum;
			return data;
		}
		return -1;
	}
	//����������o���A��Ȃ�-1
	int16_t lastOut(){
		freeType data;
		if( !isEmpty() ){	//buffer����łȂ�
			tail = (tail+(bufferNum-1)) % bufferNum;
			data = buffer[tail];
			return data;
		}
		return -1;
	}

	bool isEmpty(){
		return head == tail;
	}
	bool isFull(){
		return head == (tail+1) % bufferNum;
	}
	void doEmpty(){
		head = tail;
	}
	uint16_t len(){//���L�f�[�^��
		return (tail+(bufferNum-head)) % bufferNum;
	}
	bool overWrite(freeType data, uint16_t adrs){
		if(adrs>=0){
			if(adrs>=len()){
				return false;
			}
			buffer[(head+adrs) % bufferNum] = data;
			return true;
		}
		else{
			if (-adrs>len()){
				return false;
			}
			buffer[(tail+(bufferNum+adrs)) % bufferNum] = data;
			return true;
		}
	}
	freeType look(int32_t adrs){
		if(adrs>=0){
			if(adrs>=len()){
				return 0;
			}
			return buffer[(head+adrs) % bufferNum];
		}
		else{
			if (-adrs>len()){
				return 0;
			}
			return buffer[(tail+(bufferNum+adrs)) % bufferNum];
		}
	}
	//�l���Ȃ��Ƃ���-1
	int32_t index(freeType value){
		for(uint16_t i=0; i<len(); i++){
			if(look(i)==value){
				return i;
			}
		}
		return -1;
	}
	uint16_t copy(freeType* array){
		uint16_t length = len();
		for(uint16_t i=0; i<length; i++){
			*(array+i) = buffer[(head+i) % bufferNum];
		}
		return length;
	}
};
#endif