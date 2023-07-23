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
// 工程：	MQTT_JX											//	注：在《esp_mqtt_proj》例程上修改									//
//															//																		//
// 平台：	【技新电子】物联网开发板 ESP8266 V1.0			//	①：添加【详实的注释】唉，不说了，说多了都是泪！！！				//
//															//																		//
// 功能：	①：设置MQTT相关参数							//	②：修改【MQTT参数数组】config.h -> device_id/mqtt_host/mqtt_pass	//
//															//																		//
//			②：与MQTT服务端，建立网络连接(TCP)				//	③：修改【MQTT_CLIENT_ID宏定义】mqtt_config.h -> MQTT_CLIENT_ID		//
//															//																		//
//			③：配置/发送【CONNECT】报文，连接MQTT服务端	//	④：修改【PROTOCOL_NAMEv31宏】mqtt_config.h -> PROTOCOL_NAMEv311	//
//															//																		//
//			④：订阅主题"SW_LED"							//	⑤：修改【心跳报文的发送间隔】mqtt.c ->	[mqtt_timer]函数			//
//															//																		//
//			⑤：向主题"SW_LED"发布"ESP8266_Online"			//	⑥：修改【SNTP服务器设置】user_main.c -> [sntpfn]函数				//
//															//																		//
//			⑥：根据接收到"SW_LED"主题的消息，控制LED亮灭	//	⑦：注释【遗嘱设置】user_main.c -> [user_init]函数					//
//															//																		//
//			⑦：每隔一分钟，向MQTT服务端发送【心跳】		//	⑧：添加【MQTT消息控制LED亮/灭】user_main.c -> [mqttDataCb]函数		//
//															//																		//
//	版本：	V1.1											//																		//
//															//																		//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 头文件
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
#include "driver/dht11.h"		// DHT11头文件

//==============================

// 类型定义
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




char A_JSON_Tree[256] = {0};	// 存放JSON树
static int luminance = 0;
static char tempstr[10] = {0};
static char humitystr[10] = {0};
uint16_t LampSt=1,DevSt=1;  //表示缺省为高电平状态
uint16_t iversLampSt=0,iversDevSt=0;  //上面数据的反向
uint16_t distant =0;
uint16_t AutomodeDo =0,HumantoAuto =0;  //缺省情况下，人工模式开启   HumantoAuto 0,没有变化，1有了变化

// 创建JSON树
//===================================================================================================
void ICACHE_FLASH_ATTR Setup_JSON_Tree_JX(void)
{

	// 赋值JSON树【赋值JSON_Tree_Format字符串中的格式字符】
	//--------------------------------------------------------------------------------------------
	//os_sprintf(A_JSON_Tree, JSON_Tree_Format, luminance);
	os_sprintf(A_JSON_Tree, JSON_Tree_Format,luminance,distant,humitystr,tempstr,iversLampSt,iversDevSt);

	os_printf("\r\n-------------------- 创建JSON树 -------------------\r\n");

	os_printf("%s",A_JSON_Tree);	// 串口打印JSON树

	os_printf("\r\n-------------------- 创建JSON树 -------------------\r\n");
}

/******************************************************************************
 * FunctionName : Parse_JSON_Tree_JX
 * Description  : 解析json树
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
char A_JSONTree_Value[4] = {0};
void Parse_JSON_Tree_JX(char* dataBuf)
{
        os_printf("\r\n---------------------------------------\r\n");

           // JSON数据缓存数组

        char * T_Pointer_Head = NULL;        // 临时指针
        char * T_Pointer_end = NULL;        // 临时指针

        if(NULL == dataBuf || strlen(dataBuf)==0)
        {
        	os_printf("\r\n json string is null!\r\n");
        	return;
        }
        //………………………………………………………………………………………………………………
        T_Pointer_Head = strstr(dataBuf, "\"OnOff\"");        // 【"OnOff"】
        if(NULL == T_Pointer_Head)
        {
        	os_printf("\r\n OnOff not found!\r\n");
        	return;
        }

        memset(A_JSONTree_Value, 0, 4);

        if(T_Pointer_Head[10] == 'F')
        {
			memcpy(A_JSONTree_Value, T_Pointer_Head+9, 3);        // 获取OnOff【值】
			A_JSONTree_Value[4] = '\0';                            // 【值后添'\0'】
        }
        else
        {
			memcpy(A_JSONTree_Value, T_Pointer_Head+9, 2);        // 获取OnOff【值】
			A_JSONTree_Value[3] = '\0';                            // 【值后添'\0'】
        }
        os_printf("\t OnOff:%s\n",A_JSONTree_Value);

        return ;

}

// 全局变量
//============================================================================
MQTT_Client mqttClient;			// MQTT客户端_结构体【此变量非常重要】

static ETSTimer sntp_timer;		// SNTP定时器
//============================================================================


// SNTP定时函数：获取当前网络时间
//============================================================================
void sntpfn()
{
    u32_t ts = 0;

    ts = sntp_get_current_timestamp();		// 获取当前的偏移时间

    os_printf("current time : %s\n", sntp_get_real_time(ts));	// 获取真实时间

    if (ts == 0)		// 网络时间获取失败
    {
        os_printf("did not get a valid time from sntp server\n");
    }
    else //(ts != 0)	// 网络时间获取成功
    {
            os_timer_disarm(&sntp_timer);	// 关闭SNTP定时器

            MQTT_Connect(&mqttClient);		// 开始MQTT连接
    }
}
//============================================================================


// WIFI连接状态改变：参数 = wifiStatus
//============================================================================
void wifiConnectCb(uint8_t status)
{
	// 成功获取到IP地址
	//---------------------------------------------------------------------
    if(status == STATION_GOT_IP)
    {
    	ip_addr_t * addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));

    	// 在官方例程的基础上，增加2个备用服务器
    	//---------------------------------------------------------------
    	sntp_setservername(0, "us.pool.ntp.org");	// 服务器_0【域名】
    	sntp_setservername(1, "ntp.sjtu.edu.cn");	// 服务器_1【域名】

    	ipaddr_aton("210.72.145.44", addr);	// 点分十进制 => 32位二进制
    	sntp_setserver(2, addr);					// 服务器_2【IP地址】
    	os_free(addr);								// 释放addr

    	sntp_init();	// SNTP初始化


        // 设置SNTP定时器[sntp_timer]
        //-----------------------------------------------------------
        os_timer_disarm(&sntp_timer);
        os_timer_setfn(&sntp_timer, (os_timer_func_t *)sntpfn, NULL);
        os_timer_arm(&sntp_timer, 1000, 1);		// 1s定时
    }

    // IP地址获取失败
	//----------------------------------------------------------------
    else
    {
          MQTT_Disconnect(&mqttClient);	// WIFI连接出错，TCP断开连接
    }
}
//============================================================================


// MQTT已成功连接：ESP8266发送【CONNECT】，并接收到【CONNACK】
//============================================================================
void mqttConnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;	// 获取mqttClient指针

    INFO("MQTT: Connected\r\n");

	MQTT_Subscribe(client, "$oc/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/sys/commands/#", 0);
	MQTT_Subscribe(client, "$oc/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/sys/properties/get/#", 0);
    INFO("MQTT: Subscribed\r\n");

}
//============================================================================

// MQTT成功断开连接
//============================================================================
void mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Disconnected\r\n");
}
//============================================================================

// MQTT成功发布消息
//============================================================================
void mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    INFO("MQTT: Published\r\n");
}
//============================================================================
char luminanceData[300] = "$oc/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/sys/properties/get/response/request_id=1111111";
// 【接收MQTT的[PUBLISH]数据】函数		【参数1：主题 / 参数2：主题长度 / 参数3：有效载荷 / 参数4：有效载荷长度】
//===============================================================================================================
void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    char *topicBuf = (char*)os_zalloc(topic_len+1);		// 申请【主题】空间
    char *dataBuf  = (char*)os_zalloc(data_len+1);		// 申请【有效载荷】空间
    char * tmpT = NULL;

    MQTT_Client* client = (MQTT_Client*)args;	// 获取MQTT_Client指针

    os_memcpy(topicBuf, topic, topic_len);	// 缓存主题
    topicBuf[topic_len] = 0;				// 最后添'\0'

    os_memcpy(dataBuf, data, data_len);		// 缓存有效载荷
    dataBuf[data_len] = 0;					// 最后添'\0'

    INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);	// 串口打印【主题】【有效载荷】
    INFO("Topic_len = %d, Data_len = %d\r\n", topic_len, data_len);	// 串口打印【主题长度】【有效载荷长度】

    //1.判断返回的命令中有没有“Light_SW”
    tmpT = strstr(dataBuf, "Light");
    if(tmpT != NULL)
    {
    	//有“Light_SW”
    	tmpT = strstr(dataBuf, "ON");
    	if(tmpT != NULL)
    	{
    		//有setled 还有 on
    		os_printf("\r\n收到云端的命令，进行开灯\r\n");
    		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);
    		LampSt=0;
    		gpio16_output_set(LampSt);    //开灯的命令
    		iversLampSt=1;


    	}
    	else
    	{
    		os_printf("\r\n收到云端的命令，进行关灯\r\n");
    		LampSt=1;
    		iversLampSt=0;
    		gpio16_output_set(LampSt);    //关灯的命令
    		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);
    	}
    }

    //2.判断返回的命令中有没有“Dev_SW”
     tmpT = strstr(dataBuf, "Motor");
     if(tmpT != NULL)
     {
     	//有“Dev_SW”
     	tmpT = strstr(dataBuf, "ON");
     	if(tmpT != NULL)
     	{
     		//有setled 还有 on
     		os_printf("\r\n收到云端的命令，进行开启电机\r\n");
     		DevSt=0;
     		iversDevSt=1;

     		GPIO_OUTPUT_SET(12,DevSt);    //开电机的命令
     		os_printf("\r\n此时的电机信号为 %d\r\n",DevSt);
     	}
     	else
     	{
     		os_printf("\r\n收到云端的命令，进行关闭电机\r\n");
     		DevSt=1;
     		iversDevSt=0;

     		GPIO_OUTPUT_SET(12,DevSt);    //开电机的命令

     	}
     }

     //3.判断返回的命令中有没有“AutoMode”   //给系统设置为自动工作模式
          tmpT = strstr(dataBuf, "AutoMode");
          if(tmpT != NULL)
          {
          	//有“Dev_SW”
          	tmpT = strstr(dataBuf, "ON");
          	if(tmpT != NULL)
          	{
          		//有AutoMode 还有 on
          		os_printf("\r\n收到云端的命令，设备进入自动模式\r\n");
          		AutomodeDo =1;
          		HumantoAuto =1;

          	}
          	else
          	{
          		os_printf("\r\n收到云端的命令，设备进入人工控制模式\r\n");
          		AutomodeDo=0;  //在下一轮的TIMER函数中，会感知到；

          	}
          }

    //Setup_JSON_Tree_JX();
    os_sprintf(A_JSON_Tree, JSON_Tree_Format,luminance,distant,humitystr,tempstr,iversLampSt,iversDevSt);
    os_strncpy(luminanceData+strlen(luminanceData)-36, topicBuf+topic_len-36, 36);
    INFO("luminanceData: %s\r\n", luminanceData);
    MQTT_Publish(client, luminanceData, A_JSON_Tree, strlen(A_JSON_Tree), 0, 0);

    os_free(topicBuf);	// 释放【主题】空间
    os_free(dataBuf);	// 释放【有效载荷】空间
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


// 全局变量
//==================================================================================
u8 F_LED = 0;				// LED状态标志位

os_timer_t OS_Timer_1;		// 软件定时器
//==================================================================================
//将证书变成字符串
void int_to_string(int num,char data[])
{

	int i=0,j=0;
	char temp[10];

	while(num)
	{
    	temp[i++]=num%10+'0';  //将数字加字符0就变成相应字符
    	num/=10;               //此时的字符串为逆序
	}
    temp[i]='\0';
 	i=i-1;
 	while(i>=0)
 		data[j++]=temp[i--];   //将逆序的字符串转为正序
 	data[j]='\0';               //字符串结束标志
}

// 毫秒延时函数
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================

//定时上报属性
char luminanceReport[256] = "$oc/devices/64a389ee9415fc7a573b7ef3_Smart_Agriculture_LiteOS/sys/properties/report";
// 定时的回调函数（初始化1s循环一次，下边这个函数就会1s调用一次）
//==========================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{

	uint16_t light =0;
	real32_t tempLimit =0.0,humidLimit=0.0;
	uint16_t a=0;
	uint16_t h=0,l=0;
	uint16_t readIO13=0;




		char data1[10]={0},data2[10]={0};

		//读取距离传感器的数据
		readIO13=GPIO_INPUT_GET(GPIO_ID_PIN(13));
		os_printf("\r\n13端口的初始值：%d \r\n",readIO13);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(14),1);	//超声波脉冲信号 14口进行发射os_printf("\r\n 12端口的发射：1 \r\n");
		os_delay_us(20);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(14),0);	//超声波脉冲信号14	//os_printf("\r\n 12端口的发射：0 \r\n");

		readIO13=GPIO_INPUT_GET(GPIO_ID_PIN(13));

		os_printf("\r\n第一次调试数值 13端口的数为：%d \r\n",readIO13);

		while(readIO13 == 0)
			{
				readIO13=GPIO_INPUT_GET(GPIO_ID_PIN(13));
				a++;
				if (a>10000)break;
			}  //13口进行数据采集
			h=system_get_rtc_time();
		while(readIO13 == 1)
			{
				readIO13=GPIO_INPUT_GET(GPIO_ID_PIN(13));
				a++;
				l=system_get_rtc_time();
				if (a>10000)break;
			}

			/*os_printf("\r\n 第二次调试数值 13端口的数为：%d \r\n",readIO14);*/
			distant = l-h;
			os_printf("\r\n(l-h)==%d\r\n",l-h);
			distant = (l-h)*34/26;  //计算距离 Len_mm = (Time_Echo_us * 0.34mm/us) / 2 (mm)
			os_printf("\r\n(l-h)*34/2/26==%u\r\n",distant);


	if(DHT11_Read_Data_Complete() == 0)		// 读取DHT11温湿度值
			{
				//-------------------------------------------------
				// DHT11_Data_Array[0] == 湿度_整数_部分 --无符号整型 120以内，可以表达一个小数
				// DHT11_Data_Array[1] == 湿度_小数_部分  ---无符号整型 120以内，可以表达一个小数
				// DHT11_Data_Array[2] == 温度_整数_部分
				// DHT11_Data_Array[3] == 温度_小数_部分
				// DHT11_Data_Array[4] == 校验字节
				// DHT11_Data_Array[5] == 【1:温度>=0】【0:温度<0】 -----表示正数和负数
				//-------------------------------------------------





				// 串口输出温湿度
				//---------------------------------------------------------------------------------
				if(DHT11_Data_Array[5] == 1)			// 温度 >= 0℃
				{
					os_printf("\r\n湿度 == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
					os_printf("\r\n温度 == %d.%d ℃\r\n", DHT11_Data_Array[2],DHT11_Data_Array[3]);
				}
				else // if(DHT11_Data_Array[5] == 0)	// 温度 < 0℃
				{
					os_printf("\r\n湿度 == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
					os_printf("\r\n温度 == -%d.%d ℃\r\n",DHT11_Data_Array[2],DHT11_Data_Array[3]);
				}
//
//				// OLED显示温湿度
//				//---------------------------------------------------------------------------------
				DHT11_NUM_Char();	// DHT11数据值转成字符串

				OLED_ShowString(40,0,DHT11_Data_Char[0]);	// DHT11_Data_Char[0] == 【湿度字符串】
				OLED_ShowString(40,2,DHT11_Data_Char[1]);	// DHT11_Data_Char[1] == 【温度字符串】
			}

	os_strncpy(tempstr,DHT11_Data_Char[1],strlen(DHT11_Data_Char[1]));
	os_strncpy(humitystr,DHT11_Data_Char[0],strlen(DHT11_Data_Char[0]));
	os_printf("卧室的亮度是%d\r\n",  system_adc_read());

	luminance = system_adc_read();
	light=luminance;
	// 读取设备状态，上报到云

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
		// 温度超过32℃，LED亮
		//----------------------------------------------------
		tempLimit =DHT11_Data_Array[2]+ DHT11_Data_Array[3]/100;
		humidLimit =DHT11_Data_Array[0]+ DHT11_Data_Array[1]/100;
				if(DHT11_Data_Array[5]==1 && tempLimit>=32.4){
					//GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);		// 外接电机启动
					GPIO_OUTPUT_SET(GPIO_ID_PIN(12),0);		// 外接电机启动
					iversDevSt=1;
					DevSt=0;  //表示缺省为高电平状态
				}
				else if (humidLimit>=86.0){
					GPIO_OUTPUT_SET(GPIO_ID_PIN(12),0);		// 外接电机启动
					iversDevSt=1;
					DevSt=0;  //表示缺省为高电平状态
				}
				else{
					//GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);		// 外接电机关机
					GPIO_OUTPUT_SET(GPIO_ID_PIN(12),1);		// 外接电机关机
					iversDevSt=0;
					DevSt=1;  //表示缺省为高电平状态
				}

		if (light>=600){
			gpio16_output_set(1);   //开启16端口，控制灯泡
			iversLampSt=0;
			LampSt=1;
		}
		else{
			gpio16_output_set(0);   //开启16端口，控制灯泡
			iversLampSt=1;
			LampSt=0;
			}

	}
	else              //在上一轮TIMER函数过程中，是不是已经为人工模式，此处，就什么也不做，上一轮为自动模式，刚转为人工模式，就做一些初始化
	{
		//AutomodeDo =0,HumantoAuto
		if(HumantoAuto)
		{
			gpio16_output_set(1);   //开启16端口，控制灯泡
			GPIO_OUTPUT_SET(GPIO_ID_PIN(12),1);//恢复到停止状态；
			iversLampSt=0;
			iversDevSt=0;
			LampSt=1;
			DevSt=1;  //表示缺省为高电平状态

			HumantoAuto=0;   //避免在TIMER函数中反复设置引脚电平；
		}
	}


	//将光线数据包装成IoT云需要的JSON格式
	os_sprintf(A_JSON_Tree, JSON_Tree_Format,luminance,distant,humitystr,tempstr,iversLampSt,iversDevSt); /*,LampStstr,DevStstr*/
	//发布到指定的设备上的属性数据
	MQTT_Publish(&mqttClient, luminanceReport, A_JSON_Tree, strlen(A_JSON_Tree), 0, 0);


}
//==========================================================================================

// 软件定时器初始化(ms毫秒)
//==========================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{

	os_timer_disarm(&OS_Timer_1);	// 关闭定时器
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// 设置定时器
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // 使能定时器
}
//==========================================================================================

// LED初始化
//==========================================================================================
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	//PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U,	FUNC_GPIO2);	// GPI2设为IO口

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4设为IO口

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U,	FUNC_GPIO13);	// GPIO13设为IO口
//
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(13)); 					// IO13 = 1

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U,	FUNC_GPIO14);	// GPIO14设为IO口
//
	GPIO_OUTPUT_SET(GPIO_ID_PIN(14),1);						// IO14 = 1


	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
	//16口的初始化，作为灯泡使用；
	gpio16_output_conf();
	gpio16_output_set(1);    //输出高指定电平

	//设置电机的12口的初始化，作为电机的驱动使用
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,	FUNC_GPIO12);	// GPIO12设为IO口
	//
	GPIO_OUTPUT_SET(GPIO_ID_PIN(12),1);						// IO12 = 1

}

// user_init：entry of user application, init user function here
//===================================================================================================================
void user_init(void)
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);	// 串口波特率设为115200
    os_delay_us(60000);

	LED_Init_JX();		// LED初始化
	OLED_Init();							// OLED初始化---主要工作是清屏
	OLED_ShowString(0,0,"Humi:");		    // 湿度--只能显示英语--写汉字就是乱码
	OLED_ShowString(0,2,"Temp:");	// 温度
	OLED_ShowString(0,4,"Lght:");		    // 湿度--只能显示英语--写汉字就是乱码
	OLED_ShowString(0,6,"Dist:");	// 温度

	OS_Timer_1_Init_JX(1000,1);		// 1秒定时毫秒级(重复)


    CFG_Load();	// 加载/更新系统参数【WIFI参数、MQTT参数】
	// 连接WIFI：SSID[..]、PASSWORD[..]、WIFI连接成功函数[wifiConnectCb]
	//--------------------------------------------------------------------------
	WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);

	Setup_JSON_Tree_JX();		// 创建JSON树

    // 网络连接参数赋值：服务端域名【mqtt_test_jx.mqtt.iot.gz.baidubce.com】、网络连接端口【1883】、安全类型【0：NO_TLS】
	//-------------------------------------------------------------------------------------------------------------------
	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);

	// MQTT连接参数赋值：客户端标识符【..】、MQTT用户名【..】、MQTT密钥【..】、保持连接时长【120s】、清除会话【1：clean_session】
	//----------------------------------------------------------------------------------------------------------------------------
	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);

	// 设置遗嘱参数(如果云端没有对应的遗嘱主题，则MQTT连接会被拒绝)
	//--------------------------------------------------------------
	MQTT_InitLWT(&mqttClient, "Will", "ESP8266_offline", 0, 0);


	// 设置MQTT相关函数
	//--------------------------------------------------------------------------------------------------
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);			// 设置【MQTT成功连接】函数的另一种调用方式
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);	// 设置【MQTT成功断开】函数的另一种调用方式
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);			// 设置【MQTT成功发布】函数的另一种调用方式
	MQTT_OnData(&mqttClient, mqttDataCb);					// 设置【接收MQTT数据】函数的另一种调用方式


	INFO("\r\nSystem started ...\r\n");
}
//===================================================================================================================
