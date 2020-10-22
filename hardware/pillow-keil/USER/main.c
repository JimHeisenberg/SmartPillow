#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./adc/bsp_adc.h"
#include "./esp8266/esp8266.h"
#include "./beep/bsp_beep.h"
#include "./tim/bsp_GeneralTim.h"
char aRxBuffer[400] = {0x00};
uint8_t issleep = 0;
int DevideID = 2;
uint32_t version = 0;
volatile uint16_t RxCounter = 0;
volatile uint8_t ReceiveState = 0;
volatile uint32_t time = 0; // ms 计时变量
volatile uint8_t SendData;
extern __IO uint32_t TimeDisplay;
extern __IO uint32_t TimeAlarm;
extern __IO uint16_t ADC_ConvertedValue;

char *WIFI_NAME = "JIM-MIX2S";
char *WIFI_PASSWORD = "JimHotspot";
char *SERVER_IP = "175.24.76.61";
int SERVER_PORT = 54321;
float PRESSURE_THRESHOLD = 0.1;

// N = 2^32/365/24/60/60 = 136 年

/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime = {0, 0, 0, 1, 1, 2000, 0};
/*时间结构体，闹钟时间2000-01-01 00:00:08*/
//struct rtc_time clocktime=
//{
//0,40,17,5,5,2020,0
//};
struct rtc_time clocktime = {15, 5, 0, 1, 1, 2000, 0};

uint32_t ClockTable[3] = {0xffffffff, 0xffffffff, 0xffffffff}; //最多三个闹钟
uint32_t *CurrentClock;

void SendJson(float *data)
{
    char JsonToSend[300];
    char temp[20];
    float Pressure;
    time = 0;
    memset((void *)JsonToSend, 0, 100);
    strcat(JsonToSend, "{\"DeviceID\": ");
    sprintf(temp, "%d", DevideID);
    strcat(JsonToSend, temp);
    strcat(JsonToSend, ", \"Pressure\": [");

    Pressure = *data++;
    sprintf(temp, "%.3f", Pressure);
    strcat(JsonToSend, temp);

    for (int i = 1; i < 40; i++)
    { //一包四十个数据，一共占200个字节
        Pressure = *data++;
        if (Pressure > 0.01)
        {
            sprintf(temp, ",%.3f", Pressure);
            strcat(JsonToSend, temp);
        }
    }
    strcat(JsonToSend, temp);
    strcat(JsonToSend, "]}");
    printf("%s", JsonToSend); //透传
}

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
    //uint32_t clock_timestamp;
    //struct rtc_time set_time;
    float NewAdcData;
    uint8_t head = 0, headcount = 0;
    float databuf[40];
    uint8_t datacount = 0;

    // hardware init start

    GENERAL_TIM_Init(); // 时钟初始化
    /* 配置RTC秒中断优先级 */
    RTC_NVIC_Config();
    RTC_CheckAndConfig(&systmtime);
    /*设置闹钟寄存器*/
    //注意：设置闹钟寄存器之后对可编程计数器更改需要同步,可以重设一次闹钟来解决问题
    //北京时间比标准时间晚八个小时
	/*
    clock_timestamp = mktimev(&clocktime) - TIME_ZOOM;
    RTC_SetAlarm(clock_timestamp);
    RTC_WaitForLastTask();
*/
    // pressure infomation is from adc
    ADCx_Init();

    // BEEP init
    BEEP_GPIO_Config();
    BEEP(OFF);

    // USART for ESP8266 init
    USART_Config();
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE | USART_IT_IDLE, DISABLE); //先关闭中断，防止8266的初始化信息导致混乱
    ESP8266_Init(WIFI_NAME, WIFI_PASSWORD, SERVER_IP, SERVER_PORT);
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE | USART_IT_IDLE, ENABLE); //重新打开中断

    // hardware init done

    time = 0;
    while (1)
    {
        if (time > 500) //0.5s一次处理
        {
            time = 0;
            NewAdcData = (float)ADC_ConvertedValue / 4100;
            databuf[datacount++] = NewAdcData; //更新数据
					
						// 本地滤波算法 start
            headcount = headcount >> 1;        //headcount是一个8位的软件寄存器
            if (NewAdcData > PRESSURE_THRESHOLD)
                headcount |= 0x80;
            if (headcount > 0xE0 && head == 0)
						{//0b11100000
							SendJson(databuf);
							for (int i = 0; i < 40; i++)
								databuf[i] = 0;
							datacount = 0;
							head = 1;
							}
            else if (headcount < 0x10 && head == 1)
						{//0b00010000
							SendJson(databuf);
							for (int i = 0; i < 40; i++)
								databuf[i] = 0;
							datacount = 0;
							head = 0;
							BEEP(OFF);
            }
						// 本地滤波算法 end
        }
        //响铃
        if (TimeAlarm == 1 )
        {
            TimeAlarm = 0;
						if (head == 1)
						{
							BEEP(ON);
						}
					  ClockSchedule();
        }
        if (ReceiveState == 1)
        {
            ReceiveState = 0;
            ESP8266_ReceiveData(aRxBuffer);
        }
        //20*1000ms,20s一次向服务器发送
        if (datacount == 40)
        {
            SendJson(databuf);
            for (int i = 0; i < 40; i++)
                databuf[i] = 0;
            datacount = 0;
        }
    }
}

/***********************************END OF FILE*********************************/
