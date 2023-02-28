/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "task.h"

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the AVR port.
 *----------------------------------------------------------*/

/* Start tasks with interrupts enables. */
#define portFLAGS_INT_ENABLED    ( ( StackType_t ) 0x80 )

/*-----------------------------------------------------------*/

/* We require the address of the pxCurrentTCB variable, but don't want to know
 * any details of its type. */
typedef void RTOS_TCB_t;
extern volatile RTOS_TCB_t * volatile pxCurrentTCB;

/*-----------------------------------------------------------*/

/*
 * Perform hardware setup to enable ticks from timer.
 */
static void prvSetupTimerInterrupt(void);
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack(StackType_t * pxTopOfStack,
        TaskFunction_t pxCode,
        void * pvParameters) {
    uint16_t usAddress;

    /*lint -e950 -e611 -e923 Lint doesn't like this much - but nothing I can do about it. */

    /* Place a few bytes of known values on the bottom of the stack.
     * This is just useful for debugging. Uncomment if needed. */
    /* *pxTopOfStack = 0x11; */
    /* pxTopOfStack--; */
    /* *pxTopOfStack = 0x22; */
    /* pxTopOfStack--; */
    /* *pxTopOfStack = 0x33; */
    /* pxTopOfStack--; */

    /* The start of the task code will be popped off the stack last, so place
     * it on first. */
    usAddress = (uint16_t) pxCode;
    *pxTopOfStack = (StackType_t) (usAddress & (uint16_t) 0x00ff);
    pxTopOfStack--;

    usAddress >>= 8;
    *pxTopOfStack = (StackType_t) (usAddress & (uint16_t) 0x00ff);
    pxTopOfStack--;

    /* Next simulate the stack as if after a call to portSAVE_CONTEXT().
     *  portSAVE_CONTEXT places the flags on the stack immediately after r0
     *  to ensure the interrupts get disabled as soon as possible, and so ensuring
     *  the stack use is minimal should a context switch interrupt occur. */
    *pxTopOfStack = (StackType_t) 0x00; /* R0 */
    pxTopOfStack--;
    *pxTopOfStack = portFLAGS_INT_ENABLED;
    pxTopOfStack--;
#if defined(__AVR_HAVE_RAMPZ__)
    *pxTopOfStack = (StackType_t) 0x00; /* RAMPZ */
    pxTopOfStack--;
#endif

    /* Now the remaining registers.   The compiler expects R1 to be 0. */
    *pxTopOfStack = (StackType_t) 0x00; /* R1 */

    /* Leave R2 - R23 untouched */
    pxTopOfStack -= 23;

    /* Place the parameter on the stack in the expected location. */
    usAddress = (uint16_t) pvParameters;
    *pxTopOfStack = (StackType_t) (usAddress & (uint16_t) 0x00ff);
    pxTopOfStack--;

    usAddress >>= 8;
    *pxTopOfStack = (StackType_t) (usAddress & (uint16_t) 0x00ff);

    /* Leave register R26 - R31 untouched */
    pxTopOfStack -= 7;

    /*lint +e950 +e611 +e923 */

    return pxTopOfStack;
}

/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler(void) {
    /* Setup the hardware to generate the tick. */
    prvSetupTimerInterrupt();

    /* Restore the context of the first task that is going to run. */
    portRESTORE_CONTEXT();

    /* Simulate a function call end as generated by the compiler.  We will now
     * jump to the start of the task the context of which we have just restored. */
    asm volatile ( "ret");

    /* Should not get here. */
    return pdTRUE;
}

/*-----------------------------------------------------------*/

void vPortEndScheduler(void) {
    /* vPortEndScheduler is not implemented in this port. */
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch.  The first thing we do is save the registers so we
 * can use a naked attribute.
 */
void vPortYield(void) __attribute__((naked));

void vPortYield(void) {
    portSAVE_CONTEXT();
    vTaskSwitchContext();
    portRESTORE_CONTEXT();
    asm volatile ( "ret");
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch callable from ISRs. The first thing
 * we do is save the registers so we can use a naked attribute.
 */
void vPortYieldFromISR(void) __attribute__((naked));

void vPortYieldFromISR(void) {
    portSAVE_CONTEXT();
    vTaskSwitchContext();
    portRESTORE_CONTEXT();
    asm volatile ( "reti");
}
/*-----------------------------------------------------------*/

/*
 * Context switch function used by the tick.  This must be identical to
 * vPortYield() from the call to vTaskSwitchContext() onwards.  The only
 * difference from vPortYield() is the tick count is incremented as the
 * call comes from the tick ISR.
 */
void vPortYieldFromTick(void) __attribute__((naked));

void vPortYieldFromTick(void) {
    portSAVE_CONTEXT();

    if (xTaskIncrementTick() != pdFALSE) {
        vTaskSwitchContext();
    }

    portRESTORE_CONTEXT();

    asm volatile ( "ret");
}
/*-----------------------------------------------------------*/

/*
 * Setup timer to generate a tick interrupt.
 */
static void prvSetupTimerInterrupt(void) {
    /* Configure low-power timer  used in tickless mode */
#if (configUSE_TICKLESS_IDLE == 1)
    RTC_INIT();
#endif

    /* TICK PIT is started by MCC generated code, hence FreeRTOS does not need 
     * to start it. */
}
/*-----------------------------------------------------------*/

#if (configUSE_PREEMPTION == 1)

/*
 * Tick ISR for preemptive scheduler.  We can use a naked attribute as
 * the context is saved at the start of vPortYieldFromTick().  The tick
 * count is incremented after the context is saved.
 */
void rtc_pit_callback(void) {
    vPortYieldFromTick();
}

#if 0 // this interrupt is handled by MCC auto generated code.

ISR(TICK_INT_vect, ISR_NAKED) {
    /* Clear tick interrupt flag. */
    CLR_INT(INT_FLAGS, INT_MASK);

    vPortYieldFromTick();

    asm volatile ( "reti");
}
#endif

#else  /* if configUSE_PREEMPTION == 1 */

/*
 * Tick ISR for the cooperative scheduler.  All this does is increment the
 * tick count.  We don't need to switch context, this can only be done by
 * manual calls to taskYIELD();
 */
ISR(TICK_INT_vect) {
    /* Clear tick interrupt flag. */
    INT_FLAGS = INT_MASK;
    xTaskIncrementTick();
}
#endif /* if configUSE_PREEMPTION == 1 */

#if (configUSE_TICKLESS_IDLE == 1)

volatile uint32_t RTC_OVF_Count = 0;

ISR(RTC_CNT_vect) {
    if (RTC.INTFLAGS & RTC_OVF_bm) {
        RTC_OVF_Count += 0x00010000;
        RTC.INTFLAGS = (RTC_OVF_bm);
    }

    if (RTC.INTFLAGS & RTC_CMP_bm) {
        RTC.INTFLAGS = (RTC_CMP_bm);
        //Disable compare interrupt
        RTC.INTCTRL &= (0xFF ^ RTC_CMP_bm);
    }

}

static uint32_t ulGetExternalTime(void) {
    uint32_t time_rtc;

    while (RTC.STATUS & RTC_CNTBUSY_bm) {
        ;
    }
    time_rtc = RTC.CNT;
    time_rtc += RTC_OVF_Count;
    return time_rtc;
}

static void vSetWakeTimeInterrupt(uint16_t xExpectedIdleTime) {
    uint32_t rtc_cnt_time;

    /* compute the required  */
    rtc_cnt_time = RTC_TICKS_TO_COUNTS(xExpectedIdleTime);
    rtc_cnt_time += ulGetExternalTime();

    while (RTC.STATUS & RTC_CMPBUSY_bm) {
        ;
    }
    RTC.CMP = (rtc_cnt_time & 0xFFFF);

    //Enable compare interrupt
    RTC.INTCTRL |= RTC_CMP_bm;
}

/* Define the function that is called by portSUPPRESS_TICKS_AND_SLEEP(). */
__attribute__((weak)) void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime) {
    eSleepModeStatus eSleepStatus;
    uint32_t ulLowPowerTimeBeforeSleep, ulLowPowerTimeAfterSleep;

    /* Read the current time from a time source that will remain operational
    while the microcontroller is in a low power state. */
    ulLowPowerTimeBeforeSleep = ulGetExternalTime();

    /* Stop the timer that is generating the tick interrupt. */
    TICK_TMR_STOP();

    /* Enter a critical section that will not effect interrupts bringing the MCU
    out of sleep mode. */
    portDISABLE_INTERRUPTS();

    /* Ensure it is still ok to enter the sleep mode. */
    eSleepStatus = eTaskConfirmSleepModeStatus();

    if (eSleepStatus == eAbortSleep) {
        /* A task has been moved out of the Blocked state since this macro was
         * executed, or a context switch is being held pending.  Do not enter a
         * sleep state.  Restart the tick and exit the critical section. */
        TICK_TMR_START();
        portENABLE_INTERRUPTS();
    } else {
        if (eSleepStatus == eNoTasksWaitingTimeout) {
            /* A user definable macro that allows application code to be inserted
             * here.  Such application code can be used to minimize power consumption
             * further by turning off IO, peripheral clocks, the Flash, etc. */
            configPRE_PWR_DOWN_PROCESSING();

            /* There are no running state tasks and no tasks that are blocked with a
             * time out.  Assuming the application does not care if the tick time slips
             * with respect to calendar time then enter a deep sleep that can only be
             * woken by (in this demo case) the user button being pushed on the
             * Curiosity Nano board.  If the application does require the tick time
             * to keep better track of the calender time then the PIT peripheral can be
             * used to make rough adjustments. */
            portSET_MODE_AND_SLEEP(SLEEP_MODE_PWR_DOWN);

            /* A user definable macro that allows application code to be inserted
             * here.  Such application code can be used to reverse any actions taken
             * by the configPRE_STOP_PROCESSING() */
            configPOST_PWR_DOWN_PROCESSING();
        } else {
            /* Configure an interrupt to bring the microcontroller out of its low
             * power state at the time the kernel next needs to execute.  The
             * interrupt must be generated from a source that remains operational
             * when the microcontroller is in a low power state. */
            vSetWakeTimeInterrupt(xExpectedIdleTime);

            /* Allow the application to define some pre-sleep processing.  This is
             * the standard configPRE_SLEEP_PROCESSING() macro as described on the
             * FreeRTOS.org website. */
            configPRE_SLEEP_PROCESSING(xExpectedIdleTime);

            /* Enter the low power state. */
            portSET_MODE_AND_SLEEP(SLEEP_MODE_STANDBY);

            /* Determine how long the microcontroller was actually in a low power
             * state for, which will be less than xExpectedIdleTime if the
             * microcontroller was brought out of low power mode by an interrupt
             * other than that configured by the vSetWakeTimeInterrupt() call.
             * Note that the scheduler is suspended before
             * portSUPPRESS_TICKS_AND_SLEEP() is called, and resumed when
             * portSUPPRESS_TICKS_AND_SLEEP() returns.  Therefore no other tasks will
             * execute until this function completes. */
            ulLowPowerTimeAfterSleep = ulGetExternalTime();

            /* Allow the application to define some post sleep processing.  This is
             * the standard configPOST_SLEEP_PROCESSING() macro, as described on the
             * FreeRTOS.org website. 
             * It can be used to reverse the actions of configPRE_SLEEP_PROCESSING(), 
             * and in so doing, return the microcontroller back to its fully operational state */
            configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

            /* Correct the kernels tick count to account for the time the
             * microcontroller spent in its low power state. */
            vTaskStepTick(RTC_COUNTS_TO_TICKS(ulLowPowerTimeAfterSleep - ulLowPowerTimeBeforeSleep));
            //  vTaskStepTick(xExpectedIdleTime);

        }

        /* Exit the critical section - it might be possible to do this immediately
         * after the SET_MODE_AND_SLEEP calls. */
        portENABLE_INTERRUPTS();

        /* Restart the timer that is generating the tick interrupt. */
        TICK_TMR_START();
    }
}

#endif 

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
        StackType_t **ppxIdleTaskStackBuffer,
        uint32_t *pulIdleTaskStackSize) {

    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];


    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;


    *ppxIdleTaskStackBuffer = uxIdleTaskStack;


    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
        StackType_t **ppxTimerTaskStackBuffer,
        uint32_t *pulTimerTaskStackSize) {
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}