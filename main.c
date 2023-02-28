#include <avr/sleep.h>
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "FreeRTOS.h"
#include "task.h"


#include "mcc_generated_files/system/system.h"

#include "src/time.h"
#include "src/bits.h"

#define HIGH_STACK(str)    {                                            \
    uint16_t high_mark = uxTaskGetStackHighWaterMark2(NULL);            \
    printf("%s: High Stack %u\r\n", str, high_mark);                    \
}


/*-----------------------------------------------------------*/

TaskHandle_t main_task_hdl;
TaskHandle_t side_task_hdl;


/*-----------------------------------------------------------*/

portTASK_FUNCTION(main_task, pv)
{
    while (true) {
        time_t ms = millis();
        printf("%lu ms\r\n", ms);
        vTaskDelay(pdMS_TO_TICKS(500));
        
        HIGH_STACK("main task");
        portYIELD();
    }
}

portTASK_FUNCTION(side_task, pv)
{
    while (true) {
       IO_STATUSLED_Toggle();
       IO_WINDLED_Toggle();
       vTaskDelay(pdMS_TO_TICKS(500));
       printf ("side task\r\n");
    }
}

/*-----------------------------------------------------------*/

void vApplicationTickHook()
{
    //IO_RAINLED_Toggle();
    //printf ("t\n");
    //IO_STATUSLED_Toggle();
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *task_name)
{
    //IO_WINDLED_Toggle();
}

/*-----------------------------------------------------------*/

int main(void)
{
    SYSTEM_Initialize();

    RTC_SetPITIsrCallback(rtc_pit_callback);
    RTC_SetOVFIsrCallback(rtc_ovf_callback);
    
    printf("Hello world 71!\r\n");
    
    xTaskCreate(
            main_task, 
            "main task",
            configMINIMAL_STACK_SIZE + 124,
            NULL,
            1,
            &main_task_hdl
            );
    
        xTaskCreate(
            side_task, 
            "side task",
            configMINIMAL_STACK_SIZE + 124,
            NULL,
            2,
            &side_task_hdl
            );
    
    printf("Hello world 23!\r\n");
    vTaskStartScheduler();
    
    printf("End!\n");
    
    return 0;
}