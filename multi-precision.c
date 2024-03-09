#include <hal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct { int32_t a[11]; } int256;
typedef struct { int32_t a[22]; } int512;
const uint16_t TOTAL_BITS = 512;
const uint16_t TOTAL_LIMBS = 11;
const uint8_t INT_BITS = 31; // Sign takes one bit
const uint8_t RADIX = 24;

int512 Karatsuba(int512 a, int512 b, int bits);
int512 MPAAdd(int512 operand1, int512 operand2);
int512 MPASubtraction(int512 operand1, int512 operand2);
int512 MPABitshiftLeft(int512 number, int places);
int512 MPABitshiftRight(int512 number, int places);
int512 MPASchoolbookMultiplication(int512 operand1, int512 operand2);
int512 MPAAbsoluteValue(int512 number);
int512 ReducedRepresentation(int512 number);
int256 Zero256();
int512 Zero512();
int512 ToInt512(int256 number);

int ReadBytes(int bytes);
void WriteBytes(int variable, int bytes);
int256 ReadInt256();
void WriteInt256(int256 variable);
void WriteInt512(int512 variable);


int main()
{
    // Setup the board and turn on the lights (I think)
	hal_setup();
    hal_led_on();

    for (int rounds = 0; rounds < 10; rounds++)
    {
        printf("alive and well\n");

        uint8_t opcode = hal_getchar();
        printf("opcode = %u\n", opcode);

        // Initialize two operands as zero
        int256 numA = ReadInt256();
        int256 numB = ReadInt256();

        printf("done loading - %u and %u and %u and %u and %u and %u and %u and %u and %u and %u and %u\n", numA.a[10], numA.a[9], numA.a[8], numA.a[7], numA.a[6], numA.a[5], numA.a[4], numA.a[3], numA.a[2], numA.a[1], numA.a[0]);

        // Declare the time and result variables to be returned
        int timeStart, timeStop;
        int512 result;

        // Expand the operands to an int512 type
        int512 expandedNumA = ToInt512(numA);
        int512 expandedNumB = ToInt512(numB);

        // Perform either addition (0) or multiplication (1)
        if (opcode == 0)
        {
            // Start the time
            timeStart = hal_get_time();
            
            // Perform addition and convert the variable to a 512-variant
            result = MPAAdd(expandedNumA, expandedNumB);

            // Stop the time and calculate the final time
            timeStop = hal_get_time();          
        }
        else
        {
            // Start the time
            timeStart = hal_get_time();
            
            // Perform multiplication
            result = Karatsuba(expandedNumA, expandedNumB, TOTAL_BITS);

            // Stop the time and calculate the final time
            timeStop = hal_get_time();
        }

        // Send back the result opcode and the result
        hal_putchar(opcode);
        WriteInt512(result);

        // Send back the time opcode and the time
        uint8_t timeOpcode = 5;
        hal_putchar(timeOpcode);
        WriteBytes(timeStop - timeStart, 4);
    }


    hal_led_off();

    while (1)
    ;
    return 0;
}

int512 Karatsuba(int512 a, int512 b, int bits)
{
	// Initialize the result and calculate the numbers' highest bit-representation in a potential next level Karatsuba
	int bitsSection = bits / 2;
	int512 result = Zero512();

	// Split the numbers into highs and lows through bitshifting
	int512 aHigh = MPABitshiftRight(a, bitsSection);
	int512 bHigh = MPABitshiftRight(b, bitsSection);
	int512 aLow = MPABitshiftRight(MPABitshiftLeft(a, TOTAL_BITS - bitsSection), TOTAL_BITS - bitsSection);
	int512 bLow = MPABitshiftRight(MPABitshiftLeft(b, TOTAL_BITS - bitsSection), TOTAL_BITS - bitsSection);

	// Declare and initialize components to be used in the final calculation
	int512 high;
	int512 low;
	int512 medium;
	int512 mediumHat;
	int512 aDiff = MPASubtraction(aLow, aHigh);
	int512 bDiff = MPASubtraction(bLow, bHigh);

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
	for (int i = 0; i < (TOTAL_LIMBS * 2); i++)
		if (medium.a[i] == mediumHat.a[i])
			t += 0;
		else
			t = 0 + 1;

	// Pick the medium value to use
	int512 mediumType;
    if (t == 0)
        mediumType = medium;
    else
        mediumType = mediumHat;

	// Calculate the result
	int512 parenthisSum = MPASubtraction(MPAAdd(low, high), mediumType);
	result = MPAAdd(MPAAdd(low, MPABitshiftLeft(parenthisSum, bits / 2)), MPABitshiftLeft(high, bits));

	return result;
}

int512 MPAAdd(int512 operand1, int512 operand2)
{
	int512 result;

	for (int i = 0; i < (TOTAL_LIMBS * 2); i++)
		result.a[i] = operand1.a[i] + operand2.a[i];

	return result;
}

int512 MPASubtraction(int512 operand1, int512 operand2)
{
	// Calculate operand2's negative version by finding its two's complement (negating and adding 1)
	for (int i = 0; i < (TOTAL_LIMBS * 2); i++)
		operand2.a[i] = ~operand2.a[i];
	operand2.a[0] += 1;

	// Calculate subtraction by adding the negative of operand 2 to operand 1
	int512 result = MPAAdd(operand1, operand2);
	result = ReducedRepresentation(result);
	return result;
}

int512 MPABitshiftLeft(int512 number, int places)
{
	// If we want to shift by more places than in a limb, we shift by the size of a limb and save the places left to shift with
	int remainingPlaces = places;
	if (remainingPlaces > RADIX)
		places = RADIX;
	remainingPlaces -= places;

	// Just shift the bottom-most limb as we don't need to save anything
	number.a[(TOTAL_LIMBS * 2) - 1] <<= places;

	for (int i = (TOTAL_LIMBS * 2) - 2; i >= 0; i--)
	{
		// Find the portion of the i'th limb that will get deleted and save it
		uint32_t mask = (((1 << RADIX) - 1) >> (RADIX - places)) << (RADIX - places);
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

int512 MPABitshiftRight(int512 number, int places)
{
	// If we want to shift by more places than in a limb, we shift by the size of a limb and save the places left to shift with
	int remainingPlaces = places;
	if (remainingPlaces > RADIX)
		places = RADIX;
	remainingPlaces -= places;

	// Just shift the bottom-most limb as we don't need to save anything
	number.a[0] >>= places;

	for (int i = 1; i < (TOTAL_LIMBS * 2); i++)
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

int512 MPASchoolbookMultiplication(int512 operand1, int512 operand2)
{
	// Set the result to be the limb-wise schoolbook multiplication of the operands
	int512 result;
	for (int i = 0; i < (TOTAL_LIMBS * 2); i++)
		result.a[i] = operand1.a[i] * operand2.a[i];

	return result;
}

int512 MPAAbsoluteValue(int512 number)
{
	return number;
}

int512 ReducedRepresentation(int512 number)
{
	for (int i = 0; i < ((TOTAL_LIMBS * 2) - 1); i++)
	{
		uint32_t carry = number.a[i] >> RADIX;
		number.a[i + 1] += carry;
		carry <<= RADIX;
		number.a[i] -= carry;
	}

	return number;
}

int256 Zero256()
{
	int256 number;

	for (int i = 0; i < TOTAL_LIMBS; i++)
		number.a[i] = 0;

	return number;
}

int512 Zero512()
{
	int512 number;

	for (int i = 0; i < (TOTAL_LIMBS * 2); i++)
		number.a[i] = 0;

	return number;
}

int512 ToInt512(int256 number)
{
	int512 newNumber;

	for (int i = 0; i < TOTAL_LIMBS; i++)
		newNumber.a[i] = number.a[i];

    for (int i = TOTAL_LIMBS; i < (TOTAL_LIMBS * 2); i++)
		newNumber.a[i] = 0;
    
    return newNumber;
}

int ReadBytes(int bytes)
{
  int variable = 0;

  for (int i = 0; i < bytes; i++) // Load the 4-byte int in byt bitshifting (8 times as 1 byte = 8 bit)
      variable = variable << 8 | hal_getchar();
  
  return variable;
}

void WriteBytes(int variable, int bytes)
{
  for (int i = 0; i < bytes; i++)
      hal_putchar((variable >> (((bytes-1)-i)*8)) & 0xff);
}

int256 ReadInt256()
{
    int256 number = Zero256();

    number.a[TOTAL_LIMBS - 1] = ReadBytes(2);
    for (int limb = (TOTAL_LIMBS - 2); limb > -1; limb--)
        number.a[limb] = ReadBytes(3);

    return number;
}

void WriteInt256(int256 variable)
{
    int value = variable.a[TOTAL_LIMBS - 1];
    value = (value << 16) >> 16;
    WriteBytes(value, 2);
    
    for (int limb = (TOTAL_LIMBS - 2); limb > -1; limb--)
    {
        value = variable.a[limb];
        value = (value << 8) >> 8;
        WriteBytes(value, 3);
    }
}

void WriteInt512(int512 variable)
{
    int value = variable.a[(TOTAL_LIMBS * 2) - 1];
    value = (value << 24) >> 24;
    WriteBytes(value, 1);
    
    for (int limb = ((TOTAL_LIMBS * 2) - 2); limb > -1; limb--)
    {
        value = variable.a[limb];
        value = (value << 8) >> 8;
        WriteBytes(value, 3);
    }
}