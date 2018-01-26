//���얢�m�F
#ifndef STRING_CTRL_H_
#define STRING_CTRL_H_

#include <stdio.h>

void byteToStr(char byte, char* str){
	*(str+0)='0';
	*(str+1)='b';
	for(int i=7; 0<=i; i--){
		*(str+(9-i)) = ((byte>>i)&1)+'0';
	}
	*(str+10) = '\0';
}

uint16_t getStringLen(const char* str){
	uint16_t len=0;
	while(*(str+len)!='\0'){
		len++;
	}
	return len;
}
uint16_t getCharaNum(char* str, char chara){
	uint16_t num=0;
	while(1){
		if(*(str)=='\0'){
			break;
		}
		else if(*(str)==chara){
			num++;
		}
		str++;
	}
	return num;
}
//sprintf
//bool
//long double
//
void numToStr(unsigned int num, char *str, char format){
	switch(format){
		case 1:
			if(num==0){
				*(str+0)='F';
				*(str+1)='a';
				*(str+2)='l';
				*(str+3)='s';
				*(str+4)='e';
			}
			else{
				*(str+0)='T';
				*(str+1)='r';
				*(str+2)='u';
				*(str+3)='e';
			}
		break;
		case 2:
			*(str+0)='0';
			*(str+1)='b';
			for(int i=7; 0<=i; i--){
				*(str+(9-i)) = ((num>>i)&1)+'0';
			}
			*(str+10) = '\0';
		break;
		case 10:

		break;
		case 16:
			*(str+0)='0';
			*(str+1)='x';
			sprintf((str+2), "%x", num);	//1500byte���炢�H��
		break;
	}
}

int16_t strToNum(const char* str, uint16_t len){
	int16_t num=0;
	int16_t signedNum=1;
	if(*str=='+'){
		len--;
	}
	else if(*str=='-'){
		signedNum=-1;
		len--;
	}
	for(uint16_t i=0; i<len; i++){
		num *= 10;
		if( '0'<=*(str+i) && *(str+i)<='9'){
			num += *(str+i)-'0';
		}
		else{
			return 0;
		}
	}
	return num*signedNum;

}

//�߂�l
//�� : cmd������
//�O : checkCmd��trueCmd�ɑ΂��ď��Ȃ��itrueCmd���������checkCmdNum�̂ق������Ȃ��j
//�� : -�߂�l�����ڂ��Ԉ���Ă���A
//trueCmd��NULL�������K�v
int16_t cmdCheck(const char* trueCmd, const char* checkCmd, uint16_t checkCmdNum){
	uint16_t checkingTrueCmdNum=0, checkingCheckCmdNum=0;
	char checkChara;
	while(1){
		if(*(trueCmd+checkingTrueCmdNum) == '\0'){	//������cmd�Ƃ̔�r���Ō�(NULL����)�܂Ő��������Ƃ�
			return checkingCheckCmdNum;
		}
		if( checkCmdNum==checkingCheckCmdNum ){	//���S�ɔ�r�f�[�^�����������Ă��邩�ǂ���
			return 0;	//���S�ɔ�r�f�[�^�i�j�����������Ă��Ȃ�
		}

		checkChara = *(checkCmd+checkingCheckCmdNum);	//���v���Ă��邩���ׂ镶���̑��

		if(*(trueCmd+checkingTrueCmdNum) == checkChara){	//�ꕶ���̂Ƃ��ɍ��v���Ă��邩����
			checkingTrueCmdNum++;
		}
		else if(*(trueCmd+checkingTrueCmdNum) == '?'){	//���l�����ɍ��v���Ă��邩����
			if(checkChara<'0' || '9'<checkChara){
				return -checkingCheckCmdNum-1;
			}
			else{
				checkingTrueCmdNum++;
			}
		}
		else if(*(trueCmd+checkingTrueCmdNum) == '{'){	//���������J�n����( { )�̂Ƃ��ɍ��v���Ă��邩����
			bool flag=false;
			while(1){
				checkingTrueCmdNum++;
				if(*(trueCmd+checkingTrueCmdNum) == checkChara){
					flag=true;
				}
				else if(*(trueCmd+checkingTrueCmdNum) == '}'){	//���������I������( } )�̂Ƃ��ɍ��v���Ă��邩����
					if(flag){	//����cmd�ɍ��v���Ă�����
						checkingTrueCmdNum++;
						break;
					}
					else{
						return -checkingCheckCmdNum-1;
					}
				}
			}
		}
		else{	//�ǂ�ɂ����v���Ȃ������Ƃ�
			return -checkingCheckCmdNum-1;
		}
		checkingCheckCmdNum++;
	}
}

#endif
