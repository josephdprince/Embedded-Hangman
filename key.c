int get_key();
int is_pressed();

int get_key() {
	int i, j;
	for(i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			if (is_pressed(i, j)) {
				return i*4+j+1;
			}
		}
	}
	return 0;
}

int is_pressed(int r, int c) {
	// Set all pins to N/C
	DDRC = 0;
	PORTC = 0;
	
	// Set r to 0 (DDR 1 PORT 0)
	SET_BIT(DDRC, r);
	CLR_BIT(PORTC, r);
	
	// Set c to w1 (DDR 0 PORT 1)
	CLR_BIT(DDRC, c+4);
	SET_BIT(PORTC, c+4);
	
	// Check to see if button was pressed
	avr_wait(1);
	if (GET_BIT(PINC, c+4) == 0) {
		return 1;
	}
	return 0;
}