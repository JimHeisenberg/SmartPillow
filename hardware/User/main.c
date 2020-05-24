/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc 测试，显示时间格式为: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
volatile uint32_t time = 0; // ms 计时变量 
volatile uint8_t SendData;
extern __IO uint32_t TimeDisplay ;
extern __IO uint32_t TimeAlarm ;
extern __IO uint16_t ADC_ConvertedValue;

// N = 2^32/365/24/60/60 = 136 年

/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};
/*时间结构体，闹钟时间2000-01-01 00:00:08*/
//struct rtc_time clocktime=
//{
//0,40,17,5,5,2020,0
//};
struct rtc_time clocktime=
{
15,5,0,1,1,2000,0
};

uint32_t ClockTable[3]={0xffffffff,0xffffffff,0xffffffff};//最多三个闹钟
uint32_t *CurrentClock;
//【*】注意事项：
//在bsp_rtc.h文件中：

//1.可设置宏USE_LCD_DISPLAY控制是否使用LCD显示
//2.可设置宏RTC_CLOCK_SOURCE_LSI和RTC_CLOCK_SOURCE_LSE控制使用LSE晶振还是LSI晶振

//3.STM32的LSE晶振要求非常严格，同样的电路、板子批量产品时总有些会出现问题。
//  本实验中默认使用LSI晶振。
//  
//4.！！！若希望RTC在主电源掉电后仍然运行，需要给开发板的电池槽安装钮扣电池，
//  ！！！且改成使用外部晶振模式RTC_CLOCK_SOURCE_LSE
//  钮扣电池型号：CR1220
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */

int main()
{			
		uint32_t clock_timestamp;
		uint32_t current_timestamp;
		//struct rtc_time set_time;
		int DevideID = 99;
		float Pressure = 0;
		uint8_t head = 0,headcount = 0;
		char temp[25];//用于存放字符串拼接的临时数据
	
	  USART_Config();
		USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE|USART_IT_IDLE,DISABLE);//先关闭中断，防止8266的初始化信息导致混乱	
		ADCx_Init();
		BEEP_GPIO_Config();
		GENERAL_TIM_Init();
//		printf("Init finish\n");
		BEEP(OFF);
		
		/* 配置RTC秒中断优先级 */
		RTC_NVIC_Config();
		RTC_CheckAndConfig(&systmtime);
		/*设置闹钟寄存器*/
		//注意：设置闹钟寄存器之后对可编程计数器更改需要同步,可以重设一次闹钟来解决问题
		clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;//北京时间比标准时间晚八个小时
		RTC_SetAlarm(clock_timestamp);
		RTC_WaitForLastTask();

//		if(1){
//		/*使用串口接收设置的时间，输入数字时注意末尾要加回车*/
//			Time_Regulate_Get(&set_time);
//		/*用接收到的时间设置RTC*/
//			Time_Adjust(&set_time);			
//		// 	向备份寄存器写入标志
//			BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
//		}
		//BEEP(ON);
		printf("+++");
		time = 0;
		while(time<1000);
		printf("AT+RST\r\n");
		time = 0;
		while(time<5000);//等待初始化完毕
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
		USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE|USART_IT_IDLE,ENABLE);//重新打开中断
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
	    /* 每过1s 更新一次时间*/
	    if (TimeDisplay == 1)
	    {
				/* 当前时间 */							
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
						printf("%s",JsonToSend);//透传
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
						printf("%s",JsonToSend);//透传
					}
					head=0;
					BEEP(OFF);
				}
//				printf("head:%d\n",head);
//				printf("headcount:%d\n",headcount);
//				printf("AD:%f\n",(float)ADC_ConvertedValue/4100);
	    }
			//响铃
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
			
			//300*1000ms,5分钟一次向服务器发送
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
				printf("%s",JsonToSend);//透传
			}
			//按下按键，通过串口修改时间
			//if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
//			if(1)
//			{
//				//struct rtc_time set_time;

//				/*使用串口接收设置的时间，输入数字时注意末尾要加回车*/
//				
//				Time_Regulate_Get(&set_time);
//				/*用接收到的时间设置RTC*/
//				Time_Adjust(&set_time);
//				
//				//向备份寄存器写入标志
//				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
//			} 	
	  }
}

/***********************************END OF FILE*********************************/

