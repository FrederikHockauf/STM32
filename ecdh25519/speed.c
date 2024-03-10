#include <hal.h>
#include <stdio.h>

#include "smult.h"

#define OUTLEN 1024

unsigned char sk[32] = {
  0x57, 0x6c, 0x7c, 0x77, 0x6a, 0xc2, 0x93, 0xc6, 0x78, 0x3a, 0x4a, 0x48, 0xc9, 0x45, 0x20, 0x36, 
  0x7d, 0xb3, 0xd4, 0x8c, 0x66, 0xa0, 0x52, 0xa8, 0xb2, 0xea, 0x09, 0xdc, 0x41, 0x43, 0xc5, 0x61};

unsigned char pk[32];
unsigned char ss[32];

int main(void)
{
  hal_setup();

  unsigned int oldcount, newcount;

  printf("\n============ IGNORE OUTPUT BEFORE THIS LINE ============\n\n");
  
  oldcount = hal_get_time();
  crypto_scalarmult_base(pk, sk);
  newcount = hal_get_time()-oldcount;

  printf("cycles for scalarmult_base: %u\n\n", newcount);

  oldcount = hal_get_time();
  crypto_scalarmult(ss, sk, pk);
  newcount = hal_get_time()-oldcount;

  printf("cycles for scalarmult: %u\n", newcount);

  while(1);

  return 0;
}
