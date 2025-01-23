/*
 * io.h -- switch and button module interface
 *
 */
#pragma once

#include <stdio.h>			/* printf for errors */
#include <stdbool.h>
#include <xgpio.h>		  	/* axi gpio */
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */

#include "gic.h"		/* interrupt controller interface */


#include "io.h"

#include <stdio.h>		/* getchar,printf */
#include <stdlib.h>		/* strtod */
#include <stdbool.h>		/* type bool */
#include <unistd.h>		/* sleep */
#include <string.h>

static XGpio btnport;
static XGpio swport;

static void (*button_callback)(u32 led_num);
static void (*switch_callback)(u32 led_num);



/*
 * controll is passed to this function when a button is pushed
 *
 * devicep -- ptr to the device that caused the interrupt
 */
void btn_handler(void *devicep) {
	/* coerce the generic pointer into a gpio */
	XGpio *dev = (XGpio*)devicep;

	XGpio_InterruptClear(dev, 1);
	u32 btn_output = XGpio_DiscreteRead(dev, 1);

	u32 btn0_mask = 0b1;
	u32 btn1_mask = 0b10;
	u32 btn2_mask = 0b100;
	u32 btn3_mask = 0b1000;

	if ((btn_output & btn0_mask) == btn0_mask) {
		button_callback(0);
	}

	if ((btn_output & btn1_mask) == btn1_mask) {
		button_callback(1);
		}

	if ((btn_output & btn2_mask) == btn2_mask) {
		button_callback(2);
		}

	if ((btn_output & btn3_mask) == btn3_mask) {
		button_callback(3);
		}


	fflush(stdout);
	XGpio_InterruptEnable(dev, 1);
}

static u32 swState;

void sw_handler(void *devicep) {

	XGpio *dev = (XGpio*)devicep;

	XGpio_InterruptClear(dev, 1);
	u32 sw_output = XGpio_DiscreteRead(dev, 1);

	u32 sw0_mask = 0b1;
	u32 sw1_mask = 0b10;
	u32 sw2_mask = 0b100;
	u32 sw3_mask = 0b1000;

	if ((sw_output & sw0_mask) != (swState & sw0_mask)) {
		switch_callback(0);
	}

	if ((sw_output & sw1_mask) != (swState & sw1_mask)) {
		switch_callback(1);
	}

	if ((sw_output & sw2_mask) != (swState & sw2_mask)) {
		switch_callback(2);
		}

	if ((sw_output & sw3_mask) != (swState & sw3_mask)) {
		switch_callback(3);
		}


	swState = XGpio_DiscreteRead(dev, 1);
	XGpio_InterruptEnable(dev, 1);
}


/*
 * initialize the btns providing a callback
 */
void io_btn_init(void (*btn_callback)(u32 btn)) {
	button_callback = btn_callback;

    /* initialize btnport (c.f. module 1) and immediately dissable interrupts */
    XGpio_Initialize(&btnport, XPAR_AXI_GPIO_1_DEVICE_ID);
    XGpio_SetDataDirection(&btnport, 1, 0xF);
    swState = XGpio_DiscreteRead(&btnport, 1);

    XGpio_InterruptDisable(&btnport, 1);

    /* connect handler to the gic (c.f. gic.h) */
    gic_connect(XPAR_FABRIC_GPIO_1_VEC_ID, btn_handler, &btnport);

    /* enable interrupts on channel (c.f. table 2.1) */
    XGpio_InterruptEnable(&btnport, 1);

    /* enable interrupt to processor (c.f. table 2.1) */
    XGpio_InterruptGlobalEnable(&btnport);
}

/*
 * close the btns
 */
void io_btn_close(void) {
	/* disconnect the interrupts (c.f. gic.h) */
	gic_disconnect(XPAR_FABRIC_GPIO_1_VEC_ID);
}


/*
 * initialize the switches providing a callback
 */
void io_sw_init(void (*sw_callback)(u32 sw)) {
	switch_callback = sw_callback;

    XGpio_Initialize(&swport, XPAR_AXI_GPIO_2_DEVICE_ID);
    XGpio_SetDataDirection(&swport, 1, 0xF);
    XGpio_InterruptDisable(&swport, 1);

    gic_connect(XPAR_FABRIC_GPIO_2_VEC_ID, sw_handler, &swport);

    XGpio_InterruptEnable(&swport, 1);

    XGpio_InterruptGlobalEnable(&swport);
}

/*
 * close the switches
 */
void io_sw_close(void) {
	gic_disconnect(XPAR_FABRIC_GPIO_2_VEC_ID);
}
