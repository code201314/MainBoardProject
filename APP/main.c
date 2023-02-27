#include "main.h"
#include "sys.h"

#include "FreeRTOS.h"
#include "task.h"
/* ������Ӳ��bspͷ�ļ� */
#include "led.h"
#include "usart.h"

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
//AHT20�ɼ�һ����ʪ������������
static TaskHandle_t AHT20_Task_Handle = NULL;;


static void AppTaskCreate(void);/* ���ڴ������� */
/* LED1_Task����ʵ�� */
static void LED1_Task(void* pvParameters);
//����ͨ������ʵ��
static void USART_Task(void* parameter);
//AHT20�ɼ�һ����ʪ����������ʵ��
static void AHT20_Task(void* parameter);

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */


int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

  /* ������Ӳ����ʼ�� */
  BSP_Init();
  printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS-��̬����������ʵ��!\r\n");
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
	//AHT20�ɼ�һ����ʪ������
	xReturn = xTaskCreate((TaskFunction_t )AHT20_Task,
                        (const char*    )"AHT20_Task",
                        (uint16_t       )512,
                        (void*          )NULL,	
                        (UBaseType_t    )4,	 
                        (TaskHandle_t*  )&AHT20_Task_Handle);
  if(pdPASS == xReturn)
		printf("����AHT20_Task����ɹ�!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LEDС��
static void LED1_Task(void* parameter)
{	
    while (1)
    {
        LED0 = 0;
        vTaskDelay(200);   /* ��ʱ500��tick */
        printf("LED1_Task Running,LED1_ON\r\n");
        
        LED0 = 1;     
        vTaskDelay(200);   /* ��ʱ500��tick */		 		
        printf("LED1_Task Running,LED1_OFF\r\n");
    }
}


//����ͨ��
static void USART_Task(void* parameter)
{
	while (1)
	{
		vTaskDelay(2000);
		printf("helloworld!\r\n");
	}
}

//AHT20�ɼ�һ����ʪ������
static void AHT20_Task(void* parameter)
{
	while (1)
	{
		vTaskDelay(5000);
		printf("�ɼ����¶�Ϊ10��\r\n");
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
  
}

