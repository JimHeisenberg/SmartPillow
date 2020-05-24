/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc ���ԣ���ʾʱ���ʽΪ: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./adc/bsp_adc.h"
#include "./esp8266/esp8266.h"
#include "./beep/bsp_beep.h"  
#include "./tim/bsp_GeneralTim.h"
char aRxBuffer[400]={0x00};
uint8_t issleep=0;
volatile uint16_t RxCounter=0;
volatile uint8_t ReceiveState=0;
volatile uint32_t time = 0; // ms ��ʱ���� 
volatile uint8_t SendData;
extern __IO uint32_t TimeDisplay ;
extern __IO uint32_t TimeAlarm ;
extern __IO uint16_t ADC_ConvertedValue;

// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};
/*ʱ��ṹ�壬����ʱ��2000-01-01 00:00:08*/
//struct rtc_time clocktime=
//{
//0,40,17,5,5,2020,0
//};
struct rtc_time clocktime=
{
15,5,0,1,1,2000,0
};

uint32_t ClockTable[3]={0xffffffff,0xffffffff,0xffffffff};//�����������
uint32_t *CurrentClock;
//��*��ע�����
//��bsp_rtc.h�ļ��У�

//1.�����ú�USE_LCD_DISPLAY�����Ƿ�ʹ��LCD��ʾ
//2.�����ú�RTC_CLOCK_SOURCE_LSI��RTC_CLOCK_SOURCE_LSE����ʹ��LSE������LSI����

//3.STM32��LSE����Ҫ��ǳ��ϸ�ͬ���ĵ�·������������Ʒʱ����Щ��������⡣
//  ��ʵ����Ĭ��ʹ��LSI����
//  
//4.��������ϣ��RTC������Դ�������Ȼ���У���Ҫ��������ĵ�ز۰�װť�۵�أ�
//  �������Ҹĳ�ʹ���ⲿ����ģʽRTC_CLOCK_SOURCE_LSE
//  ť�۵���ͺţ�CR1220
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */

int main()
{			
		uint32_t clock_timestamp;
		uint32_t current_timestamp;
		//struct rtc_time set_time;
		int DevideID = 99;
		float Pressure = 0;
		uint8_t head = 0,headcount = 0;
		char temp[25];//���ڴ���ַ���ƴ�ӵ���ʱ����
	
	  USART_Config();
		USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE|USART_IT_IDLE,DISABLE);//�ȹر��жϣ���ֹ8266�ĳ�ʼ����Ϣ���»���	
		ADCx_Init();
		BEEP_GPIO_Config();
		GENERAL_TIM_Init();
//		printf("Init finish\n");
		BEEP(OFF);
		
		/* ����RTC���ж����ȼ� */
		RTC_NVIC_Config();
		RTC_CheckAndConfig(&systmtime);
		/*�������ӼĴ���*/
		//ע�⣺�������ӼĴ���֮��Կɱ�̼�����������Ҫͬ��,��������һ���������������
		clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;//����ʱ��ȱ�׼ʱ����˸�Сʱ
		RTC_SetAlarm(clock_timestamp);
		RTC_WaitForLastTask();

//		if(1){
//		/*ʹ�ô��ڽ������õ�ʱ�䣬��������ʱע��ĩβҪ�ӻس�*/
//			Time_Regulate_Get(&set_time);
//		/*�ý��յ���ʱ������RTC*/
//			Time_Adjust(&set_time);			
//		// 	�򱸷ݼĴ���д���־
//			BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
//		}
		//BEEP(ON);
		printf("+++");
		time = 0;
		while(time<1000);
		printf("AT+RST\r\n");
		time = 0;
		while(time<5000);//�ȴ���ʼ�����
		//time = 3000;
		printf("+++");
		time = 0;
		while(time<2000);
		printf("AT+CLOSE\r\n");
		time = 0;
		while(time<1000);
		printf("AT+CIPSTART=\"TCP\",\"175.24.76.61\",54321\r\n");
		//printf("AT+CIPSTART=\"TCP\",\"192.168.3.13\",8082\r\n");
		time = 0;
		while(time<2000);
		printf("AT+CIPMODE=1\r\n");
		time = 0;
		while(time<500);
		printf("AT+CIPSEND\r\n");
		time = 0;
		while(time<500);
		//BEEP(OFF);
		USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE|USART_IT_IDLE,ENABLE);//���´��ж�
		time = 300000;
		while (1)
	  {
//			while(1){
//				for(int i=0;i<10;i++){
//					printf("{\"DeviceID\": 12, \"Pressure\": %f}",(float)i/10);
//					time = 0;
//					while(time<3000);
//				}
//			}
	    /* ÿ��1s ����һ��ʱ��*/
	    if (TimeDisplay == 1)
	    {
				/* ��ǰʱ�� */							
				current_timestamp = RTC_GetCounter();	
				//Time_Display(current_timestamp,&systmtime);					
	      TimeDisplay = 0;
				if((float)ADC_ConvertedValue/4100>0.1){
					if(headcount<6)headcount++;
				}else{
					if(headcount>0)headcount--;
				}
				if(headcount>2){
					if(head==0){
						char JsonToSend[100];
						time = 0;
						memset((void*)JsonToSend,0,100);
						strcat(JsonToSend,"{\"DeviceID\": ");
						sprintf(temp,"%d",DevideID);
						strcat(JsonToSend,temp);
						strcat(JsonToSend,", \"Pressure\": ");
						Pressure = (float)ADC_ConvertedValue/4100;
						sprintf(temp,"%.1f",Pressure);
						strcat(JsonToSend,temp);
						strcat(JsonToSend,"}");
						printf("%s",JsonToSend);//͸��
					}
					head=1;
				}
				else{
					if(head==1){
						char JsonToSend[100];
						time = 0;
						memset((void*)JsonToSend,0,100);
						strcat(JsonToSend,"{\"DeviceID\": ");
						sprintf(temp,"%d",DevideID);
						strcat(JsonToSend,temp);
						strcat(JsonToSend,", \"Pressure\": ");
						Pressure = (float)ADC_ConvertedValue/4100;
						sprintf(temp,"%.1f",Pressure);
						strcat(JsonToSend,temp);
						strcat(JsonToSend,"}");
						printf("%s",JsonToSend);//͸��
					}
					head=0;
					BEEP(OFF);
				}
//				printf("head:%d\n",head);
//				printf("headcount:%d\n",headcount);
//				printf("AD:%f\n",(float)ADC_ConvertedValue/4100);
	    }
			//����
			//if( TimeAlarm == 1){
			if( TimeAlarm == 1&&head==1){
			//if( TimeAlarm == 1||issleep == 1||head==1){
					TimeAlarm = 0;
					BEEP(ON);
					//while(head==1);
					//BEEP(OFF);
			}
			if(	ReceiveState ==1 ){
				ReceiveState = 0;
				ESP8266_ReceiveData(aRxBuffer);
			}
			
			//300*1000ms,5����һ�������������
			if(time > 300000){
				char JsonToSend[100];
				time = 0;
				memset((void*)JsonToSend,0,100);
				strcat(JsonToSend,"{\"DeviceID\": ");
				sprintf(temp,"%d",DevideID);
				strcat(JsonToSend,temp);
				strcat(JsonToSend,", \"Pressure\": ");
				Pressure = (float)ADC_ConvertedValue/4100;
				sprintf(temp,"%.1f",Pressure);
				strcat(JsonToSend,temp);
				strcat(JsonToSend,"}");
				printf("%s",JsonToSend);//͸��
			}
			//���°�����ͨ�������޸�ʱ��
			//if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
//			if(1)
//			{
//				//struct rtc_time set_time;

//				/*ʹ�ô��ڽ������õ�ʱ�䣬��������ʱע��ĩβҪ�ӻس�*/
//				
//				Time_Regulate_Get(&set_time);
//				/*�ý��յ���ʱ������RTC*/
//				Time_Adjust(&set_time);
//				
//				//�򱸷ݼĴ���д���־
//				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
//			} 	
	  }
}

/***********************************END OF FILE*********************************/

