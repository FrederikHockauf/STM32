#include <hal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct { int32_t a[10]; } int256;
const uint16_t TOTAL_BITS = 256;
const uint8_t INT_BITS = 31; // Sign takes one bit
const uint8_t RADIX = 26;

int256 Karatsuba(int256 a, int256 b, int bits);
int256 MPAAdd(int256 operand1, int256 operand2);
int256 MPASubtraction(int256 operand1, int256 operand2);
int256 MPABitshiftLeft(int256 number, int places);
int256 MPABitshiftRight(int256 number, int places);
int256 MPASchoolbookMultiplication(int256 operand1, int256 operand2);
int256 MPAAbsoluteValue(int256 number);
int256 ReducedRepresentation(int256 number);
int256 Zero();


int main()
{
    // Setup the board and turn on the lights (I think)
	hal_setup();
    hal_led_on();

    for (int rounds = 0; rounds < 10; rounds++)
    {
        uint8_t opcode = hal_getchar();
        printf("opcode = %u", opcode);

        // Initialize two operands as zero
        int256 numA = Zero();
        int256 numB = Zero();

        // Edit limbs to create non-zero numbers
        numA.a[0] = 0b11000000000000000000000000;
        numB.a[0] = 0b01000000000000000000000011;
        numB.a[1] = 0b00000000000000000000111011;
        numB.a[2] = 0b00000000000000000000000100;

        // Calculate and print the product
        int256 result = Karatsuba(numA, numB, TOTAL_BITS);
        PrintNum(result, 1);
        
        
        // Write the encrypted message to the computer
        //WriteBlock(encMessage);
    }


    hal_led_off();

    while (1)
    ;
    return 0;
}

int256 Karatsuba(int256 a, int256 b, int bits)
{
	// Initialize the result and calculate the numbers' highest bit-representation in a potential next level Karatsuba
	int bitsSection = bits / 2;
	int256 result = Zero();

	// Split the numbers into highs and lows through bitshifting
	int256 aHigh = MPABitshiftRight(a, bitsSection);
	int256 bHigh = MPABitshiftRight(b, bitsSection);
	int256 aLow = MPABitshiftRight(MPABitshiftLeft(a, TOTAL_BITS - bitsSection), TOTAL_BITS - bitsSection);
	int256 bLow = MPABitshiftRight(MPABitshiftLeft(b, TOTAL_BITS - bitsSection), TOTAL_BITS - bitsSection);

	// Declare and initialize components to be used in the final calculation
	int256 high;
	int256 low;
	int256 medium;
	int256 mediumHat;
	int256 aDiff = MPASubtraction(aLow, aHigh);
	int256 bDiff = MPASubtraction(bLow, bHigh);

	// If the numbers contain few enough bits to be contained in a limb after multiplcation use SB, otherwise recursive Karatsuba
	if (bits <= INT_BITS / 2)
	{
		high = MPASchoolbookMultiplication(aHigh, bHigh);
		low = MPASchoolbookMultiplication(aLow, bLow);
		medium = MPASchoolbookMultiplication(MPAAbsoluteValue(aDiff), MPAAbsoluteValue(bDiff));
		mediumHat = MPASchoolbookMultiplication(aDiff, bDiff);
	}
	else
	{
		high = Karatsuba(aHigh, bHigh, bitsSection);
		low = Karatsuba(aLow, bLow, bitsSection);
		medium = Karatsuba(MPAAbsoluteValue(aDiff), MPAAbsoluteValue(bDiff), bitsSection);
		mediumHat = Karatsuba(aDiff, bDiff, bitsSection);
	}

	// Constant time (?) comparison between medium and mediumHat to determine if it should be negated
	int t;
	for (int i = 0; i < 10; i++)
		if (medium.a[i] == mediumHat.a[i])
			t += 0;
		else
			t = 0 + 1;

	// Pick the medium value to use
	int256 mediumType;
    if (t == 0)
        mediumType = medium;
    else
        mediumType = mediumHat;

	// Calculate the result
	int256 parenthisSum = MPASubtraction(MPAAdd(low, high), mediumType);
	result = MPAAdd(MPAAdd(low, MPABitshiftLeft(parenthisSum, bits / 2)), MPABitshiftLeft(high, bits));

	return result;
}

int256 MPAAdd(int256 operand1, int256 operand2)
{
	int256 result;

	for (int i = 0; i < 10; i++)
		result.a[i] = operand1.a[i] + operand2.a[i];

	return result;
}

int256 MPASubtraction(int256 operand1, int256 operand2)
{
	// Calculate operand2's negative version by finding its two's complement (negating and adding 1)
	for (int i = 0; i < 10; i++)
		operand2.a[i] = !operand2.a[i];
	operand2.a[0] += 1;

	// Calculate subtraction by adding the negative of operand 2 to operand 1
	int256 result = MPAAdd(operand1, operand2);
	result = ReducedRepresentation(result);
	return result;
}

int256 MPABitshiftLeft(int256 number, int places)
{
	// If we want to shift by more places than in a limb, we shift by the size of a limb and save the places left to shift with
	int remainingPlaces = places;
	if (remainingPlaces > RADIX)
		places = RADIX;
	remainingPlaces -= places;

	// Just shift the bottom-most limb as we don't need to save anything
	number.a[9] <<= places;

	for (int i = 8; i >= 0; i--)
	{
		// Find the portion of the i'th limb that will get deleted and save it
		uint32_t mask = (((1 << RADIX) - 1) >> RADIX - places) << RADIX - places;
		uint32_t carryOver = number.a[i] & mask;

		// Add the saved portion to the limb above and shift the limb itself
		number.a[i + 1] ^= carryOver >> (RADIX - places);
		number.a[i] <<= places;
	}

	// In the case where we still have more places to shift with, we call this function recursively
	if (remainingPlaces > 0)
		number = MPABitshiftLeft(number, remainingPlaces);

	return number;
}

int256 MPABitshiftRight(int256 number, int places)
{
	// If we want to shift by more places than in a limb, we shift by the size of a limb and save the places left to shift with
	int remainingPlaces = places;
	if (remainingPlaces > RADIX)
		places = RADIX;
	remainingPlaces -= places;

	// Just shift the bottom-most limb as we don't need to save anything
	number.a[0] >>= places;

	for (int i = 1; i < 10; i++)
	{
		// Find the portion of the i'th limb that will get deleted and save it
		uint32_t mask = (1 << places) - 1;
		uint32_t carryOver = number.a[i] & mask;

		// Add the saved portion to the limb below and shift the limb itself
		number.a[i - 1] ^= carryOver << (RADIX - places);
		number.a[i] >>= places;
	}

	// In the case where we still have more places to shift with, we call this function recursively
	if (remainingPlaces > 0)
		number = MPABitshiftRight(number, remainingPlaces);

	return number;
}

int256 MPASchoolbookMultiplication(int256 operand1, int256 operand2)
{
	// Set the result to be the limb-wise schoolbook multiplication of the operands
	int256 result;
	for (int i = 0; i < 10; i++)
		result.a[i] = operand1.a[i] * operand2.a[i];

	return result;
}

int256 MPAAbsoluteValue(int256 number)
{
	return number;
}

int256 ReducedRepresentation(int256 number)
{
	for (int i = 0; i < 9; i++)
	{
		uint32_t carry = number.a[i] >> RADIX;
		number.a[i + 1] += carry;
		carry <<= RADIX;
		number.a[i] -= carry;
	}

	return number;
}

int256 Zero()
{
	int256 number;

	for (int i = 0; i < 10; i++)
		number.a[i] = 0;

	return number;
}
