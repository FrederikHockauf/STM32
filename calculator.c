#include <hal.h>
#include <stdio.h>


extern const char _elf_name[];

#ifdef STM32F4  // assembler demo only for the target device
int test_asm(int);
extern int somedata; // = 0x12345678;
#endif




int get_4bytes()
{
  int variable = 0;

  for (int i = 0; i < 4; i++) // Load the 4-byte int in byt bitshifting (8 times as 1 byte = 8 bit)
      variable = variable << 8 | hal_getchar();
  
  return variable;
}

void set_4bytes(int variable)
{
  for (int i = 3; i >= 0; i--)
      hal_putchar((variable >> i*8) & 0xff);
}


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
    int operand_a = get_4bytes();
    int operand_b = -1;

    // Send back the opcode
    hal_putchar(opcode & 0xff);

    // Get time variables
    uint64_t t_start = hal_get_time();
    uint64_t t_stop = hal_get_time();

    switch (opcode)
    {
      case 0:
        operand_b = get_4bytes();
        t_start = hal_get_time();
        printf("This is addition with %i\n", operand_a);
        t_stop = hal_get_time();
        break;
      case 1:
        operand_b = get_4bytes();
        t_start = hal_get_time();
        printf("This is multiplication with %i\n", operand_a);
        t_stop = hal_get_time();
        break;
      case 2:
        operand_b = get_4bytes();
        t_start = hal_get_time();
        printf("This is exponentiation with %i\n", operand_a);
        t_stop = hal_get_time();
        break;
      case 3:
        t_start = hal_get_time();
        printf("This is incrementation with %i\n", operand_a);
        t_stop = hal_get_time();
        break;
      case 4:
        operand_b = get_4bytes();
        t_start = hal_get_time();
        printf("This is Res with %i\n", operand_a);
        t_stop = hal_get_time();
        break;
      case 5:
        operand_b = get_4bytes();
        t_start = hal_get_time();
        printf("This is Tme with %i\n", operand_a);
        t_stop = hal_get_time();
        set_4bytes(t_stop - t_start);
        break;
      case 255:
        operand_b = get_4bytes();
        t_start = hal_get_time();
        printf("This is ERR with %i\n", operand_a);
        t_stop = hal_get_time();
        break;
    }

    // Send back the opcode (again??)
    hal_putchar(opcode & 0xff);



  }


  hal_led_off();

  while (1)
    ;
  return 0;
}
