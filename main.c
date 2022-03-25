/*
* Project 5.c
*
* Created: 2/26/2022 10:20:17 AM
* Author : Joseph
*/

// TODO LIST:

// BUG LIST:

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "avr.c"
#include "lcd.c"
#include "key.c"
#include "note.c"
#include "songs.c"
#include "AnswerBank.c"

int gameMode(int*);
int getAnswer(char*);
int validateAnswer(char*, int);
char collectKeypadInput(int, int, int);
void printUnderscores(char*, int);
int play(char*, int);
void updateUnderscores(char*, int, char);
int findNumDistinctElements(char*, int);

int NUM_IN_BANK = 7;
const int keyMapping[] = {0, 1, 2, 3, 16, 4, 5, 6, 11, 7, 8, 9, 14, 13, 10, 15, 17};
const char mapping[10][4] = {{'1', '1', '1', '1'},  // 1
							 {'A', 'B', 'C', '2'},  // 2
							 {'D', 'E', 'F', '3'},  // 3
							 {'G', 'H', 'I', '4'},  // 4
							 {'J', 'K', 'L', '5'},  // 5
							 {'M', 'N', 'O', '6'},  // 6
							 {'P', 'R', 'S', '7'},  // 7
							 {'T', 'U', 'V', '8'},  // 8
							 {'W', 'X', 'Y', '9'},  // 9
							 {'0', '0', '0', '0'}}; // 0
												    // 13 -> Q
													// 15 -> Z

int main(void) {
	avr_init();
	lcd_init();
	lcd_clr();

	char* answer;
	int size;

	int mode;
	int seed = 0;
	int* seedPtr = &seed;
	char gameWon[] = "You Win!";
	char gameLost[] = "You Lose";
	char promptInput1[] = "Please enter";
	char promptInput2[] = "your phrase";

	// Game loop
	while (1) {
		// Get input from user if they want to do single player or multi player
		mode = gameMode(seedPtr);

		// if single player, choose a word from test bank and store answer
		if (mode == 1) {
			srand(seed);
			int randInt = rand() % NUM_IN_BANK;
			answer = bank[randInt].words;
			size = bank[randInt].size;
		}

		// else, take input and store that as answer
		else {
			lcd_pos(0,3);
			lcd_puts2(promptInput1);
			lcd_pos(1,3);
			lcd_puts2(promptInput2);
			avr_wait(2500);
			
			char wordArr[32];
			answer = wordArr;
			
			size = getAnswer(wordArr);
		}

		// Underscores for the word
		printUnderscores(answer, size);

		// Play the game
		int win = play(answer, size);
		
		lcd_clr();
		lcd_pos(0,4);
		
		SET_BIT(DDRB, 3);
		if (!win) {
			lcd_puts2(gameLost);
			Playsong(LOSE, 3);
		}
		else {
			lcd_puts2(gameWon);
			Playsong(WIN, 4);
		}
		CLR_BIT(DDRB, 3);
		avr_wait(2000);
	}
}

int gameMode(int* seed) {
	char m1[] = "Hangman";
	char m2[] = "1P/2P:";
	int k;
	
	lcd_clr();
	lcd_pos(0,5);
	lcd_puts2(m1);
	lcd_pos(1,5);
	lcd_puts2(m2);

	k = get_key();
	while (k != 1 && k != 2) {
		k = get_key();
		*seed += 1;
	}

	lcd_pos(1,12);
	lcd_put(k+48);
	avr_wait(800);

	return k;
}

// '_' means add a space and ' ' means done guessing
int getAnswer(char* wordArr) {
	lcd_clr();
	
	int row = 0;
	int column = 0;
	int len = 0;
	
	char letter = '!';
	while (letter != ' ') {
		letter = collectKeypadInput(row, column, 0);
		
		// Make sure first input is not a space, backspace, or end game
		if ((len == 0) && (letter == ' ' || letter == '_' || letter == '<')) {
			letter = '!';
		}
		
		// Update answer array
		if (letter != '!') {
			// If backspace
			if (letter == '<') {
				lcd_pos(row, column);
				lcd_put(' ');
				column--;
				len--;
				
				// If need to go back a row
				if (column == -1) {
					row = 0;
					column = 15;
				}
				
				lcd_pos(row, column);
				lcd_put(' ');
			}
			else { 
				if (letter != ' ') {
					wordArr[len] = letter;
					len++;
			
					// Move column and row
					++column;
					if (column == 16) {
						// Move down a row
						if (row == 0) {
							row = 1;
							column = 0;
						}
						// End of input
						else {
							letter = ' ';
							
						}
					}
				}
				if (letter == ' ') {
					// INPUT CHECK
					// At this point we have the answer array we are looking for. Now we just want to make sure it is formatted properly
					// Go through answer and replace underscores with spaces
					for (int i = 0; i < len; ++i) {
						if (wordArr[i] == '_') {
							wordArr[i] = ' ';
						}
					}
				
					// Go through answer and make sure it does not end in space
					char temp = wordArr[len - 1];
					while (temp == ' ') {
						len--;
						temp = wordArr[len - 1];
					}
				
					// If not valid, restart the input
					if (!validateAnswer(wordArr, len)) {
						len = 0;
						row = 0;
						column = 0;
						letter = '!';
						char m1[] = "Invalid Input";
						lcd_clr();
						lcd_pos(0,1);
						lcd_puts2(m1);
						avr_wait(500);
						lcd_clr();
					}
				}
			}
		}
	}
	return len;
}

// Answer needs to be validated to make sure that it can fit on the screen
// such that a single word does not occupy two different rows
int validateAnswer(char* answer, int size) {
	if (size <= 16) {
		return 1;
	}
	
	int beg;
	for (int i = 0; i < size; ++i) {
		beg = i;
		while (answer[i] != ' ' && i != size ) {
			++i;
		}
		
		// When this happens we have a word that has crossed into the next row
		if (i > 16) {
			if (size - beg > 12) {
				return 0;
			}
			else {
				return 1;
			}
		}
	}
	// Control should not reach here
	return 0;
}

char collectKeypadInput(int row, int column, int key) {
	int k;
	int prev = 0;
	int prevK = 0;
	int count = 0;
	int gotInput = 0;

	char input = 1;

	while (!gotInput) {
		
		k = get_key();
		k = keyMapping[k];
		
		// key is used to check when this function is called. We do not want user to enter k == 14 or 17 while playing (only when providing word)
		if (key == 1) {
			if (k == 11 || k == 14 || k == 17) {
				k = 0;
			}
		}

		// If pressed an invalid key do not move on
		if (k != 0 && k != prevK) {
			if (k == 16) {
				if (input != 1) {
					gotInput = 1;
				}
			}
			else if (k == 11) {
				input = '<';
			}
			else if (k == 13) {
				input = 'Q';
			}
			else if (k == 15) {
				input = 'Z';
			}
			else if (k == 14) {
				input = '_';
			}
			else if (k == 17) {
				input = ' ';
				gotInput = 1;
			}
			else {
				// If user presses a different key then count needs to be reset
				if (prev == k) {
					count++;
					count = count % 4;
				}
				else {
					count = 0;
				}

				input = mapping[k - 1][count];
				prev = k;
			}

			// Output the input to screen
			if (input != 1) {
				lcd_pos(row,column);
				lcd_put(input);
			}
		}
		prevK = k;
	}
	
	return input;
}

void printUnderscores(char* answer, int size) {
	int bottomRow = 0;
	int currWordSize = 0;
	int loc = 0;
	int outputKey = 0;
	
	lcd_clr();

	// Iterate through the answer to output underscores
	for (int i = 0; i < size; ++i) {
		// If string location is not a space
		if (answer[i] != 32) {
			++currWordSize;
		}
		else {
			outputKey = 1;
		}

		// If output is set or at end of word
		if (outputKey || i == size - 1) {
			// If we need to move down a row
			if (currWordSize + loc > 16) {
				bottomRow = 1;
				if (loc < 16) {
					loc = 0;
				}
				else {
					loc = loc % 16;
				}
			}

			for (int j = 0; j < currWordSize; ++j) {
				lcd_pos(bottomRow, loc);
				lcd_put(95);
				++loc;
			}
			++loc;
			currWordSize = 0;
		}
		outputKey = 0;
	}
}

int play(char* answer, int size) {
	int lives = 6;
	int win	= 0;
	char guess;
	
	int correctGuesses = 0;
	int distinctElements = findNumDistinctElements(answer, size);
	
	char guesses[32];
	int guessesSize = 0;
	
	lcd_pos(1,15);
	lcd_put(lives + 48);

	// While game is not won and there are still lives left	
	while (lives > 0 && win != 1) {
		int alreadyGuessed = 0;
		
		guess = collectKeypadInput(1, 13, 1);
		
		// Check if this guess was already used
		for (int i = 0; i < guessesSize; ++i) {
			if (guess == guesses[i]) {
				alreadyGuessed = 1;
				break;
			}
		}
		
		// Don't move forward if already guessed
		if (!alreadyGuessed) {
			// Put in guessed vector
			guesses[guessesSize] = guess;
			++guessesSize;
			
			// Check if valid guess
			int foundGuess = 0;
			for (int i = 0; i < size; ++i) {
				if (guess == answer[i]) {
					++correctGuesses;
					foundGuess = 1;
					updateUnderscores(answer, size, guess);
					break;
				}
			}
			
			// If invalid guess then decrement a life
			if (!foundGuess) {
				lives -= 1;
				lcd_pos(1,15);
				lcd_put(lives + 48);
			}
			// Check if won
			else {
				if (correctGuesses == distinctElements) {
					win = 1;
					avr_wait(1000);
				}
			}
		}
	}
	return win;
}

void updateUnderscores(char* answer, int size, char guess) {
	int bottomRow = 0;
	int ansPos = 0;
	int lcdPos = 0;
	int wordSize;
	
	for (int i = 0; i < size; ++i) {
		ansPos = i;
		int guessInWord = 0;
		
		while (answer[i] != ' ' && i != size) {
			if (answer[i] == guess) {
				guessInWord = 1;
			}
			i++;
		}
		wordSize = i - ansPos + 1;
		
		if (i > 16 && !bottomRow) {
			// Special case where last word ends on the last column of row 0
			if (answer[16] == ' ') {
				lcdPos = 1;
			}
			else {
				lcdPos = 0;
			}
			bottomRow = 1;
		}
		
		// At this point the row and column are matched up so we want to update a valid word
		if (guessInWord) {
			while (ansPos != i) {
				if (answer[ansPos] == guess) {
					lcd_pos(bottomRow, lcdPos);
					lcd_put(guess);
				}
				ansPos++;
				lcdPos++;
			}
			lcdPos++;
		}
		else {
			lcdPos = lcdPos + wordSize;
		}
	}

}

int findNumDistinctElements(char* answer, int size) {
	int numElements[36] = {0};
	int count = 0;
	
	// Loop through guess list and keep track an int count of each char we find
	for (int i = 0; i < size; ++i) {
		char temp = answer[i];
		if (temp > 47 && temp < 58) {
			numElements[temp - 48]++;
		}
		else {
			numElements[temp - 55]++;
		}
	}
	
	// Loop through our newly generated numElements array and count how many places have a value larger than 1
	for (int i = 0; i < 36; ++i) {
		if (numElements[i] > 0) {
			++count;
		}
	}
	
	return count;
}