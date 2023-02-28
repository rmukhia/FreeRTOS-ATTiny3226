#include <stdint.h>
#include <stdio.h>
#include "mcc_generated_files/timer/rtc.h"
#include "time.h"
#include "config.h"
#include "portmacro.h"

#define PERIOD_OVF  64000

void rtc_ovf_callback(void) __attribute__((naked));

void rtc_ovf_callback(void)
{
    WS_RTC_OVERFLOW_COUNTER++;
    asm volatile ("ret");
}

time_t millis(void)
{
    uint16_t cnt;
    
    if (RTC.INTFLAGS & RTC_OVF_bm) {
        return ((WS_RTC_OVERFLOW_COUNTER + 1) * (time_t)PERIOD_OVF);
    }
    
    cnt = ((float) RTC_ReadCounter() / 1024.f) * 1000;
    //printf("per: %u cnt:%u ctr: %u\r\n", per, cnt, WS_RTC_OVERFLOW_COUNTER);
    return (WS_RTC_OVERFLOW_COUNTER * (time_t)PERIOD_OVF) + cnt;
}