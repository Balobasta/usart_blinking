#include <stm32f10x_conf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define rb_size 9
uint8_t rb[rb_size],data;
int led_state=0,rb_index=0,wb_index=0,f=0;

uint16_t get_char(void);

void Init (void){
	GPIO_InitTypeDef gp;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_StructInit(&gp);
	gp.GPIO_Pin = GPIO_Pin_1;
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gp);


	//TX USART
	gp.GPIO_Mode = GPIO_Mode_AF_PP;
	gp.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA,&gp);

	//RX USART
	gp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gp.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&gp);

	USART_InitTypeDef us;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	us.USART_BaudRate = 9600;
	us.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	us.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	us.USART_Parity = USART_Parity_No;
	us.USART_StopBits = USART_StopBits_1;
	us.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&us);

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);

	NVIC_EnableIRQ(USART1_IRQn);

}

void send_char(uint16_t x){
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, x);
}

  void send_string(char *str){
	  int i;
	  for(i=0; i<=strlen(str);i++)
		  send_char(str[i]);
  }

  void USART1_IRQHandler(void) {
  	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
  		      	if ((USART1->SR & (USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_ORE)) == 0) {
  						rb[wb_index]=(uint8_t)(USART_ReceiveData(USART1)& 0xFF);
  						wb_index=(wb_index+1)%rb_size;
  					}
  					else USART_ReceiveData(USART1);
  		  }
  	}

  uint16_t get_char(void)
  {
  	uint16_t data;
  	while (rb_index==wb_index);
  	data = rb[rb_index];
  	rb_index = (rb_index+1)%rb_size;
  	send_char(data);
  	return data;
  }


  int main(){
	  uint16_t d;
	  int k=0;
	   Init();
	   send_string("\ron to on, off to off V konze komandi nazmite ENTER\n\r");
	   d=get_char();
	   while(1){

		   if (d=='o'){
			   d=get_char();
			   if(d=='n') led_state =0;
			   else if (d=='f'){
				   d=get_char();
				   if (d=='f') led_state=1;
				   else {send_string("\n !!!ERROR!!!\n\r");

				   }
			   }else {send_string("\n !!!ERROR!!!\n\r");
			   }

		   }else {
			   send_string("\n !!!ERROR!!!\n\r");
		  }
		  /* if (!led_state){
			   if (get_char()==13) send_string("\n");
		   }else if(led_state==1) if (get_char()==13) send_string("\n");
*/
		   GPIO_WriteBit(GPIOA,GPIO_Pin_1, led_state ? Bit_SET :Bit_RESET);
		   d=get_char();
		   if (d==13) k=1;
		   if (k)
         {
			  send_string("\n");
        	  k=0;
        	  d=get_char();
          }
	   }
  }









