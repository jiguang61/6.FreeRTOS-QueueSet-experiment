/**
    �����FreeRTOSʵ��
    ʹ�ÿ�����Ұ���STM32F429ϵ�п�����
    ʵ��6-���м�ʵ��
    ʱ�䣺23/11/29
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bsp_led.h"
#include "bsp_key.h" 
#include "bsp_debug_usart.h"

/*************************����������********************************/
/*������ƿ�ָ�봴��*/
TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t Receive_Task_Handle = NULL;
TaskHandle_t Send_Task_Handle = NULL;
TaskHandle_t Send_Task_Handle_2 = NULL;

/*************************�ں˾������********************************/
QueueHandle_t Test_Queue_Handle = NULL;
QueueHandle_t Test_Queue_Handle_2 = NULL;
QueueSetHandle_t Test_QueueSet_Handle = NULL;

/****************************��������********************************/
static void AppTaskCreate(void);
static void Receive_Test_Task(void * param);
static void Send_Test_Task(void * param);
static void Send_Test_Task_2(void * param);

/******************************�����ڱ������ĺ궨��*******************************/
/*���еĳ��ȣ����ɰ������ٸ���Ϣ*/
#define  QUEUE_LEN    2   
/*������ÿ����Ϣ��С*/
#define  QUEUE_SIZE   4   

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
    /*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
    BaseType_t xReturn = pdPASS;
    /*LED �˿ڳ�ʼ��*/
    LED_GPIO_Config();
    /*���ڳ�ʼ��*/
    Debug_USART_Config();
    /*��ʼ������*/
    Key_GPIO_Config();
    
    LED_RGBOFF;
    
    printf("FreeRTOSʵ��3,����ʵ�飬���߼���\n");
    
    //���������������������
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,			/* ������ں��� */
                        (const char*	)"AppTaskCreate",			/* �������� */
                        (uint16_t		)256,   					/* ����ջ��С */
                        (void*			)NULL,						/* ������ں������� */
                        (UBaseType_t	)1,	    					/* ��������ȼ� */
                        (TaskHandle_t*	)&AppTaskCreate_Handle);	/* ������ƿ�ָ�� */
    
    /*�����������*/
    if(xReturn == pdPASS)
    {
        /*�������������*/
        vTaskStartScheduler();
    }
    else
    {
        printf("�������ʧ�ܣ��������\n");
        return -1;
    }
    
    /*������˵����ִ�е�����*/
    while(1);
}

/**************************************************************************************/
static void AppTaskCreate(void)
{
    /*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
    BaseType_t xReturn = pdPASS;
    
    /*�����ٽ�α���*/
    taskENTER_CRITICAL();
    
    /* ����Test_Queue_Handle */
    Test_Queue_Handle = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
                            (UBaseType_t ) QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
    if(NULL != Test_Queue_Handle)
    {
        printf("����Test_Queue_Handle��Ϣ���гɹ�!\r\n");
    }

    /* ����Test_Queue_Handle 2 */
    Test_Queue_Handle_2 = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
                            (UBaseType_t ) QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
    if(NULL != Test_Queue_Handle_2)
    {
        printf("����Test_Queue_Handle_2��Ϣ���гɹ�!\r\n");
    }

    /*�������м������Դ����������*/
    Test_QueueSet_Handle = xQueueCreateSet(4);

    if(NULL != Test_QueueSet_Handle)
    {
        printf("����Test_QueueSet_Handle���м��ɹ�!\r\n");
    }

    /*������������ӵ����м�֮��*/ 
    xQueueAddToSet(Test_Queue_Handle , Test_QueueSet_Handle);
    xQueueAddToSet(Test_Queue_Handle_2 , Test_QueueSet_Handle);
    
    //������������
    xReturn = xTaskCreate((TaskFunction_t )Send_Test_Task,      /* ������ں��� */
                        (const char*	)"Send_Test_Task",      /* �������� */
                        (uint16_t		)512,                   /* ����ջ��С */
                        (void*			)"�������з��ͺ���",      /* ������ں������� */
                        (UBaseType_t	)2,                     /* ��������ȼ� */
                        (TaskHandle_t*	)&Send_Task_Handle);    /* ������ƿ�ָ�� */
                            
    /*�鿴�����Ƿ񴴽��ɹ�*/
    if(xReturn == pdPASS)
    {
        printf("�������񴴽��ɹ�\n");
    }

    //������������2
    xReturn = xTaskCreate((TaskFunction_t )Send_Test_Task_2,    /* ������ں��� */
                        (const char*	)"Send_Test_Task_2",    /* �������� */
                        (uint16_t		)512,                   /* ����ջ��С */
                        (void*			)"�������з��ͺ���2",      /* ������ں������� */
                        (UBaseType_t	)2,                     /* ��������ȼ� */
                        (TaskHandle_t*	)&Send_Task_Handle_2);  /* ������ƿ�ָ�� */
                            
    /*�鿴�����Ƿ񴴽��ɹ�*/
    if(xReturn == pdPASS)
    {
        printf("��������2�����ɹ�\n");
    }

    //������������
    xReturn = xTaskCreate((TaskFunction_t )Receive_Test_Task,   /* ������ں��� */
                        (const char*	)"Receive_Test_Task",   /* �������� */
                        (uint16_t		)512,                   /* ����ջ��С */
                        (void*			)"�������н��ܺ���",        /* ������ں������� */
                        (UBaseType_t	)3,                     /* ��������ȼ� */
                        (TaskHandle_t*	)&Receive_Task_Handle); /* ������ƿ�ָ�� */
    
    /*�鿴�����Ƿ񴴽��ɹ�*/
    if(xReturn == pdPASS)
    {
        printf("�������񴴽��ɹ�\n");
    }
    
    /*ɾ������*/
    /*���������ҳ�����ֱ������NULL��ɾ���������������ϾͻῨ����prvTaskExitError��������У��ܹ�*/
    vTaskDelete(AppTaskCreate_Handle);
    
    taskEXIT_CRITICAL();
}

static void Receive_Test_Task(void * param)
{
    /*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
    // BaseType_t xReturn = pdPASS;

    QueueSetMemberHandle_t QS_Test_handle;
    int val = 0;
    while(1)
    {

        /*��ȡ���ĸ����з�������Ϣ*/
        QS_Test_handle = xQueueSelectFromSet(Test_QueueSet_Handle, portMAX_DELAY);

        /*��ȡ���ܵ�����Ϣ*/
        xQueueReceive(QS_Test_handle, &val, portMAX_DELAY);

        printf("��������ִ�гɹ��������Ƕ���%d���͵�\n",val);


    }
}

void Send_Test_Task(void * param)
{
    /*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
    // BaseType_t xReturn = pdPASS;
    int val = 1;
    while(1)
    {
        xQueueSend(Test_Queue_Handle, &val, portMAX_DELAY);

        // if(xReturn == pdPASS)
        // {
        //     printf("��������1ִ�гɹ�\n");
        // }

        /*��ʱ20��tick*/
        vTaskDelay(20);
    }
}

void Send_Test_Task_2(void * param)
{
    /*������Ϣ��ֵ֤����ʼĬ��ΪpdPASS*/
    // BaseType_t xReturn = pdPASS;
    while(1)
    {
        int val = 2;
        xQueueSend(Test_Queue_Handle_2, &val, portMAX_DELAY);

        // if(xReturn == pdPASS)
        // {
        //     printf("��������2ִ�гɹ�\n");
        // }

        /*��ʱ20��tick*/
        vTaskDelay(20);
    }
}




/*********************************************END OF FILE**********************/

