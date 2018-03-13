#ifndef UART_H_
#define UART_H_

#include ".\BIT_CTRL.cpp"
#include ".\TRANSCEIVER_RINGARRAY.cpp"
#include <avr/interrupt.h>

#ifndef TX_BUFFER_NUM
#define TX_BUFFER_NUM 100
#endif
#ifndef RX_BUFFER_NUM
#define RX_BUFFER_NUM 100
#endif

#include <stdio.h>
#include <math.h>

#define SERIAL_5N1 0x00
#define SERIAL_6N1 0x02
#define SERIAL_7N1 0x04
#define SERIAL_8N1 0x06
#define SERIAL_5N2 0x08
#define SERIAL_6N2 0x0A
#define SERIAL_7N2 0x0C
#define SERIAL_8N2 0x0E
#define SERIAL_5E1 0x20
#define SERIAL_6E1 0x22
#define SERIAL_7E1 0x24
#define SERIAL_8E1 0x26
#define SERIAL_5E2 0x28
#define SERIAL_6E2 0x2A
#define SERIAL_7E2 0x2C
#define SERIAL_8E2 0x2E
#define SERIAL_5O1 0x30
#define SERIAL_6O1 0x32
#define SERIAL_7O1 0x34
#define SERIAL_8O1 0x36
#define SERIAL_5O2 0x38
#define SERIAL_6O2 0x3A
#define SERIAL_7O2 0x3C
#define SERIAL_8O2 0x3E

class Uart : public TransceiverQueue{
	public:
	Uart() : TransceiverQueue(50, 50){	}
	Uart(uint16_t txBuffer, uint16_t rxBuffer) : TransceiverQueue(txBuffer, rxBuffer){	}

//オーバーライド>>>>>
	int16_t txIn(char data){	//キューに追加した文字数
		bool success = transmitter.lastIn(data);
		if(success){
			sbi(UCSR0B, 5);
			return 1;
		}
		return 0;
	}
//<<<<<オーバーライド

	void enableTxInterrupt(){
		sbi(UCSR0B, 5);
	}
	void disableTxInterrupt(){
		cbi(UCSR0B, 5);
	}

	void begin(uint32_t baud, uint8_t config=SERIAL_8N1){
		unsigned int ubrr = (F_CPU / 8 / baud -1);	//U2Xn=1(倍速許可)の場合
		//int ubrr = (main_clock / 16 / baud -1);	//U2Xn=0(倍速不許可)の場合
		//	UBRR= 16000000/8/9600-1;	//ポーレート分周値
		//システムクロック/同期条件/ポーレート-1
		UCSR0A = 0b00000010;
		UCSR0B = 0b10011000;
		changebyte(UCSR0C, config, 0b00111111);
		UBRR0H = (uint8_t)(ubrr>>8);
		UBRR0L = (uint8_t)ubrr;
		sei();
	}
	void stop(){
		cbi(UCSR0B, RXEN0);
		cbi(UCSR0B, TXEN0);
	}
};
Uart uart(TX_BUFFER_NUM, RX_BUFFER_NUM);

ISR(USART_UDRE_vect){	//多重割り込み有効にすると、無限に割り込みが発生するので注意
	if(uart.transmitter.isEmpty()){
		uart.disableTxInterrupt();
	}
	else{
		UDR0 = uart.txOut();
	}
}
ISR(USART_RX_vect){
	uart.rxIn(UDR0);
}

#endif
//THIS PROGRAM IS VERY NICE!!!