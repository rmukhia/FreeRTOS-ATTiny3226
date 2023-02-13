#include "mcc_generated_files/system/system.h"
#include <avr/sleep.h>
/*
    Main application
*/

void RTC_PIT_CallBack(void)
{
    GPIOR2++;
}

int main(void)
{
    SYSTEM_Initialize();

    RTC_SetPITIsrCallback(RTC_PIT_CallBack);
    
    while(1)
    {
        sleep_cpu();
    }    
}