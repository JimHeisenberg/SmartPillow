#include "./esp8266/esp8266.h"
#include <string.h>
#include <stdio.h>
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./adc/bsp_adc.h"
#include "./beep/bsp_beep.h" 

extern uint8_t issleep;
/*ESP8266��Ҫ��ǰ��ɵ�����
*1.����ΪAPģʽ
*2.����wifi��д��flash��
*3.����TCP AT+CIPSTART="TCP","212.64.54.38",54321��д��flash��
*
*/
void ESP8266_Init(){
	


}

void ESP8266_SendData(char *data,int length){
	printf("AT+CIPSEND=%d",length);
	printf("%s",data);
}

void ESP8266_ReceiveData(char *data){
	char* point;
	char TheDate[30];
	char TheClock[30];
	uint8_t count;
	struct rtc_time *newclock;
	uint32_t clock_timestamp;
//	printf("receive data:\n%s\n\n",data);
	if(0){
		//�豸ID����ȷ
		
	}
//	if((point=FindValue(data,"IsSleeping"))!=NULL){
//		//IsSleeping�ж�
//		if(*point=='0')issleep=0;
//			else issleep=0;
//	}
	
	if((point=FindStringValue(data,"DateTime"))!=NULL){
		int i = 0;
		while(*point!='\"')TheDate[i++] = *point++;
		TheDate[i]=0;
	}
	//printf("TheData:%s\n",TheDate);
	SetTimeByXML(TheDate);
	if((point=FindValue(data,"PeriodNumber"))!=NULL){
		count = *point-'0';//i����С��10
		for(int i=0;i<count;i++){
			char WakeupTime[12]={0};
			sprintf(WakeupTime,"WakeupTime%d",i);
			if((point=FindStringValue(data,WakeupTime))!=NULL){
				int j = 0;
				while(j!='\"')TheClock[j++] = *point++;
				TheClock[j]=0;
			}
			SetClockTable(TheClock,i);
		}
		ClockSchedul();
	}
	return;
}

char* FindStringValue(char* point,char* key){
	//ע��ð�ź���һ��Ҫ�пո�
	//"name": "xiaoming"
	char *aim;
	if((aim = strstr(point,key))!=NULL){
		aim = aim + strlen(key) + 4;
		return aim;
	}
	return NULL;
}

char* FindValue(char* point,char* key){
	//ע��ð�ź���һ��Ҫ�пո�
	//"name": "xiaoming"
	char *aim;
	if((aim = strstr(point,key))!=NULL){
		aim = aim + strlen(key) + 3;
		return aim;
	}
	return NULL;
}
