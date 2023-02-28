/* 
 * File:   time.h
 * Author: raunak
 *
 * Created on February 13, 2023, 9:43 PM
 */

#ifndef WS_TIME_H
#define	WS_TIME_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef uint32_t time_t;
    time_t millis(void);
    void rtc_ovf_callback(void) __attribute__((naked));

#ifdef	__cplusplus
}
#endif

#endif	/* WS_TIME_H */

