/* main.c -- MQTT client example
*
* Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Redis nor the names of its contributors may be used
* to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//															//																		//
// ���̣�	MQTT_JX											//	ע���ڡ�esp_mqtt_proj���������޸�									//
//															//																		//
// ƽ̨��	�����µ��ӡ������������� ESP8266 V1.0			//	�٣���ӡ���ʵ��ע�͡�������˵�ˣ�˵���˶����ᣡ����				//
//															//																		//
// ���ܣ�	�٣�����MQTT��ز���							//	�ڣ��޸ġ�MQTT�������顿config.h -> device_id/mqtt_host/mqtt_pass	//
//															//																		//
//			�ڣ���MQTT����ˣ�������������(TCP)				//	�ۣ��޸ġ�MQTT_CLIENT_ID�궨�塿mqtt_config.h -> MQTT_CLIENT_ID		//
//															//																		//
//			�ۣ�����/���͡�CONNECT�����ģ�����MQTT�����	//	�ܣ��޸ġ�PROTOCOL_NAMEv31�꡿mqtt_config.h -> PROTOCOL_NAMEv311	//
//															//																		//
//			�ܣ���������"SW_LED"							//	�ݣ��޸ġ��������ĵķ��ͼ����mqtt.c ->	[mqtt_timer]����			//
//															//																		//
//			�ݣ�������"SW_LED"����"ESP8266_Online"			//	�ޣ��޸ġ�SNTP���������á�user_main.c -> [sntpfn]����				//
//															//																		//
//			�ޣ����ݽ��յ�"SW_LED"�������Ϣ������LED����	//	�ߣ�ע�͡��������á�user_main.c -> [user_init]����					//
//															//																		//
//			�ߣ�ÿ��һ���ӣ���MQTT����˷��͡�������		//	�ࣺ��ӡ�MQTT��Ϣ����LED��/��user_main.c -> [mqttDataCb]����		//
//															//																		//
//	�汾��	V1.1											//																		//
//															//																		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ͷ�ļ�
//==============================
#include "ets_sys.h"
#include "driver/uart.h"
#include "driver/gpio16.h"
#include "driver/oled.h"  		// OLED
#include "osapi.h"
#include "mqtt.h"
#include "wifi.h"
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"
#include "sntp.h"
#include "driver/dht11.h"		// DHT11ͷ�ļ�

//==============================

// ���Ͷ���
//=================================
typedef unsigned long 		u32_t;
//=================================

//								"\"humi\":\"%s\", \n"	\
//"\"temp\":\"%s\",\n"\

#define		JSON_Tree_Format	"{ \n "					\
								"\"services\": [{ \n"		\
								"\"service_id\": \"64a389ee9415fc7a573b7ef3\", \n"	\
								"\"properties\": { \n"	\
								"\"lumin\":  %d, \n"	\
								"\"dist\":  %d, \n"	\
								"\"humi\":  \"%s\", \n"	\
								"\"temp\":  \"%s\", \n"	\
								"\"LampSt\":  %d, \n"	\
								"\"devSt\":  %d \n"	\
								"}, \n"	\
								"\"event_time\": \"\" \n"	\
								"} \n"	\
								"] \n"	\
								"}\n"

//"\"properties\": { \n"	\
//"\"lumilate\":  %d, \n"	\
//"\"temp\":  \"%s\", \n"	\
//"\"humilate\":  \"%s\", \n"	\
//"\"distance\":  %d \n"	\
//"\"lampst\":  \"%s\", \n"	\
//"\"devst\":  \"%s\" \n"	\
//"\"LampSt\":  %d, \n"	\
//"\"DevSt\":  %d \n"	\




char A_JSON_Tree[256] = {0};	// ���JSON��
static int luminance = 0;
static char tempstr[10] = {0};
static char humitystr[10] = {0};
uint16_t LampSt=1,DevSt=1;  //��ʾȱʡΪ�ߵ�ƽ״̬
uint16_t iversLampSt=0,iversDevSt=0;  //�������ݵķ���
uint16_t distant =0;
uint16_t AutomodeDo =0,HumantoAuto =0;  //ȱʡ����£��˹�ģʽ����   HumantoAuto 0,û�б仯��1���˱仯

// ����JSON��
//===================================================================================================
void ICACHE_FLASH_ATTR Setup_JSON_Tree_JX(void)
{

	// ��ֵJSON������ֵJSON_Tree_Format�ַ����еĸ�ʽ�ַ���
	//--------------------------------------------------------------------------------------------
	//os_sprintf(A_JSON_Tree, JSON_Tree_Format, luminance);
	os_sprintf(A_JSON_Tree, JSON_Tree_Format,luminance,distant,humitystr,tempstr,iversLampSt,iversDevSt);

	os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");

	os_printf("%s",A_JSON_Tree);	// ���ڴ�ӡJSON��

	os_printf("\r\n-------------------- ����JSON�� -------------------\r\n");
}

/******************************************************************************
 * FunctionName : Parse_JSON_Tree_JX
 * Description  : ����json��
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
char A_JSONTree_Value[4] = {0};
void Parse_JSON_Tree_JX(char* dataBuf)
{
        os_printf("\r\n---------------------------------------\r\n");

           // JSON���ݻ�������

        char * T_Pointer_Head = NULL;        // ��ʱָ��
        char * T_Pointer_end = NULL;        // ��ʱָ��

        if(NULL == dataBuf || strlen(dataBuf)==0)
        {
        	os_printf("\r\n json string is null!\r\n");
        	return;
        }
        //������������������������������������������������������������������������������������
        T_Pointer_Head = strstr(dataBuf, "\"OnOff\"");        // ��"OnOff"��
        if(NULL == T_Pointer_Head)
        {
        	os_printf("\r\n OnOff not found!\r\n");
        	return;
        }

        memset(A_JSONTree_Value, 0, 4);

        if(T_Pointer_Head[10] == 'F')
        {
			memcpy(A_JSONTree_Value, T_Pointer_Head+9, 3);        // ��ȡOnOff��ֵ��
			A_JSONTree_Value[4] = '\0';                            // ��ֵ����'\0'��
        }
        else
        {
			memcpy(A_JSONTree_Value, T_Pointer_Head+9, 2);        // ��ȡOnOff��ֵ��
			A_JSONTree_Value[3] = '\0';                            // ��ֵ����'\0'��
        }
        os_printf("\t OnOff:%s\n",A_JSONTree_Value);

        return ;

}

// ȫ�ֱ���
//============================================================================
MQTT_Client mqttClient;			// MQTT�ͻ���_�ṹ�塾�˱����ǳ���Ҫ��

static ETSTimer sntp_timer;		// SNTP��ʱ��
//============================================================================


// SNTP��ʱ��������ȡ��ǰ����ʱ��
//============================================================================
void sntpfn()
{
    u32_t ts = 0;

    ts = sntp_get_current_timestamp();		// ��ȡ��ǰ��ƫ��ʱ��

    os_printf("current time : %s\n", sntp_get_real_time(ts));	// ��ȡ��ʵʱ��

    if (ts == 0)		// ����ʱ���ȡʧ��
    {
        os_printf("did not get a valid time from sntp server\n");
    }
    else //(ts != 0)	// ����ʱ���ȡ�ɹ�
    {
            os_timer_disarm(&sntp_timer);	// �ر�SNTP��ʱ��

            MQTT_Connect(&mqttClient);		// ��ʼMQTT����
    }
}
//============================================================================


// WIFI����״̬�ı䣺���� = wifiStatus
//============================================================================
void wifiConnectCb(uint8_t status)
{
	// �ɹ���ȡ��IP��ַ
	//---------------------------------------------------------------------
    if(status == STATION_GOT_IP)
    {
    	ip_addr_t * addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));

    	// �ڹٷ����̵Ļ����ϣ�����2�����÷�����
    	//---------------------------------------------------------------
    	sntp_setservername(0, "us.pool.ntp.org");	// ������_0��������
    	sntp_setservername(1, "ntp.sjtu.edu.cn");	// ������_1��������

    	ipaddr_aton("210.72.145.44", addr);	// ���ʮ���� => 32λ������
    	sntp_setserver(2, addr);					// ������_2��IP��ַ��
    	os_free(addr);								// �ͷ�addr

    	sntp_init();	// SNTP��ʼ��


        // ����SNTP��ʱ��[sntp_timer]
        //-----------------------------------------------------------
        os_timer_disarm(&sntp_timer);
        os_timer_setfn(&sntp_timer, (os_timer_func_t *)sntpfn, NULL);
        os_timer_arm(&sntp_timer, 1000, 1);		// 1s��ʱ
    }

    // IP��ַ��ȡʧ��
	//----------------------------------------------------------------
    else
    {
          MQTT_Disconnect(&mqttClient);	// WIFI���ӳ���TCP�Ͽ�����
    }
}
//============================================================================


// MQTT�ѳɹ����ӣ�ESP8266���͡�CONNECT���������յ���CONNACK��
//============================================================================
void mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;	// ��ȡmqttClientָ��

    INFO("MQTT: Connected\r\n");

	MQTT_Subscribe(client, "$oc/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/sys/commands/#", 0);
	MQTT_Subscribe(client, "$oc/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/sys/properties/get/#", 0);
    INFO("MQTT: Subscribed\r\n");

}
//============================================================================

// MQTT�ɹ��Ͽ�����
//============================================================================
void mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Disconnected\r\n");
}
//============================================================================

// MQTT�ɹ�������Ϣ
//============================================================================
void mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Published\r\n");
}
//============================================================================
char luminanceData[300] = "$oc/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/sys/properties/get/response/request_id=1111111";
// ������MQTT��[PUBLISH]���ݡ�����		������1������ / ����2�����ⳤ�� / ����3����Ч�غ� / ����4����Ч�غɳ��ȡ�
//===============================================================================================================
void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len+1);		// ���롾���⡿�ռ�
    char *dataBuf  = (char*)os_zalloc(data_len+1);		// ���롾��Ч�غɡ��ռ�
    char * tmpT = NULL;

    MQTT_Client* client = (MQTT_Client*)args;	// ��ȡMQTT_Clientָ��

    os_memcpy(topicBuf, topic, topic_len);	// ��������
    topicBuf[topic_len] = 0;				// �����'\0'

    os_memcpy(dataBuf, data, data_len);		// ������Ч�غ�
    dataBuf[data_len] = 0;					// �����'\0'

    INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);	// ���ڴ�ӡ�����⡿����Ч�غɡ�
    INFO("Topic_len = %d, Data_len = %d\r\n", topic_len, data_len);	// ���ڴ�ӡ�����ⳤ�ȡ�����Ч�غɳ��ȡ�

    //1.�жϷ��ص���������û�С�Light_SW��
    tmpT = strstr(dataBuf, "Light");
    if(tmpT != NULL)
    {
    	//�С�Light_SW��
    	tmpT = strstr(dataBuf, "ON");
    	if(tmpT != NULL)
    	{
    		//��setled ���� on
    		os_printf("\r\n�յ��ƶ˵�������п���\r\n");
    		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);
    		LampSt=0;
    		gpio16_output_set(LampSt);    //���Ƶ�����
    		iversLampSt=1;


    	}
    	else
    	{
    		os_printf("\r\n�յ��ƶ˵�������йص�\r\n");
    		LampSt=1;
    		iversLampSt=0;
    		gpio16_output_set(LampSt);    //�صƵ�����
    		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);
    	}
    }

    //2.�жϷ��ص���������û�С�Dev_SW��
     tmpT = strstr(dataBuf, "Motor");
     if(tmpT != NULL)
     {
     	//�С�Dev_SW��
     	tmpT = strstr(dataBuf, "ON");
     	if(tmpT != NULL)
     	{
     		//��setled ���� on
     		os_printf("\r\n�յ��ƶ˵�������п������\r\n");
     		DevSt=0;
     		iversDevSt=1;

     		GPIO_OUTPUT_SET(12,DevSt);    //�����������
     		os_printf("\r\n��ʱ�ĵ���ź�Ϊ %d\r\n",DevSt);
     	}
     	else
     	{
     		os_printf("\r\n�յ��ƶ˵�������йرյ��\r\n");
     		DevSt=1;
     		iversDevSt=0;

     		GPIO_OUTPUT_SET(12,DevSt);    //�����������

     	}
     }

     //3.�жϷ��ص���������û�С�AutoMode��   //��ϵͳ����Ϊ�Զ�����ģʽ
          tmpT = strstr(dataBuf, "AutoMode");
          if(tmpT != NULL)
          {
          	//�С�Dev_SW��
          	tmpT = strstr(dataBuf, "ON");
          	if(tmpT != NULL)
          	{
          		//��AutoMode ���� on
          		os_printf("\r\n�յ��ƶ˵�����豸�����Զ�ģʽ\r\n");
          		AutomodeDo =1;
          		HumantoAuto =1;

          	}
          	else
          	{
          		os_printf("\r\n�յ��ƶ˵�����豸�����˹�����ģʽ\r\n");
          		AutomodeDo=0;  //����һ�ֵ�TIMER�����У����֪����

          	}
          }

    //Setup_JSON_Tree_JX();
    os_sprintf(A_JSON_Tree, JSON_Tree_Format,luminance,distant,humitystr,tempstr,iversLampSt,iversDevSt);
    os_strncpy(luminanceData+strlen(luminanceData)-36, topicBuf+topic_len-36, 36);
    INFO("luminanceData: %s\r\n", luminanceData);
    MQTT_Publish(client, luminanceData, A_JSON_Tree, strlen(A_JSON_Tree), 0, 0);

    os_free(topicBuf);	// �ͷš����⡿�ռ�
    os_free(dataBuf);	// �ͷš���Ч�غɡ��ռ�
}
//===============================================================================================================

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
 *******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}


// ȫ�ֱ���
//==================================================================================
u8 F_LED = 0;				// LED״̬��־λ

os_timer_t OS_Timer_1;		// �����ʱ��
//==================================================================================
//��֤�����ַ���
void int_to_string(int num,char data[])
{

	int i=0,j=0;
	char temp[10];

	while(num)
	{
    	temp[i++]=num%10+'0';  //�����ּ��ַ�0�ͱ����Ӧ�ַ�
    	num/=10;               //��ʱ���ַ���Ϊ����
	}
    temp[i]='\0';
 	i=i-1;
 	while(i>=0)
 		data[j++]=temp[i--];   //��������ַ���תΪ����
 	data[j]='\0';               //�ַ���������־
}

// ������ʱ����
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================

//��ʱ�ϱ�����
char luminanceReport[256] = "$oc/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/sys/properties/report";
// ��ʱ�Ļص���������ʼ��1sѭ��һ�Σ��±���������ͻ�1s����һ�Σ�
//==========================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{

	uint16_t light =0;
	real32_t tempLimit =0.0,humidLimit=0.0;
	uint16_t a=0;
	uint16_t h=0,l=0;
	uint16_t readIO13=0;




		char data1[10]={0},data2[10]={0};

		//��ȡ���봫����������
		readIO13=GPIO_INPUT_GET(GPIO_ID_PIN(13));
		os_printf("\r\n13�˿ڵĳ�ʼֵ��%d \r\n",readIO13);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(14),1);	//�����������ź� 14�ڽ��з���os_printf("\r\n 12�˿ڵķ��䣺1 \r\n");
		os_delay_us(20);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(14),0);	//�����������ź�14	//os_printf("\r\n 12�˿ڵķ��䣺0 \r\n");

		readIO13=GPIO_INPUT_GET(GPIO_ID_PIN(13));

		os_printf("\r\n��һ�ε�����ֵ 13�˿ڵ���Ϊ��%d \r\n",readIO13);

		while(readIO13 == 0)
			{
				readIO13=GPIO_INPUT_GET(GPIO_ID_PIN(13));
				a++;
				if (a>10000)break;
			}  //13�ڽ������ݲɼ�
			h=system_get_rtc_time();
		while(readIO13 == 1)
			{
				readIO13=GPIO_INPUT_GET(GPIO_ID_PIN(13));
				a++;
				l=system_get_rtc_time();
				if (a>10000)break;
			}

			/*os_printf("\r\n �ڶ��ε�����ֵ 13�˿ڵ���Ϊ��%d \r\n",readIO14);*/
			distant = l-h;
			os_printf("\r\n(l-h)==%d\r\n",l-h);
			distant = (l-h)*34/26;  //������� Len_mm = (Time_Echo_us * 0.34mm/us) / 2 (mm)
			os_printf("\r\n(l-h)*34/2/26==%u\r\n",distant);


	if(DHT11_Read_Data_Complete() == 0)		// ��ȡDHT11��ʪ��ֵ
			{
				//-------------------------------------------------
				// DHT11_Data_Array[0] == ʪ��_����_���� --�޷������� 120���ڣ����Ա��һ��С��
				// DHT11_Data_Array[1] == ʪ��_С��_����  ---�޷������� 120���ڣ����Ա��һ��С��
				// DHT11_Data_Array[2] == �¶�_����_����
				// DHT11_Data_Array[3] == �¶�_С��_����
				// DHT11_Data_Array[4] == У���ֽ�
				// DHT11_Data_Array[5] == ��1:�¶�>=0����0:�¶�<0�� -----��ʾ�����͸���
				//-------------------------------------------------





				// ���������ʪ��
				//---------------------------------------------------------------------------------
				if(DHT11_Data_Array[5] == 1)			// �¶� >= 0��
				{
					os_printf("\r\nʪ�� == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
					os_printf("\r\n�¶� == %d.%d ��\r\n", DHT11_Data_Array[2],DHT11_Data_Array[3]);
				}
				else // if(DHT11_Data_Array[5] == 0)	// �¶� < 0��
				{
					os_printf("\r\nʪ�� == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
					os_printf("\r\n�¶� == -%d.%d ��\r\n",DHT11_Data_Array[2],DHT11_Data_Array[3]);
				}
//
//				// OLED��ʾ��ʪ��
//				//---------------------------------------------------------------------------------
				DHT11_NUM_Char();	// DHT11����ֵת���ַ���

				OLED_ShowString(40,0,DHT11_Data_Char[0]);	// DHT11_Data_Char[0] == ��ʪ���ַ�����
				OLED_ShowString(40,2,DHT11_Data_Char[1]);	// DHT11_Data_Char[1] == ���¶��ַ�����
			}

	os_strncpy(tempstr,DHT11_Data_Char[1],strlen(DHT11_Data_Char[1]));
	os_strncpy(humitystr,DHT11_Data_Char[0],strlen(DHT11_Data_Char[0]));
	os_printf("���ҵ�������%d\r\n",  system_adc_read());

	luminance = system_adc_read();
	light=luminance;
	// ��ȡ�豸״̬���ϱ�����

	LampSt=0;
	//gipo16_output_get();
	DevSt=0;

	int_to_string(light,data1);

	OLED_ShowString(40,4,data1);   //light
	if(light<=999)
		OLED_ShowString(64,4,"nits   ");   //light
	else
		OLED_ShowString(72,4,"nits   ");
	int_to_string(distant,data2);
	OLED_ShowString(40,6,data2);   //disctant
	if(distant<=999)
		OLED_ShowString(64,6,"mm   ");   //disctant
	else
		OLED_ShowString(72,6,"mm   ");   //disctant


	if (AutomodeDo)
	{
		// �¶ȳ���32�棬LED��
		//----------------------------------------------------
		tempLimit =DHT11_Data_Array[2]+ DHT11_Data_Array[3]/100;
		humidLimit =DHT11_Data_Array[0]+ DHT11_Data_Array[1]/100;
				if(DHT11_Data_Array[5]==1 && tempLimit>=32.4){
					//GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);		// ��ӵ������
					GPIO_OUTPUT_SET(GPIO_ID_PIN(12),0);		// ��ӵ������
					iversDevSt=1;
					DevSt=0;  //��ʾȱʡΪ�ߵ�ƽ״̬
				}
				else if (humidLimit>=86.0){
					GPIO_OUTPUT_SET(GPIO_ID_PIN(12),0);		// ��ӵ������
					iversDevSt=1;
					DevSt=0;  //��ʾȱʡΪ�ߵ�ƽ״̬
				}
				else{
					//GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);		// ��ӵ���ػ�
					GPIO_OUTPUT_SET(GPIO_ID_PIN(12),1);		// ��ӵ���ػ�
					iversDevSt=0;
					DevSt=1;  //��ʾȱʡΪ�ߵ�ƽ״̬
				}

		if (light>=600){
			gpio16_output_set(1);   //����16�˿ڣ����Ƶ���
			iversLampSt=0;
			LampSt=1;
		}
		else{
			gpio16_output_set(0);   //����16�˿ڣ����Ƶ���
			iversLampSt=1;
			LampSt=0;
			}

	}
	else              //����һ��TIMER���������У��ǲ����Ѿ�Ϊ�˹�ģʽ���˴�����ʲôҲ��������һ��Ϊ�Զ�ģʽ����תΪ�˹�ģʽ������һЩ��ʼ��
	{
		//AutomodeDo =0,HumantoAuto
		if(HumantoAuto)
		{
			gpio16_output_set(1);   //����16�˿ڣ����Ƶ���
			GPIO_OUTPUT_SET(GPIO_ID_PIN(12),1);//�ָ���ֹͣ״̬��
			iversLampSt=0;
			iversDevSt=0;
			LampSt=1;
			DevSt=1;  //��ʾȱʡΪ�ߵ�ƽ״̬

			HumantoAuto=0;   //������TIMER�����з����������ŵ�ƽ��
		}
	}


	//���������ݰ�װ��IoT����Ҫ��JSON��ʽ
	os_sprintf(A_JSON_Tree, JSON_Tree_Format,luminance,distant,humitystr,tempstr,iversLampSt,iversDevSt); /*,LampStstr,DevStstr*/
	//������ָ�����豸�ϵ���������
	MQTT_Publish(&mqttClient, luminanceReport, A_JSON_Tree, strlen(A_JSON_Tree), 0, 0);


}
//==========================================================================================

// �����ʱ����ʼ��(ms����)
//==========================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{

	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}
//==========================================================================================

// LED��ʼ��
//==========================================================================================
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	//PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U,	FUNC_GPIO2);	// GPI2��ΪIO��

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U,	FUNC_GPIO13);	// GPIO13��ΪIO��
//
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(13)); 					// IO13 = 1

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U,	FUNC_GPIO14);	// GPIO14��ΪIO��
//
	GPIO_OUTPUT_SET(GPIO_ID_PIN(14),1);						// IO14 = 1


	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
	//16�ڵĳ�ʼ������Ϊ����ʹ�ã�
	gpio16_output_conf();
	gpio16_output_set(1);    //�����ָ����ƽ

	//���õ����12�ڵĳ�ʼ������Ϊ���������ʹ��
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,	FUNC_GPIO12);	// GPIO12��ΪIO��
	//
	GPIO_OUTPUT_SET(GPIO_ID_PIN(12),1);						// IO12 = 1

}

// user_init��entry of user application, init user function here
//===================================================================================================================
void user_init(void)
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);	// ���ڲ�������Ϊ115200
    os_delay_us(60000);

	LED_Init_JX();		// LED��ʼ��
	OLED_Init();							// OLED��ʼ��---��Ҫ����������
	OLED_ShowString(0,0,"Humi:");		    // ʪ��--ֻ����ʾӢ��--д���־�������
	OLED_ShowString(0,2,"Temp:");	// �¶�
	OLED_ShowString(0,4,"Lght:");		    // ʪ��--ֻ����ʾӢ��--д���־�������
	OLED_ShowString(0,6,"Dist:");	// �¶�

	OS_Timer_1_Init_JX(1000,1);		// 1�붨ʱ���뼶(�ظ�)


    CFG_Load();	// ����/����ϵͳ������WIFI������MQTT������
	// ����WIFI��SSID[..]��PASSWORD[..]��WIFI���ӳɹ�����[wifiConnectCb]
	//--------------------------------------------------------------------------
	WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);

	Setup_JSON_Tree_JX();		// ����JSON��

    // �������Ӳ�����ֵ�������������mqtt_test_jx.mqtt.iot.gz.baidubce.com�����������Ӷ˿ڡ�1883������ȫ���͡�0��NO_TLS��
	//-------------------------------------------------------------------------------------------------------------------
	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);

	// MQTT���Ӳ�����ֵ���ͻ��˱�ʶ����..����MQTT�û�����..����MQTT��Կ��..������������ʱ����120s��������Ự��1��clean_session��
	//----------------------------------------------------------------------------------------------------------------------------
	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);

	// ������������(����ƶ�û�ж�Ӧ���������⣬��MQTT���ӻᱻ�ܾ�)
	//--------------------------------------------------------------
	MQTT_InitLWT(&mqttClient, "Will", "ESP8266_offline", 0, 0);


	// ����MQTT��غ���
	//--------------------------------------------------------------------------------------------------
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);			// ���á�MQTT�ɹ����ӡ���������һ�ֵ��÷�ʽ
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);	// ���á�MQTT�ɹ��Ͽ�����������һ�ֵ��÷�ʽ
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);			// ���á�MQTT�ɹ���������������һ�ֵ��÷�ʽ
	MQTT_OnData(&mqttClient, mqttDataCb);					// ���á�����MQTT���ݡ���������һ�ֵ��÷�ʽ


	INFO("\r\nSystem started ...\r\n");
}
//===================================================================================================================
