#ifndef _ESP_8266
#define _ESP_8266
void ESP8266_Init(char*WIFI_NAME,char* WIFI_PASSWORD,char* SERVER_IP,int SERVER_PORT );
void ESP8266_SendData(char *data,int length);
void ESP8266_ReceiveData(char *data);
char* FindStringValue(char* point,char* key);
char* FindValue(char* point,char* key);
#endif
