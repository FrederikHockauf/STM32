#include <hal.h>
#include <stdio.h>

#include "group.h"
#include "smult.h"

unsigned char sk0[32] = {0xb1, 0x7a, 0xa0, 0x76, 0x93, 0xd7, 0x8d, 0x70, 0xfb, 0x44, 0x3a, 0x5b, 0xf1, 0xc6, 0x90, 0xe2, 
                         0xc3, 0x79, 0x39, 0x6f, 0x56, 0xac, 0xc5, 0x5f, 0xb5, 0xfc, 0x1c, 0xc5, 0x58, 0xa2, 0xd9, 0x85};

unsigned char sk1[32] = {0x00, 0x20, 0x04, 0x00, 0x10, 0xa1, 0x9b, 0x7d, 0x42, 0xcd, 0xcc, 0x00, 0x73, 0xca, 0x72, 0xed,
                         0x8a, 0x60, 0x9b, 0x95, 0xe3, 0xf8, 0x33, 0x98, 0x37, 0x4a, 0xb2, 0x53, 0x54, 0xab, 0xe8, 0x69};


unsigned char cmppk0[32] = {0xdc, 0x04, 0xf6, 0xaa, 0x52, 0xd3, 0x71, 0x76, 0x12, 0xd7, 0x13, 0x81, 0xab, 0x0d, 0x75, 0xef,
                            0xe7, 0x9d, 0x59, 0x95, 0xff, 0x93, 0x21, 0xda, 0x46, 0x23, 0x41, 0xd0, 0xb3, 0x27, 0xed, 0x51};


unsigned char cmppk1[32] = {0xbe, 0x32, 0x6d, 0x49, 0x53, 0xa8, 0x53, 0xd7, 0xbc, 0xb8, 0x32, 0xc2, 0xf6, 0xc3, 0xf7, 0xaf,
                            0xbd, 0x63, 0x2c, 0xb5, 0xe9, 0x2b, 0x4c, 0x99, 0x7d, 0x62, 0x3f, 0x94, 0x06, 0x76, 0x3e, 0x62};

unsigned char cmpss[32]  = {0x96, 0x02, 0x7e, 0x70, 0x6a, 0x34, 0x2d, 0x64, 0xee, 0x62, 0xa5, 0xea, 0x38, 0x13, 0x42, 0x7d,
                            0x11, 0x59, 0xfd, 0xe0, 0x4c, 0x50, 0x6f, 0x9d, 0x09, 0xab, 0x82, 0x16, 0xd9, 0x2a, 0x89, 0x45};


int main(void)
{
  hal_setup();

  int i;
  unsigned char pk0[GROUP_GE_PACKEDBYTES];
  unsigned char pk1[GROUP_GE_PACKEDBYTES];
  unsigned char ss0[GROUP_GE_PACKEDBYTES];
  unsigned char ss1[GROUP_GE_PACKEDBYTES];

  crypto_scalarmult_base(pk0, sk0); 
  crypto_scalarmult_base(pk1, sk1); 

  crypto_scalarmult(ss0, sk0, pk1);
  crypto_scalarmult(ss1, sk1, pk0);

  for(i=0;i<32;i++)
  {
    if(ss0[i] != ss1[i])
    {
      printf("Test failed!\n");
      while(1);
      return -1;
    }

    if(pk0[i] != cmppk0[i])
    {
      printf("Test failed!\n");
      while(1);
      return -1;
    }
    if(pk1[i] != cmppk1[i])
    {
      printf("Test failed!\n");
      while(1);
      return -1;
    }
    if(ss0[i] != cmpss[i])
    {
      printf("Test failed!\n");
      while(1);
      return -1;
    }
  }     

  printf("Test successful!\n");

  while(1);

  return 0;
}