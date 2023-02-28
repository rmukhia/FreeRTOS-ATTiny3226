#include <stdint.h>

#include "mcc_generated_files/timer/rtc.h"
#include "time.h"
#include "config.h"

const float factor = (float)WS_RTC_CLK_HZ/1000;


void rtc_ovf_callback(void)
{
    WS_RTC_OVERFLOW_COUNTER++;
}

uint16_t millis(void)
{
    uint16_t cnt = RTC_ReadCounter();
    uint16_t per = RTC_ReadPeriod();
    cnt = ((float)cnt / 1024.f) * 1000;
    return (WS_RTC_OVERFLOW_COUNTER * per) + (1 * cnt);
    // return cnt;
}