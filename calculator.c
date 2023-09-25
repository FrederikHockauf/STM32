#include <hal.h>
#include <stdio.h>


extern const char _elf_name[];

#ifdef STM32F4  // assembler demo only for the target device
int test_asm(int);
extern int somedata; // = 0x12345678;
#endif

int main(void)
{
  hal_setup();

  hal_led_on();

  for (int rounds = 0; rounds < 10; rounds++)
  {
    // Initialize the opcode and retrieve it from the computer
    int opcode = 0;
    opcode = opcode << 8 | hal_getchar();

    // Initialize the two operands
    int operand_a = load_4bytes();
    int operand_b = -1;

    // Send back the opcode
    hal_putchar(opcode);

    switch (opcode)
    {
      case 0:
        operand_b = load_4bytes();
        printf("This is addition with %i\n", operand_a);
        break;
      case 1:
        operand_b = load_4bytes();
        printf("This is multiplication with %i\n", operand_a);
        break;
      case 2:
        operand_b = load_4bytes();
        printf("This is exponentiation with %i\n", operand_a);
        break;
      case 3:
        printf("This is incrementation with %i\n", operand_a);
        break;
      case 4:
        operand_b = load_4bytes();
        printf("This is Res with %i\n", operand_a);
        break;
      case 5:
        operand_b = load_4bytes();
        printf("This is Tme with %i\n", operand_a);
        break;
      case 255:
        operand_b = load_4bytes();
        printf("This is ERR with %i\n", operand_a);
        break;
    }
  }

  hal_led_off();

  while (1)
    ;
  return 0;
}

int load_4bytes()
{
  int variable = 0;

  for (int i = 0; i < 4; i++) // Load the 4-byte int in byt bitshifting (8 times as 1 byte = 8 bit)
      variable = variable << 8 | hal_getchar();
  
  return variable;
}