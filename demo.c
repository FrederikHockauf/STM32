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
    printf("Hello world from \"%s\"!\n", _elf_name);

    int val = 0; // Initialize the variable

    for (int i = 0; i < 4; i++) // Load the 4-byte int in byt bitshifting (8 times as 1 byte = 8 bit)
      val = val << 8 | hal_getchar();

    uint64_t t_start = hal_get_time(); // Create a variable to keep track of start time

    val *= 3; // Create a new value to send back. This is just to check that the initial input from Python went through correctly

    uint64_t t_stop = hal_get_time(); // Create a variable to keep track of end time

    for (int i = 0; i < 4; i++)
      hal_putchar((val >> ((3-i)*8)) & 0xff);

    for (int i = 4; i > 0; i--)
      hal_putchar((val >> ((3-i)*8)) & 0xff);

    for (int i = 4; i > 0; i--)
      hal_putchar((val >> i*8) & 0xff);

    for (int i = 4; i > 0; i--)
      hal_putchar(0xff);


    printf("%i time: %u    ", val, (unsigned) (t_stop - t_start));

#ifdef STM32F4  // assembler demo only for the target device
    val = test_asm(1);

    printf("asm: %x + 1 = %x\n", somedata, val);
#else
    printf("\n");
#endif

  }

  hal_led_off();

  while (1)
    ;
  return 0;
}
