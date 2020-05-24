#include "./esp8266/esp8266.h"
#include <string.h>
#include <stdio.h>
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./adc/bsp_adc.h"
#include "./beep/bsp_beep.h" 

extern uint8_t issleep;
/*ESP8266需要提前完成的任务：
*1.设置为AP模式
*2.连接wifi（写入flash）
*3.连接TCP AT+CIPSTART="TCP","212.64.54.38",54321（写入flash）
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
		//设备ID不正确
		
	}
//	if((point=FindValue(data,"IsSleeping"))!=NULL){
//		//IsSleeping判断
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
		count = *point-'0';//i必须小于10
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
	//注意冒号后面一定要有空格
	//"name": "xiaoming"
	char *aim;
	if((aim = strstr(point,key))!=NULL){
		aim = aim + strlen(key) + 4;
		return aim;
	}
	return NULL;
}

char* FindValue(char* point,char* key){
	//注意冒号后面一定要有空格
	//"name": "xiaoming"
	char *aim;
	if((aim = strstr(point,key))!=NULL){
		aim = aim + strlen(key) + 3;
		return aim;
	}
	return NULL;
}
