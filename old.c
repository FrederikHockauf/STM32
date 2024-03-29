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
    printf("Hello world from the biggest and bestest \"%s\"!\n", _elf_name);

    hal_led_off();

    // Initialize the opcode and retrieve it from the computer
    int opcode = 0;
    opcode = opcode << 8 | hal_getchar();

    hal_led_on();

    if (opcode == 0)
      printf("code is 0\n");
    else if (opcode == 1)
      printf("code is 1\n");
    else if (opcode == 2)
      printf("code is 2\n");
    else if (opcode == 3)
      printf("code is 3\n");
    else if (opcode == 4)
      printf("code is 4\n");
    else if (opcode == 5)
      printf("code is 5\n");
    else if (opcode == 6)
      printf("code is 6\n");
    else
      printf("none o' dese\n");

    // Initialize the two operands
    int operand_a = get_4bytes();
    int operand_b = get_4bytes();
    int c = operand_a + operand_b;

    printf("got 'em boss\n");


    // Send back the opcode
    for (int i = 3; i >= 0; i--)
      hal_putchar((c >> i*8) & 0xff);

    // Get time variables
    uint64_t t_start;
    uint64_t t_stop;

    

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

    // Send back the opcode (again?)
    hal_putchar(0xff); // used to be => opcode 0xff
    // Change has been made
  }


  hal_led_off();

  while (1)
    ;
  return 0;
}
