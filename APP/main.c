/*
 * @Author: code201314 1162782792@qq.com
 * @Date: 2023-02-26 23:18:19
 * @LastEditors: code201314 1162782792@qq.com
 * @LastEditTime: 2023-03-12 23:53:42
 * @FilePath: \MainBoardProject\APP\main.c
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */

#include "main.h"
#include "sys.h"

#include "FreeRTOS.h"
#include "task.h"
/* ������Ӳ��bspͷ�ļ� */
#include "led.h"
#include "usart.h"
#include "elog.h"
#include "TinyFrame.h"
#include <stdlib.h>

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED1������ */
static TaskHandle_t LED1_Task_Handle = NULL;
//����ͨѶ������
static TaskHandle_t USART_Task_Handle = NULL;
//��־��ӡ������
static TaskHandle_t LOG_Task_Handle = NULL;

static void AppTaskCreate(void);/* ���ڴ������� */
/* LED1_Task����ʵ�� */
static void LED1_Task(void* pvParameters);
//����ͨ������ʵ��
static void USART_Task(void* parameter);
//��־����ʵ��
static void LOG_Task(void* pvParameters);

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */


void dumpFrame(const uint8_t *buff, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
        printf("%3u \033[94m%02X\033[0m", buff[i], buff[i]);
        if (buff[i] >= 0x20 && buff[i] < 127) {
            printf(" %c", buff[i]);
        }
        else {
            printf(" \033[31m.\033[0m");
        }
        printf("\n");
    }
    printf("--- end of frame ---\n\n");
}

void dumpFrameInfo(TF_Msg *msg)
{
    printf("\033[33mFrame info\n"
               "  type: %02Xh\n"
               "  data: \"%.*s\"\n"
               "   len: %u\n"
               "    id: %Xh\033[0m\n\n",
           msg->type, msg->len, msg->data, msg->len, msg->frame_id);
}

TF_Result myListener(TinyFrame *tf, TF_Msg *msg)
{
    dumpFrameInfo(msg);
    return TF_STAY;
}

int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

  /* ������Ӳ����ʼ�� */
  BSP_Init();
  printf("%s\r\n","This is MainBoard programer running!!!");
	printf("Currunt Date:%s Time:%s\r\n",__DATE__,__TIME__);
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}

//���е����񴴽�����
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED1_Task, /* ������ں��� */
                        (const char*    )"LED1_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED1_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����LED1_Task����ɹ�!\r\n");
  //��������ͨѶ����
  xReturn = xTaskCreate((TaskFunction_t )USART_Task,
                        (const char*    )"USART_Task",
                        (uint16_t       )512,
                        (void*          )NULL,	
                        (UBaseType_t    )3,	 
                        (TaskHandle_t*  )&USART_Task_Handle);
  if(pdPASS == xReturn)
    printf("����USART_Task����ɹ�!\r\n");
	  //����LOG��ӡ����
  xReturn = xTaskCreate((TaskFunction_t )LOG_Task,
                        (const char*    )"LOG_Task",
                        (uint16_t       )512,
                        (void*          )NULL,	
                        (UBaseType_t    )3,	 
                        (TaskHandle_t*  )&LOG_Task_Handle);
  if(pdPASS == xReturn)
    printf("����LOG_Task����ɹ�!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LEDС��
static void LED1_Task(void* parameter)
{	
    while (1)
    {
        LED0 = 0;
        vTaskDelay(500);   /* ��ʱ500��tick */
        LED0 = 1;     
        vTaskDelay(500);   /* ��ʱ500��tick */		 		
    }
}


//����ͨ��
static void USART_Task(void* parameter)
{
    while (1)
    {
        vTaskDelay(2000);
    }
}

static void LOG_Task(void* pvParameters)
{
  static uint8_t buff[]="usart test\r\n";
  while (1)
	{
		log_e("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		usart_write(USART1, buff, sizeof(buff));

		vTaskDelay(1000);
	}
}

static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_Init();

	/* ���ڳ�ʼ��	*/
	uart_init(115200);

  elog_init();
  /* set EasyLogger log format */
  elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
  elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
  elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
  /* start EasyLogger */
  elog_start();
}

