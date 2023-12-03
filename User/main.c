/**
    极光的FreeRTOS实验
    使用开发版野火的STM32F429系列开发板
    实验6-队列集实验
    时间：23/11/29
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bsp_led.h"
#include "bsp_key.h" 
#include "bsp_debug_usart.h"

/*************************任务句柄创建********************************/
/*任务控制块指针创建*/
TaskHandle_t AppTaskCreate_Handle = NULL;
TaskHandle_t Receive_Task_Handle = NULL;
TaskHandle_t Send_Task_Handle = NULL;
TaskHandle_t Send_Task_Handle_2 = NULL;

/*************************内核句柄创建********************************/
QueueHandle_t Test_Queue_Handle = NULL;
QueueHandle_t Test_Queue_Handle_2 = NULL;
QueueSetHandle_t Test_QueueSet_Handle = NULL;

/****************************函数声明********************************/
static void AppTaskCreate(void);
static void Receive_Test_Task(void * param);
static void Send_Test_Task(void * param);
static void Send_Test_Task_2(void * param);

/******************************仅用于本函数的宏定义*******************************/
/*队列的长度，最大可包含多少个消息*/
#define  QUEUE_LEN    2   
/*队列中每个消息大小*/
#define  QUEUE_SIZE   4   

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    /*定义消息验证值，初始默认为pdPASS*/
    BaseType_t xReturn = pdPASS;
    /*LED 端口初始化*/
    LED_GPIO_Config();
    /*串口初始化*/
    Debug_USART_Config();
    /*初始化按键*/
    Key_GPIO_Config();
    
    LED_RGBOFF;
    
    printf("FreeRTOS实验3,队列实验，作者极光\n");
    
    //创建用来创建任务的任务
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,			/* 任务入口函数 */
                        (const char*	)"AppTaskCreate",			/* 任务名字 */
                        (uint16_t		)256,   					/* 任务栈大小 */
                        (void*			)NULL,						/* 任务入口函数参数 */
                        (UBaseType_t	)1,	    					/* 任务的优先级 */
                        (TaskHandle_t*	)&AppTaskCreate_Handle);	/* 任务控制块指针 */
    
    /*启动任务调度*/
    if(xReturn == pdPASS)
    {
        /*启动任务调度器*/
        vTaskStartScheduler();
    }
    else
    {
        printf("任务调度失败，请检查程序\n");
        return -1;
    }
    
    /*正常来说不会执行到这里*/
    while(1);
}

/**************************************************************************************/
static void AppTaskCreate(void)
{
    /*定义消息验证值，初始默认为pdPASS*/
    BaseType_t xReturn = pdPASS;
    
    /*开启临界段保护*/
    taskENTER_CRITICAL();
    
    /* 创建Test_Queue_Handle */
    Test_Queue_Handle = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* 消息队列的长度 */
                            (UBaseType_t ) QUEUE_SIZE);/* 消息的大小 */
    if(NULL != Test_Queue_Handle)
    {
        printf("创建Test_Queue_Handle消息队列成功!\r\n");
    }

    /* 创建Test_Queue_Handle 2 */
    Test_Queue_Handle_2 = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* 消息队列的长度 */
                            (UBaseType_t ) QUEUE_SIZE);/* 消息的大小 */
    if(NULL != Test_Queue_Handle_2)
    {
        printf("创建Test_Queue_Handle_2消息队列成功!\r\n");
    }

    /*创建队列集，可以存放两个队列*/
    Test_QueueSet_Handle = xQueueCreateSet(4);

    if(NULL != Test_QueueSet_Handle)
    {
        printf("创建Test_QueueSet_Handle队列集成功!\r\n");
    }

    /*将两个队列添加到队列集之中*/ 
    xQueueAddToSet(Test_Queue_Handle , Test_QueueSet_Handle);
    xQueueAddToSet(Test_Queue_Handle_2 , Test_QueueSet_Handle);
    
    //创建发送任务
    xReturn = xTaskCreate((TaskFunction_t )Send_Test_Task,      /* 任务入口函数 */
                        (const char*	)"Send_Test_Task",      /* 任务名字 */
                        (uint16_t		)512,                   /* 任务栈大小 */
                        (void*			)"正在运行发送函数",      /* 任务入口函数参数 */
                        (UBaseType_t	)2,                     /* 任务的优先级 */
                        (TaskHandle_t*	)&Send_Task_Handle);    /* 任务控制块指针 */
                            
    /*查看任务是否创建成功*/
    if(xReturn == pdPASS)
    {
        printf("发送任务创建成功\n");
    }

    //创建发送任务2
    xReturn = xTaskCreate((TaskFunction_t )Send_Test_Task_2,    /* 任务入口函数 */
                        (const char*	)"Send_Test_Task_2",    /* 任务名字 */
                        (uint16_t		)512,                   /* 任务栈大小 */
                        (void*			)"正在运行发送函数2",      /* 任务入口函数参数 */
                        (UBaseType_t	)2,                     /* 任务的优先级 */
                        (TaskHandle_t*	)&Send_Task_Handle_2);  /* 任务控制块指针 */
                            
    /*查看任务是否创建成功*/
    if(xReturn == pdPASS)
    {
        printf("发送任务2创建成功\n");
    }

    //创建接收任务
    xReturn = xTaskCreate((TaskFunction_t )Receive_Test_Task,   /* 任务入口函数 */
                        (const char*	)"Receive_Test_Task",   /* 任务名字 */
                        (uint16_t		)512,                   /* 任务栈大小 */
                        (void*			)"正在运行接受函数",        /* 任务入口函数参数 */
                        (UBaseType_t	)3,                     /* 任务的优先级 */
                        (TaskHandle_t*	)&Receive_Task_Handle); /* 任务控制块指针 */
    
    /*查看任务是否创建成功*/
    if(xReturn == pdPASS)
    {
        printf("接收任务创建成功\n");
    }
    
    /*删除自身*/
    /*？？这里我尝试了直接输入NULL来删除自身，但程序马上就会卡死在prvTaskExitError这个函数中，很怪*/
    vTaskDelete(AppTaskCreate_Handle);
    
    taskEXIT_CRITICAL();
}

static void Receive_Test_Task(void * param)
{
    /*定义消息验证值，初始默认为pdPASS*/
    // BaseType_t xReturn = pdPASS;

    QueueSetMemberHandle_t QS_Test_handle;
    int val = 0;
    while(1)
    {

        /*获取是哪个队列发送了消息*/
        QS_Test_handle = xQueueSelectFromSet(Test_QueueSet_Handle, portMAX_DELAY);

        /*获取接受到的消息*/
        xQueueReceive(QS_Test_handle, &val, portMAX_DELAY);

        printf("接收任务执行成功，本次是队列%d发送的\n",val);


    }
}

void Send_Test_Task(void * param)
{
    /*定义消息验证值，初始默认为pdPASS*/
    // BaseType_t xReturn = pdPASS;
    int val = 1;
    while(1)
    {
        xQueueSend(Test_Queue_Handle, &val, portMAX_DELAY);

        // if(xReturn == pdPASS)
        // {
        //     printf("发送任务1执行成功\n");
        // }

        /*延时20个tick*/
        vTaskDelay(20);
    }
}

void Send_Test_Task_2(void * param)
{
    /*定义消息验证值，初始默认为pdPASS*/
    // BaseType_t xReturn = pdPASS;
    while(1)
    {
        int val = 2;
        xQueueSend(Test_Queue_Handle_2, &val, portMAX_DELAY);

        // if(xReturn == pdPASS)
        // {
        //     printf("发送任务2执行成功\n");
        // }

        /*延时20个tick*/
        vTaskDelay(20);
    }
}




/*********************************************END OF FILE**********************/

