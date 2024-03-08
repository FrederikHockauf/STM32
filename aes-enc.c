#include <hal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include <stdio.h>

// The number of AES rounds to be performed. Does not actually support 12 or 14 rounds currently
const int ROUNDS = 10;

// Declare fucntions related to reading and writing data to and from the computer
uint8_t Read4Bytes();
void Write4Bytes(uint8_t variable);
void ReadBlock(uint8_t block[4][4]);
void WriteBlock(uint8_t block[4][4]);

// Declare functions related to the functions (mostly) related to key expansion
void AESKeyExpansion(uint8_t key[4][4], uint8_t expandedKey[11][4][4]);
void FillRCon(uint8_t rCon[10][4]);
void BytewiseXOR(uint8_t operand1[4], uint8_t operand2[4], uint8_t result[4]);
void SBoxWord(uint8_t word[4]);
void SBoxBlock(uint8_t block[4][4]);

// Declare functions related to round operations
void ShiftRows(uint8_t block[4][4]);
void MixColumns(uint8_t block[4][4]);
void AddRoundKey(uint8_t block[4][4], uint8_t key[4][4]);
void GFMatrixMultiplication(uint8_t word[4]);
uint8_t GFDouble(uint8_t operand);
uint8_t GFTriple(uint8_t operand);

// The S-box written out
const uint8_t sBox[256] =
{
		0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
		0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
		0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
		0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
		0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
		0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
		0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
		0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
		0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
		0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
		0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
		0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
		0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
		0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
		0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
		0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

int main()
{
    // Setup the board and turn on the lights (I think)
	hal_setup();
    hal_led_on();

	// Declare and initialize the messge, encrypted message (that we'll transform), the key, and the expanded key
	uint8_t message[4][4];
	uint8_t encMessage[4][4];
	uint8_t key[4][4];
	uint8_t expandedKey[11][4][4];

	printf("From device - This works?\n");
	
	//uint8_t variable1 = Read4Bytes();
	//uint8_t variable2 = Read4Bytes();
	//uint8_t variable3 = Read4Bytes();
	//uint8_t variable4 = Read4Bytes();
  	//for (int i = 0; i < 4; i++) // Load the 4-byte int in byt bitshifting (8 times as 1 byte = 8 bit)
	//	variable = variable << 8 | hal_getchar();

	//uint8_t v1 = hal_getchar();
	//uint8_t v2 = hal_getchar();
	//uint8_t v3 = hal_getchar();
	//uint8_t v4 = hal_getchar();
	
	//uint8_t a = Read4Bytes;
	//printf("a = %u and %u and %u and %u \n", variable1, variable2, variable3, variable4);
	//printf("a = %u and %u and %u and %u \n", v1, v2, v3, v4);
	//uint8_t alle = Read4Bytes();
	//uint8_t alle2 = Read4Bytes();

    // Read the data from the computer
    ReadBlock(key);
    ReadBlock(message);

	printf("From device - Another check!\n");

	WriteBlock(key);
	WriteBlock(message);

	//printf("From device - Done readingn\n");
	hal_led_off();

	// Prepare for the round operations by expanding the key, readying the encryption message, and adding the 0th round key
	AESKeyExpansion(key, expandedKey);
	memcpy(encMessage, message, sizeof(encMessage));
	AddRoundKey(encMessage, expandedKey[0]);

	// Go through all the the rounds of AES
	for (int i = 1; i <= ROUNDS; i++)
	{
		// Apply the round operations
		SBoxBlock(encMessage);
		ShiftRows(encMessage);
		if (i != ROUNDS)
			MixColumns(encMessage);
		AddRoundKey(encMessage, expandedKey[i]);	
	}

    // Read the data from the computer
    WriteBlock(encMessage);
    hal_led_off();

    while (1)
    ;
    return 0;
}

uint8_t Read4Bytes()
{
  uint8_t variable = 0;

  for (int i = 0; i < 4; i++) // Load the 4-byte int in byt bitshifting (8 times as 1 byte = 8 bit)
      variable = variable << 8 | hal_getchar();
  
  return variable;
}

void Write4Bytes(uint8_t variable)
{
  for (int i = 0; i < 4; i++)
      hal_putchar((variable >> ((3-i)*8)) & 0xff);
}

void ReadBlock(uint8_t block[4][4])
{
    // Load the 4-byte int in byt bitshifting (8 times as 1 byte = 8 bit)
	for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            block[i][j] = hal_getchar();
}

void WriteBlock(uint8_t block[4][4])
{
    // Load the 4-byte int in byt bitshifting (8 times as 1 byte = 8 bit)
	for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
			hal_putchar(block[i][j]);
}

void AESKeyExpansion(uint8_t key[4][4], uint8_t expandedKey[11][4][4])
{
	// The key is used as the round 0 key in the expansion
	memcpy(expandedKey[0], key, sizeof(expandedKey[0]));

	// The round constants are calculated
	uint8_t rCon[10][4];
	FillRCon(rCon);

	// Round keys are generated for each round
	for (int round = 1; round <= ROUNDS; round++)
	{
		uint8_t transformKey[4];

		// Rotate word
		for (int i = 0; i < 4; i++)
			transformKey[i] = expandedKey[round - 1][3][(i + 1) % 4];

		// Substitution box
		for (int i = 0; i < 4; i++)
			transformKey[i] = sBox[transformKey[i]];

		// RCon XOR
		uint8_t roundProduct[4];
		BytewiseXOR(transformKey, rCon[round - 1], roundProduct);

		// Word-wise XOR with the transform key or previous word (if from same block / round)
		BytewiseXOR(expandedKey[round - 1][0], roundProduct, expandedKey[round][0]);
		for (int i = 1; i < 4; i++)
			BytewiseXOR(expandedKey[round - 1][i], expandedKey[round][i - 1], expandedKey[round][i]);
	}
}

void FillRCon(uint8_t rCon[10][4])
{
	// Create round constants for all rounds
	for (int i = 0; i < ROUNDS; i++)
	{
		// A branched function that calculates a value rc_i (for byte 0) for each round i. This value depends on the prior round's value
		if (i == 0)
			rCon[i][0] = 1;
		else
		{
			if (rCon[i - 1][0] < 0x80)
				rCon[i][0] = rCon[i - 1][0] * 2;
			else
				rCon[i][0] = (rCon[i - 1][0] * 2) ^ 0x11B;
		}

		// Only the first byte of the round constant will be non-zero. Initialized to avoid potential spill data
		rCon[i][1] = 0;
		rCon[i][2] = 0;
		rCon[i][3] = 0;
	}
}

void BytewiseXOR(uint8_t operand1[4], uint8_t operand2[4], uint8_t result[4])
{
	// XOR the the elements byte by byte
	for (int i = 0; i < 4; i++)
		result[i] = operand1[i] ^ operand2[i];
}

void SBoxWord(uint8_t word[4])
{
	// Substitute all elements in a word using an S-box
	for (int i = 0; i < 4; i++)
		word[i] = sBox[word[i]];
}

void SBoxBlock(uint8_t block[4][4])
{
	// Substitute all elements in a block, using an S-box, word by word
	for (int i = 0; i < 4; i++)
		SBoxWord(block[i]);
}

void ShiftRows(uint8_t block[4][4])
{
	// Go through all elements, except for those in the first row
	for (int row = 1; row < 4; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			// We choose a fixpoint and set the start position to be 0. Then we move the start in a direction as to avoid over fixpoint
			uint8_t val = ((4 - row) * col) % 4;
			uint8_t temp1 = block[row][row];
			uint8_t temp2 = block[val][row];

			block[row][row] = temp2;
			block[val][row] = temp1;

			// When the row is 2 we get two correct movings with one round of the loop, so it's better to quit after the first round and clean up the two remainders later
			if (row == 2)
				break;
		}
	}

	// Moving the two last positions from round 2
	uint8_t temp = block[1][2];
	block[1][2] = block[3][2];
	block[3][2] = temp;
}

void MixColumns(uint8_t block[4][4])
{
	// Mix up the columns of a block by polynomial multiplication, written as matrix multiplication. This is in the Galois Field 2^8 (GF)
	for (int i = 0; i < 4; i++)
		GFMatrixMultiplication(block[i]);
}

void AddRoundKey(uint8_t block[4][4], uint8_t key[4][4])
{
	// XOR each product of the block with the key
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			block[i][j] = block[i][j] ^ key[i][j];
}

void GFMatrixMultiplication(uint8_t word[4])
{
	// Duplicate the vector so we can write to the input parameter
	uint8_t vector[4];
	memcpy(vector, word, sizeof(vector));

	// The GF matrix multiplication with our word and the polynomial coefficient matrix is written out explicitly. In GF + = ^
	word[0] = GFDouble(vector[0]) ^ GFTriple(vector[1]) ^ vector[2] ^ vector[3];
	word[1] = vector[0] ^ GFDouble(vector[1]) ^ GFTriple(vector[2]) ^ vector[3];
	word[2] = vector[0] ^ vector[1] ^ GFDouble(vector[2]) ^ GFTriple(vector[3]);
	word[3] = GFTriple(vector[0]) ^ vector[1] ^ vector[2] ^ GFDouble(vector[3]);
}

uint8_t GFDouble(uint8_t operand)
{
	// Double the operand by bitshifting
	uint8_t result = operand << 1;

	// The Galois field is modulo x^8+x^4+x^3+x+1 = 11B, however this is a 9-bit value in our 8-bit representation of the product. In GF arithmetic we can check for
	// wrap-around by masking for the right-shifted 9th bit by one (0x100 >> 1 = 0x80). If there is a match we need to XOR the 8-bit capped modulo polynomial (11B -> 1B)
	if ((operand & 0x80) == 0x80)
		result ^= 0x1B;

	return result;
}

uint8_t GFTriple(uint8_t operand)
{
	// In GF arithmetic 3 = x+1 can be expressed as 2 = x plussed with 1
	uint8_t result = GFDouble(operand) ^ operand;

	return result;
}
