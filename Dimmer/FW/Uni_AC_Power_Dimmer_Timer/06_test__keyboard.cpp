
// Test with:
// ./waf debug_uart --port=0

#include "common.hpp"

#include "avr_io_bitfields.h"
#include "uart_stdio.hpp"


struct bf_c {
	//TODO
	u8 r1 : 1;
	u8 r2 : 1;
	u8 r3 : 1;
	u8 r4 :	1;
	u8 c3 : 1;
	u8 c2 : 1;
	u8 c1 : 1;
	u8 c0 : 1;
};


struct bf_d {
	u8 rxd : 1;
	u8 txd : 1;
	u8 zc1 : 1;
	u8 zc2 : 1;
	u8 drv1 : 1;
	u8 drv2 : 1;
	u8 r0 : 1;
	u8 drv3 : 1;
};
#define o_buzz (*((volatile bf_d*)(&PORTD)))


int main(void) {
	screen.odd_0 = wide_bool(DDR_OUT);
	screen.odd_1 = wide_bool(DDR_OUT);
	screen.odd_2 = wide_bool(DDR_OUT);
	screen.odd_3 = wide_bool(DDR_OUT);

	//screen.rgb0 = screen.rgb1 = WHITE;

	DDRC = bf_to_u8<bf_c>({
			.r1 = 0,
			.r2 = 0,
			.r3 = 0,
			.r4 = 0,
			.c3 = 1,
			.c2 = 1,
			.c1 = 1,
			.c0 = 1
		});

	PORTC = bf_to_u8<bf_c>({
			.r1 = 1,
			.r2 = 1,
			.r3 = 1,
			.r4 = 1,
			.c3 = 0,
			.c2 = 0,
			.c1 = 0,
			.c0 = 0
		});

	// 0 - in
	// 1 - out
	DDRD = bf_to_u8<bf_d>({
		.rxd = 0,
		.txd = 1,
		.zc1 = 0,
		.zc2 = 0,
		.drv1 = 1,
		.drv2 = 1,
		.r0 = 0,
		.drv3 = 1
	});
	// For ins:
	// 1 - pull up
	PORTD = bf_to_u8<bf_d>({
		.rxd = 0,
		.txd = 1,
		.zc1 = 0,
		.zc2 = 0,
		.drv1 = 0,
		.drv2 = 0,
		.r0 = 1,
		.drv3 = 0
	});

	DDRD &= ~(1 << 6);   // input
	PORTD |= (1 << 6);   // pull-up

	uart_stdio_non_blocking<100> _u;
	printf("Hello!\n\r");
	printf("Hello again!\n\r");
	printf("F_CPU = %ld\n\r", F_CPU);
	
	while(1){

		printf("PC: %02X PD: %02X\n\r", PINC, PIND);
		_delay_ms(200);

		for (int col = 0; col < 4; col++) {

        // 1. all columns HIGH (PC4–PC7)
        uint8_t base = 0x0F;        // rows pull-up (PC0–PC3 = 1)
		uint8_t cols = 0xF0;        // all columns HIGH

		cols &= ~(1 << (7 - col)); // FIXED order

		PORTC = base | cols;
        _delay_us(5);

        uint8_t pc = PINC;
        uint8_t pd = PIND;

        // --- R0 (PD6) ---
        if (!(pd & (1 << 6))) {
            printf("R0 C%d\n\r", col);
        }

        // --- R1–R4 (PC0–PC3) ---
        for (int row = 0; row < 4; row++) {
            if (!(pc & (1 << row))) {
                printf("R%d C%d\n\r", row + 1, col);
            }
        }
    }

	} // while

}
