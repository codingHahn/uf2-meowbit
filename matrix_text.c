#include "support.h"
#include "alphabet.h"
#include <stdint.h>

// The alphabet encoded columnswise from left to right where the
// MSB is ignored. The characters are 5 pixels in width and 7
// pixels in height
//
// Some are uppercase while other are lowercase. This is done for
// "asthetics". I did not manage to draw a good looking uppercase W

uint8_t *text_to_buffer(char *text, uint8_t *buffer)
{
	uint8_t *offsetptr = buffer;

	for (uint8_t i = 0; i < strlen(text); i++, offsetptr += 6) {
		switch (text[i]) {
		case 'a':
			memcpy(offsetptr, A, sizeof(uint8_t) * 5);
			break;

		case 'b':
			memcpy(offsetptr, B, sizeof(uint8_t) * 5);
			break;

		case 'c':
			memcpy(offsetptr, C, sizeof(uint8_t) * 5);
			break;

		case 'd':
			memcpy(offsetptr, D, sizeof(uint8_t) * 5);
			break;

		case 'e':
			memcpy(offsetptr, E, sizeof(uint8_t) * 5);
			break;

		case 'f':
			memcpy(offsetptr, F, sizeof(uint8_t) * 5);
			break;

		case 'g':
			memcpy(offsetptr, G, sizeof(uint8_t) * 5);
			break;

		case 'h':
			memcpy(offsetptr, H, sizeof(uint8_t) * 5);
			break;

		case 'i':
			memcpy(offsetptr, I, sizeof(uint8_t) * 5);
			break;

		case 'j':
			memcpy(offsetptr, J, sizeof(uint8_t) * 5);
			break;

		case 'k':
			memcpy(offsetptr, K, sizeof(uint8_t) * 5);
			break;

		case 'l':
			memcpy(offsetptr, L, sizeof(uint8_t) * 5);
			break;

		case 'm':
			memcpy(offsetptr, M, sizeof(uint8_t) * 5);
			break;

		case 'n':
			memcpy(offsetptr, N, sizeof(uint8_t) * 5);
			break;

		case 'o':
			memcpy(offsetptr, O, sizeof(uint8_t) * 5);
			break;

		case 'p':
			memcpy(offsetptr, P, sizeof(uint8_t) * 5);
			break;

		case 'q':
			memcpy(offsetptr, Q, sizeof(uint8_t) * 5);
			break;

		case 'r':
			memcpy(offsetptr, R, sizeof(uint8_t) * 5);
			break;

		case 's':
			memcpy(offsetptr, S, sizeof(uint8_t) * 5);
			break;

		case 't':
			memcpy(offsetptr, T, sizeof(uint8_t) * 5);
			break;

		case 'u':
			memcpy(offsetptr, U, sizeof(uint8_t) * 5);
			break;

		case 'v':
			memcpy(offsetptr, V, sizeof(uint8_t) * 5);
			break;

		case 'w':
			memcpy(offsetptr, W, sizeof(uint8_t) * 5);
			break;

		case 'x':
			memcpy(offsetptr, X, sizeof(uint8_t) * 5);
			break;

		case 'y':
			memcpy(offsetptr, Y, sizeof(uint8_t) * 5);
			break;

		case 'z':
			memcpy(offsetptr, Z, sizeof(uint8_t) * 5);
			break;

		default:
			memset(offsetptr, 0, sizeof(uint8_t) * 5);
		}
	}

	return buffer;
}
