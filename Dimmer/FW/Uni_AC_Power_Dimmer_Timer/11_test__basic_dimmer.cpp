
// Test with:
// ./waf debug_log --port=0

#include "common.hpp"

#define PERCENT 75
#define HZ 50

//////////////

#define PRESCALE 8
#define PRESCALE_CODE 0b010

//////////////
// Calculated

#define TOP (F_CPU/((HZ)*2*8))

#define THRESHOLD_A (TOP*(100-PERCENT)/100)
#define THRESHOLD_B (THRESHOLD_A+10)

//////////////

u8 second_half_period;

// Zero-cross start of period.
ISR(INT2_vect) {
	portc.b7 = !portc.b7;

	// Reset counter.
	second_half_period = 0;

	// Set trigger moment.
	tc1.ocra = THRESHOLD_A;
	tc1.ocrb = THRESHOLD_B;

	// Turn on IRQs.
	irq.ocie1a = irq.ocie1b = 1;

	// Reset counter
	tc1.tcnt = 0;
}

// Start of triac pulse.
ISR(TIMER1_COMPA_vect) {
	zct.drv3 = 1;
	portc.b6 = 1; // Mirror DRV3 for debug.
}

// End of triac pulse.
ISR(TIMER1_COMPB_vect) {
	zct.drv3 = 0;
	portc.b6 = 0; // Mirror DRV3 for debug.
}


// End of half-period.
ISR(TIMER1_CAPT_vect) {
	if(second_half_period){
		// Turn off IRQs after 2nd half-period,
		// so if there is not zero-cross
		// triac would not be triggered.
		irq.ocie1a = irq.ocie1b = 1;
	}
	second_half_period = true;
}


int main(void) {

	uart_stdio_blocking _u;
	printf("%s\n", __FILE__);


	printf("F_CPU = %ld\n", F_CPU);
	printf("TOP = %ld\n", TOP);
	printf("THRESHOLD_A = %ld\n", THRESHOLD_A);
	printf("THRESHOLD_B = %ld\n", THRESHOLD_B);

	// Just for debug.
	ddrc.b4 = DDR_OUT;
	ddrc.b5 = DDR_OUT;
	ddrc.b6 = DDR_OUT;
	ddrc.b7 = DDR_OUT;


	// DRV3 output.
	zct.odd_1 = DDR_OUT;
	zct.drv3 = 0;

	// Setup IRQs.

	// Zero-cross IRQ.
	// Set PB2 (INT2) as input
	zct.idd_1 = DDR_IN;

	// INT2 IRQ
	irq.int2 = 1;
	irq.isc2 = 0; // falling edge.

	// timer IRQs.
	irq.ticie1 = 1;
	// For not turn them off.
	irq.ocie1b = 0;
	irq.ocie1a = 0;
	
	// Enable global interrupts
	sei();

	tc1.tccra = tc1.tccrb = 0; // Reset.

	tc1.cs = PRESCALE_CODE;

	tc1.ocra = 0;
	tc1.ocrb = 0;
	tc1.icr = TOP; // Before setting mode.

	// CTC mode, 12.
	tc1.wgm3 = 1;
	tc1.wgm2 = 1;
	tc1.wgm1 = 0;
	tc1.wgm0 = 0;

	while(1){
	}

}
