#include <stdio.h>
#include "xtmrctr.h"
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */


#define TIMER_DEVICE_ID XPAR_TMRCTR_0_DEVICE_ID
#define SYSTEM_CLK XPAR_TMRCTR_0_CLOCK_FREQ_HZ // 50MHz
#define PWM_FREQ 50 // 50Hz --> 20 ms


static XTmrCtr TimerInstancePtr;
/*
 * Initialize the servo, setting the duty cycle to 7.5%
 */
void servo_init(void) {
// Init
  XTmrCtr_Initialize(&TimerInstancePtr, TIMER_DEVICE_ID);

  XTmrCtr_Stop(&TimerInstancePtr, 0);
  XTmrCtr_Stop(&TimerInstancePtr, 1);

  // Clear values
  XTmrCtr_SetResetValue(&TimerInstancePtr, 0, 0);
  XTmrCtr_SetResetValue(&TimerInstancePtr, 1, 0);

}

/*
 * Set the dutycycle of the servo
 */
void servo_set(double dutycycle) {

	  u32 pwm_period = SYSTEM_CLK / PWM_FREQ;
	  u32 pwm_high = (dutycycle * pwm_period)/100;

	XTmrCtr_Stop(&TimerInstancePtr, 0);
	XTmrCtr_Stop(&TimerInstancePtr, 1);

	XTmrCtr_SetOptions(&TimerInstancePtr, 0, XTC_PWM_ENABLE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_EXT_COMPARE_OPTION);
	XTmrCtr_SetOptions(&TimerInstancePtr, 1, XTC_PWM_ENABLE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_EXT_COMPARE_OPTION);

	XTmrCtr_SetResetValue(&TimerInstancePtr, 0, pwm_period);
	XTmrCtr_SetResetValue(&TimerInstancePtr, 1, pwm_high);

	XTmrCtr_Start(&TimerInstancePtr, 0);
	XTmrCtr_Start(&TimerInstancePtr, 1);

}

