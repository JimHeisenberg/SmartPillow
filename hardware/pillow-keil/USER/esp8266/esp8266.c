#include "./esp8266/esp8266.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./adc/bsp_adc.h"
#include "./beep/bsp_beep.h" 

extern uint32_t time; // ms 计时变量 
extern uint8_t issleep;
extern uint32_t version;
/*
*/
void ESP8266_Init(char*WIFI_NAME,char* WIFI_PASSWORD,char* SERVER_IP,int SERVER_PORT ){
    
        printf("+++");
        for(time=0;time<500;);
        printf("AT+RESTORE\r\n");
        for(time=0;time<2000;);
        printf("AT+CWMODE=1\r\n");
        for(time=0;time<500;);
        // WiFi name and password
        printf("AT+CWJAP=\"%s\",\"%s\"\r\n",WIFI_NAME,WIFI_PASSWORD);
        for(time=0;time<9000;);
        // 175.24.76.61 JimHeisenberg.xyz
      //printf("AT+CIPSTART=\"TCP\",\"192.168.15.81\",54321\r\n");
        printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", SERVER_IP, SERVER_PORT );
        for(time=0;time<1000;);
        printf("AT+CIPMODE=1\r\n");
        for(time=0;time<500;);
        printf("AT+CIPSEND\r\n");
        for(time=0;time<500;);
        
}

void ESP8266_SendData(char *data,int length){
    printf("AT+CIPSEND=%d",length);
    printf("%s",data);
}

void ESP8266_ReceiveData(char *data){
    char* point;
    char TheDate[30];
    char TheClock[30];
    char hashtemp[12]={0};
    uint32_t hash;
    uint8_t count;

		
    if((point=FindValue(data,"Hash"))!=NULL){
        for(int i=0;'0'<*point&&*point<'9';i++){
            hashtemp[i]=*point++;//考虑到int32的位数，暂时不做溢出出错处理
        }
				if((point=FindStringValue(data,"DateTime"))!=NULL){
						int i = 0;
						while(*point!='"')TheDate[i++] = *point++;
						TheDate[i]=0;
						SetTimeByXML(TheDate);
				}
        hash = atoi(hashtemp);
        if(version!=hash){
            version = hash;
            ClearClockTable();
            if((point=FindValue(data,"PeriodNumber"))!=NULL){
                count = *point-'0';//i必须小于10
                for(int i=0;i<count&&i<3;i++){
                    char WakeupTime[12]={0};
                    sprintf(WakeupTime,"WakeupTime%d",i);
                    if((point=FindStringValue(data,WakeupTime))!=NULL){
                        int j = 0;
                        while(j!='\"')TheClock[j++] = *point++;
                        TheClock[j]=0;
                    }
                    SetClockTable(TheClock,i);
                }
                //ClockSchedule();
            }
        }
    }
    return;
}

char* FindStringValue(char* point,char* key){
    //注意冒号后面一定要有空格
    //"name": "xiaoming"
    char *aim;
    if((aim = strstr(point,key))!=NULL){
        aim = aim + strlen(key) + 4;// <": ">的长度为4
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
