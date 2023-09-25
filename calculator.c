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
    int opcode = 0;
    opcode = opcode << 8 | hal_getchar();

    switch (opcode)
    {
      case 0:
        printf("This is addition\n")
        break;
      case 1:
        printf("This is multiplication\n")
        break;
      case 2:
        printf("This is exponentiation\n")
        break;
      case 3:
        printf("This is incrementation\n")
        break;
      case 4:
        printf("This is Res\n")
        break;
      case 5:
        printf("This is Tme\n")
        break;
      case 255:
        printf("This is ERR\n")
        break;
    }
  }

  hal_led_off();

  while (1)
    ;
  return 0;
}
