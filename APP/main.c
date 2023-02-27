#include "main.h"
#include "sys.h"

#include "FreeRTOS.h"
#include "task.h"
/* 开发板硬件bsp头文件 */
#include "led.h"
#include "usart.h"

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
//AHT20采集一次温湿度数据任务句柄
static TaskHandle_t AHT20_Task_Handle = NULL;;


static void AppTaskCreate(void);/* 用于创建任务 */
/* LED1_Task任务实现 */
static void LED1_Task(void* pvParameters);
//串口通信任务实现
static void USART_Task(void* parameter);
//AHT20采集一次温湿度数据任务实现
static void AHT20_Task(void* parameter);

static void BSP_Init(void);/* 用于初始化板载相关资源 */


int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

  /* 开发板硬件初始化 */
  BSP_Init();
  printf("这是一个[野火]-STM32全系列开发板-FreeRTOS-动态创建多任务实验!\r\n");
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
	//AHT20采集一次温湿度数据
	xReturn = xTaskCreate((TaskFunction_t )AHT20_Task,
                        (const char*    )"AHT20_Task",
                        (uint16_t       )512,
                        (void*          )NULL,	
                        (UBaseType_t    )4,	 
                        (TaskHandle_t*  )&AHT20_Task_Handle);
  if(pdPASS == xReturn)
		printf("创建AHT20_Task任务成功!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

//LED小灯
static void LED1_Task(void* parameter)
{	
    while (1)
    {
        LED0 = 0;
        vTaskDelay(200);   /* 延时500个tick */
        printf("LED1_Task Running,LED1_ON\r\n");
        
        LED0 = 1;     
        vTaskDelay(200);   /* 延时500个tick */		 		
        printf("LED1_Task Running,LED1_OFF\r\n");
    }
}


//串口通信
static void USART_Task(void* parameter)
{
	while (1)
	{
		vTaskDelay(2000);
		printf("helloworld!\r\n");
	}
}

//AHT20采集一次温湿度数据
static void AHT20_Task(void* parameter)
{
	while (1)
	{
		vTaskDelay(5000);
		printf("采集的温度为10℃\r\n");
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
  
}

