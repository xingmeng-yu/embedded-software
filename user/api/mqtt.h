#ifndef _MQTT_H_
#define _MQTT_H_

#include "stm32f10x.h"
#include "kqm6600.h"
#include "dht11.h"
extern u8 mqtt_step;

//#define MQTT_CLIENT_ID "D48"
//#define MQTT_USERNAME  "123"
//#define MQTT_PASSWD    "123456"
//#define MQTT_IP   "broker.emqx.io"
//#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "demo_test"
#define MQTT_USERNAME  "SU1AWZp7nl"
#define MQTT_PASSWD    "version=2018-10-31&res=products%2FSU1AWZp7nl%2Fdevices%2Fdemo_test&et=1798618061&method=md5&sign=Hm8nDxydc4errUX%2Fcrrxsg%3D%3D"
#define MQTT_IP   "mqtts.heclouds.com"
#define MQTT_PORT 1883

#define NET_PDD_IP   "api.pinduoduo.com"
#define NET_PDD_PORT 80
//Ö÷Ìâ
//#define MQTT_TOPIC1 "classroom/sensor/wzl"
//#define MQTT_SUB_TOPIC1 "classroom/wzl"
#define MQTT_TOPIC1 "$sys/SU1AWZp7nl/demo_test/thing/property/post"
#define MQTT_SUB_TOPIC1 "$sys/SU1AWZp7nl/demo_test/thing/property/post/reply"

#define MQTT_TOPIC2 "$sys/SU1AWZp7nl/demo_test/thing/property/set_reply"
#define MQTT_SUB_TOPIC2 "$sys/SU1AWZp7nl/demo_test/thing/property/set"

u8 MQTT_Connect(char *client_id,char *username,char *passwd,u32 timeout);
u8 MQTT_DisConnect(void);
char *GetTopic1Data(void);
u8 MQTT_Publish(char *topic,char *data);
u8 MQTT_SubscribeOneTopic(char *topic,u32 timeout);
void MQTT_Opreation(void);
void MQTT_RecvOnenet(void);
extern int my_light;
extern int my_tem;
extern float my_voc;
extern int my_co2;
extern float my_ch2o;
#endif
