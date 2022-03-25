#include <stdio.h>

struct note {
	int freq;
	double duration;
};

void Playnote (const struct note n) {
	int freq = n.freq;
	double dur = n.duration;
	
	// Calculate values for oscillation of PORTB voltage
	int i;
	double k;

	double p = 1.0 / freq;
	k = dur / p;
	unsigned short TH = p / 2 * 100000;

	// Oscillate PORTB voltage
	for (i = 0; i < k; ++i) {
		SET_BIT(PORTB, 3);
		avr_wait_HIRES(TH);
		CLR_BIT(PORTB, 3);
		avr_wait_HIRES(TH); // TL == TH
	}
}

void Playsong(const struct note *song, int N) {
	int i;	
	for (i = 0; i < N; ++i) {
		Playnote(song[i]);
	}
}