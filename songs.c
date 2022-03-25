// Notes with resolution of 0.00001
#define A3 220.3
#define AS3 232.6
#define B3 247.5
#define C4 261.8
#define CS4 277.8
#define D4 294.1
#define DS4 310.6
#define E4 328.9
#define F4 349.7
#define FS4 370.4
#define G4 390.6
#define GS4 416.7
#define A4 438.6
#define AS4 467.3
#define B4 495.0
#define C5 520.8
#define CS5 555.6
#define D5 588.2
#define DS5 625.0
#define E5 657.9
#define F5 694.4
#define FS5 735.3
#define G5 781.3
#define GS5 833.3
#define A5 877.2
#define AS5 925.9
#define B5 980.4
#define C6 1041.7
#define CS6 1111.1
#define D6 1162.8
#define DS6 1250.0
#define E6 1315.8
#define F6 1388.9
#define FS6 1470.6
#define G6 1562.5
#define GS6 1666.7


#define W 1 // WHOLE
#define Z 0.875 // HALF + DOTTED QUARTER
#define X 0.75 // DOTTED HALF
#define C 0.625 // QUARTER + DOTTED QUARTER
#define H 0.5 // HALF
#define V 0.375 // DOTTED QUARTER
#define B 0.3125 // DOTTED EIGTH + EIGTH
#define Q 0.25 // QUARTER
#define N 0.1875 // DOTTED EIGTH
#define E 0.125 // EIGTH
#define S 0.0625 // SIXTEENTH


// 4 notes
const struct note WIN[] = {
	{C5, E},
	{C5, S},
	{C5, S},
	{G5, Q},	
};

// 3 notes
const struct note LOSE[] = {
	{C5,Q},
	{B4,Q},
	{GS4,H}	
};