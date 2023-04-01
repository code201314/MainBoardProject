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
/* 开发板硬件bsp头文件 */
#include "led.h"
#include "usart.h"
#include "elog.h"
#include "TinyFrame.h"
#include <stdlib.h>

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED1任务句柄 */
static TaskHandle_t LED1_Task_Handle = NULL;
//串口通讯任务句柄
static TaskHandle_t USART_Task_Handle = NULL;
//日志打印任务句柄
static TaskHandle_t LOG_Task_Handle = NULL;

static void AppTaskCreate(void);/* 用于创建任务 */
/* LED1_Task任务实现 */
static void LED1_Task(void* pvParameters);
//串口通信任务实现
static void USART_Task(void* parameter);
//日志任务实现
static void LOG_Task(void* pvParameters);

static void BSP_Init(void);/* 用于初始化板载相关资源 */


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
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

  /* 开发板硬件初始化 */
  BSP_Init();
  printf("%s\r\n","This is MainBoard programer running!!!");
	printf("Currunt Date:%s Time:%s\r\n",__DATE__,__TIME__);
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}

//所有的任务创建函数
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
  
  /* 创建LED_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )LED1_Task, /* 任务入口函数 */
                        (const char*    )"LED1_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&LED1_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建LED1_Task任务成功!\r\n");
  //创建串口通讯任务
  xReturn = xTaskCreate((TaskFunction_t )USART_Task,
                        (const char*    )"USART_Task",
                        (uint16_t       )512,
                        (void*          )NULL,	
                        (UBaseType_t    )3,	 
                        (TaskHandle_t*  )&USART_Task_Handle);
  if(pdPASS == xReturn)
    printf("创建USART_Task任务成功!\r\n");
	  //创建LOG打印任务
  xReturn = xTaskCreate((TaskFunction_t )LOG_Task,
                        (const char*    )"LOG_Task",
                        (uint16_t       )512,
                        (void*          )NULL,	
                        (UBaseType_t    )3,	 
                        (TaskHandle_t*  )&LOG_Task_Handle);
  if(pdPASS == xReturn)
    printf("创建LOG_Task任务成功!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

//LED小灯
static void LED1_Task(void* parameter)
{	
    while (1)
    {
        LED0 = 0;
        vTaskDelay(500);   /* 延时500个tick */
        LED0 = 1;     
        vTaskDelay(500);   /* 延时500个tick */		 		
    }
}


//串口通信
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
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_Init();

	/* 串口初始化	*/
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

