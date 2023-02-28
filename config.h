/* 
 * File:   config.h
 * Author: raunak
 *
 * Created on 13 February, 2023, 10:12 PM
 */

#ifndef WS_CONFIG_H
#define	WS_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#define WS_RTC_OVERFLOW_COUNTER     GPIOR3

    
/* Be sure to update the corresponding value for CLK_HZ and Period using 
 * graphical MCC or MCC generated files - timer/src/rtc.c */    
#define WS_RTC_CLK_HZ               1024



#ifdef	__cplusplus
}
#endif

#endif	/* WS_CONFIG_H */

