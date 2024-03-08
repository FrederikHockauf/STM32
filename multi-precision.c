#include <iostream>
#include <string>
#include <algorithm>


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

int256 StringToNum(std::string number);
std::string NumToString(int256 number);
void PrintNum(int256 number, int limbs);
void PrintNumWithOverflow(int256 number, int limbs);
std::string LimbAsString(int32_t limb, bool withOverflow);
std::string NumAsString(int256 number, int limbs, bool includeSpace);


int main()
{
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
	
	// Wait for a response before closing the window
	int a = 0;
	std::cin >> a;
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
	(t == 0) ? mediumType = medium : mediumType = mediumHat;

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

int256 StringToNum(std::string number)
{
	// Initialize the number as 0
	int256 parsed;
	for (int i = 0; i < 10; i++)
		parsed.a[i] = 0;

	// Return the 0 if the string isn't binary
	int stringLength = number.length();
	for (int i = 0; i < stringLength; i++)
		if ((number[i] != '0') && (number[i] != '1'))
			return parsed;

	int segments = (int)ceil((float)stringLength / RADIX);

	for (int i = 0; i < segments; i++)
	{
		int iterations = std::min(stringLength, (int)RADIX) - 1;
		for (int j = iterations; j >= 0; j--)
		{
			parsed.a[i] += ((uint32_t)number[j + RADIX * (segments - i - 1)] - 48) << (iterations - j);

			stringLength--;
			if (stringLength <= 0)
				goto parseDone;
		}
	}

parseDone:
	return parsed;
}

std::string NumToString(int256 number)
{
	std::string parsed = "";

	int HighestNonZeroIndex = -1;

	for (int i = 9; i >= 0; i--)
	{
		if (number.a[i] != 0)
		{
			HighestNonZeroIndex = i;
			break;
		}
	}

	parsed = NumAsString(number, HighestNonZeroIndex + 1, false);
	return parsed;
}

void PrintNum(int256 number, int limbs)
{
	int iterations = std::min(10, limbs);
	std::string full = NumAsString(number, iterations, true);
	std::cout << "number = " << full << std::endl;
}

void PrintNumWithOverflow(int256 number, int limbs)
{
	int iterations = std::min(10, limbs);
	for (int i = 0; i < iterations; i++)
	{
		std::string parsed = "";//LimbAsString(number.a[i], true);

		// Due to compiling errors:
		{
			for (int j = 0; j < 32; j++)
			{
				// Input a 1 or 0 depending on the right-most digit
				if ((number.a[i] & 1) == 1)
					parsed = "1" + parsed;
				else
					parsed = "0" + parsed;

				// Go to the next digit for the next loop
				number.a[i] >>= 1;

				// When coming to the radix-point, either create an underscore and keep going with the overflow, or break the loop
				if (j == RADIX - 1)
				{
					if (true)
						parsed = "_" + parsed;
					else
						break;
				}
			}
		}

		std::cout << "index " << i << " = " << parsed << std::endl;
	}
}

std::string LimbAsString(int32_t limb, bool withOverflow)
{
	std::string parsed = "";
	for (int i = 0; i < 32; i++)
	{
		// Input a 1 or 0 depending on the right-most digit
		if ((limb & 1) == 1)
			parsed = "1" + parsed;
		else
			parsed = "0" + parsed;

		// Go to the next digit for the next loop
		limb >>= 1;

		// When coming to the radix-point, either create an underscore and keep going with the overflow, or break the loop
		if (i == RADIX - 1)
		{
			if (withOverflow)
				parsed = "_" + parsed;
			else
				break;
		}
	}

	return parsed;
}

std::string NumAsString(int256 number, int limbs, bool includeSpace)
{
	int iterations = std::min(10, limbs);
	std::string full = "";

	for (int i = 0; i < iterations; i++)
	{
		std::string parsed = LimbAsString(number.a[i], false);

		// Mark the beginning of the next limb with a space
		if ((includeSpace) && (i != 0))
			parsed = parsed + " ";

		full = parsed + full;
	}

	return full;
}

