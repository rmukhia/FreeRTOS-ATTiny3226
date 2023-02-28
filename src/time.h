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

    uint16_t millis(void);
    void rtc_ovf_callback(void);

#ifdef	__cplusplus
}
#endif

#endif	/* WS_TIME_H */

