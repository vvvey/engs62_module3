/*
 * main.c -- A program to print a dot each time button 0 is pressed.
 *
 *  Some useful values:
 *  -- XPAR_AXI_GPIO_1_DEVICE_ID -- xparameters.h
 *  -- XPAR_FABRIC_GPIO_1_VEC_ID -- xparameters.h
 *  -- XGPIO_IR_CH1_MASK         -- xgpio_l.h (included by xgpio.h)
 */
#include <stdio.h>		/* getchar,printf */
#include <stdlib.h>		/* strtod */
#include <stdbool.h>		/* type bool */
#include <unistd.h>		/* sleep */
#include <string.h>

#include "platform.h"		/* ZYBO board interface */
#include "xil_types.h"		/* u32, s32 etc */
#include "xparameters.h"	/* constants used by hardware */

#include "gic.h"		/* interrupt controller interface */
#include "xgpio.h"		/* axi gpio interface */
#include "io.h"
#include "xttcps.h"
#include "led.h"
#include "ttc.h"

#include "xtmrctr.h"
#include "xparameters.h"

/* hidden private state */



void handler(u32 led_num) {
	led_toggle(led_num);
}


void timer_handler() {
	led_toggle(4);
}


#define TIMER_DEVICE_ID XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_COUNTER_0 0
#define TIMER_CLOCK_FREQ 111111111
#define PWM_PERIOD 20000
#define PWM_DUTY_CYCLE_HIGH 1500

int InitPwmTimer2(XTmrCtr *TimerInstancePtr) {
    int Status;

    Status = XTmrCtr_Initialize(TimerInstancePtr, TIMER_DEVICE_ID);

    u32 PeriodResetValue = (TIMER_CLOCK_FREQ / 1000000) * PWM_PERIOD;
    u32 DutyCycleResetValue = (TIMER_CLOCK_FREQ / 1000000) * PWM_DUTY_CYCLE_HIGH;

    XTmrCtr_SetResetValue(TimerInstancePtr, TIMER_COUNTER_0, PeriodResetValue);
    XTmrCtr_SetResetValue(TimerInstancePtr, TIMER_COUNTER_0 + 1, DutyCycleResetValue);

    XTmrCtr_SetOptions(TimerInstancePtr, TIMER_COUNTER_0, XTC_PWM_ENABLE_OPTION);
}

int main() {
	XTmrCtr TimerInstance2;

  init_platform();				
  gic_init();
  led_init();

  io_sw_init(handler);
  io_btn_init(handler);
  ttc_init(1, timer_handler);

  ttc_start();

  int Status;
  Status = InitPwmTimer2(&TimerInstance2);
  XTmrCtr_Start(&TimerInstance2, TIMER_COUNTER_0);


  setvbuf(stdin,NULL,_IONBF,0);
  printf("[hello]\n"); /* so we are know its alive */
  while (1) {
  	   int ch;

  	   int index = 0;
  	   char input[100];

  	   printf(">");
  	   fflush(stdout);

  	   while ((ch = getchar()) != '\r' && ch != EOF ) {
  		   input[index] = ch;
  		   printf("%c", ch);
  		   fflush(stdout);

  		   index++;
  	   }
  	   input[index] = '\0';
  	   printf("\n");
  	   fflush(stdout);

  	   if (strcmp(input, "q") == 0) {
  		   break;
  	   }

     	   if (index == 1) {

     		   char *ep;
     		   long value = strtol(input, &ep,10);
     		   if (*ep == '\0' && value >=0 && value <= 3) {

     			   switch (value) {
     			   case 0:
     				   led_toggle(0);
     				   break;
     			   case 1:
     				   led_toggle(1);
     				   break;
     			   case 2:
     				   led_toggle(2);
     				   break;
     			   case 3:
     				   led_toggle(3);
     				   break;
     			   }

     			   if (led_get(value) == true) {
     				   printf("[%ld on]\n", value);
     			   } else {
     				   printf("[%ld off]\n", value);
     			   }
     			 fflush(stdout);

     		   }  else {
     			   printf("\n");
     		   }

     	   } else {
     		   printf("\n");
     	   }
     	   fflush(stdout);
     }

  printf("\n[done]\n");

  ttc_stop();

  io_sw_close();
  io_btn_close();
  ttc_close();

  gic_close();
  /* cleanup the hardware platform */
  cleanup_platform();

  return 0;
}

