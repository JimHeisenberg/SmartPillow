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
volatile uint32_t time = 0; // ms ��ʱ����
volatile uint8_t SendData;
extern __IO uint32_t TimeDisplay;
extern __IO uint32_t TimeAlarm;
extern __IO uint16_t ADC_ConvertedValue;

char *WIFI_NAME = "JIM-MIX2S";
char *WIFI_PASSWORD = "JimHotspot";
char *SERVER_IP = "175.24.76.61";
int SERVER_PORT = 54321;
float PRESSURE_THRESHOLD = 0.1;

// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime = {0, 0, 0, 1, 1, 2000, 0};
/*ʱ��ṹ�壬����ʱ��2000-01-01 00:00:08*/
//struct rtc_time clocktime=
//{
//0,40,17,5,5,2020,0
//};
struct rtc_time clocktime = {15, 5, 0, 1, 1, 2000, 0};

uint32_t ClockTable[3] = {0xffffffff, 0xffffffff, 0xffffffff}; //�����������
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
    { //һ����ʮ�����ݣ�һ��ռ200���ֽ�
        Pressure = *data++;
        if (Pressure > 0.01)
        {
            sprintf(temp, ",%.3f", Pressure);
            strcat(JsonToSend, temp);
        }
    }
    strcat(JsonToSend, temp);
    strcat(JsonToSend, "]}");
    printf("%s", JsonToSend); //͸��
}

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
    //uint32_t clock_timestamp;
    //struct rtc_time set_time;
    float NewAdcData;
    uint8_t head = 0, headcount = 0;
    float databuf[40];
    uint8_t datacount = 0;

    // hardware init start

    GENERAL_TIM_Init(); // ʱ�ӳ�ʼ��
    /* ����RTC���ж����ȼ� */
    RTC_NVIC_Config();
    RTC_CheckAndConfig(&systmtime);
    /*�������ӼĴ���*/
    //ע�⣺�������ӼĴ���֮��Կɱ�̼�����������Ҫͬ��,��������һ���������������
    //����ʱ��ȱ�׼ʱ����˸�Сʱ
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
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE | USART_IT_IDLE, DISABLE); //�ȹر��жϣ���ֹ8266�ĳ�ʼ����Ϣ���»���
    ESP8266_Init(WIFI_NAME, WIFI_PASSWORD, SERVER_IP, SERVER_PORT);
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE | USART_IT_IDLE, ENABLE); //���´��ж�

    // hardware init done

    time = 0;
    while (1)
    {
        if (time > 500) //0.5sһ�δ���
        {
            time = 0;
            NewAdcData = (float)ADC_ConvertedValue / 4100;
            databuf[datacount++] = NewAdcData; //��������
					
						// �����˲��㷨 start
            headcount = headcount >> 1;        //headcount��һ��8λ������Ĵ���
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
						// �����˲��㷨 end
        }
        //����
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
        //20*1000ms,20sһ�������������
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
