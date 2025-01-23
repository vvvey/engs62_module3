/*
 * ttc.c
 *
 * NOTE: The TTC hardware must be enabled (Timer 0 on the processing system) before it can be used!!
 *
 */

#include <stdio.h>
#include "xttcps.h"
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */


static XTtcPs timer;
static XTtcPs_Config *dev_config;
static void (*timer_callback)(void);

void ttc_handler(void *devicep) {
	XTtcPs *timerIns = (XTtcPs *)devicep;
	XTtcPs_ClearInterruptStatus(timerIns, 1);

	timer_callback();
	XTtcPs_EnableInterrupts(timerIns, 1);
}
/*
 * ttc_init -- initialize the ttc freqency and callback
 */
void ttc_init(u32 freq, void (*ttc_callback)(void)) {
	timer_callback = ttc_callback;
  u16 interval;
  u8 prescaler;

  dev_config = XTtcPs_LookupConfig(XPAR_XTTCPS_0_DEVICE_ID);
  if (dev_config == NULL) {
	  printf("Timer configuration not found\n");
	  return XST_FAILURE;
  }

  if (XTtcPs_CfgInitialize(&timer, dev_config, dev_config->BaseAddress) != XST_SUCCESS) {
	  printf("Timer initialization failed\n");
	  return XST_FAILURE;
  }

  XTtcPs_CalcIntervalFromFreq(&timer, freq, &interval, &prescaler);
  printf("Interval: %u, Prescaler: %u\n", interval, prescaler);

  XTtcPs_SetInterval(&timer, interval);
  XTtcPs_SetPrescaler(&timer, prescaler);
  XTtcPs_SetOptions(&timer, XTTCPS_OPTION_INTERVAL_MODE);

  XTtcPs_EnableInterrupts(&timer, XTTCPS_IXR_INTERVAL_MASK);
  gic_connect(XPAR_XTTCPS_0_INTR, ttc_handler, &timer);


}

/*
 * ttc_start -- start the ttc
 */
void ttc_start(void) {
	 XTtcPs_Start(&timer);
}

/*
 * ttc_stop -- stop the ttc
 */
void ttc_stop(void) {
	XTtcPs_Stop(&timer);
}

/*
 * ttc_close -- close down the ttc
 */
void ttc_close(void) {
	gic_disconnect(XPAR_XTTCPS_0_INTR);
}
