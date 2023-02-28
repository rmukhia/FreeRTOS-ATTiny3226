/* 
 * File:   bits.h
 * Author: raunak
 *
 * Created on February 15, 2023, 8:57 PM
 */

#ifndef BITS_H
#define	BITS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    
/* picked up from https://stackoverflow.com/a/263738 */
    
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1

#define BITMASK_SET(x, mask) ((x) |= (mask))
#define BITMASK_CLEAR(x, mask) ((x) &= (~(mask)))
#define BITMASK_FLIP(x, mask) ((x) ^= (mask))
#define BITMASK_CHECK_ALL(x, mask) (!(~(x) & (mask)))
#define BITMASK_CHECK_ANY(x, mask) ((x) & (mask))

#ifdef	__cplusplus
}
#endif

#endif	/* BITS_H */

