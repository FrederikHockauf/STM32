#include "group.h"
#include "smult.h"

int crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk)
{
  group_ge p, k;
  int i,j;

  if(group_ge_unpack(&p, pk)) return -1;

  k = group_ge_neutral;

  for(i=31;i>=0;i--)
  {
    for(j=7;j>=0;j--)
    {
      group_ge_double(&k, &k);
      if((sk[i] >> j) & 1) {
        group_ge_add(&k, &k, &p);
      }
    }
  }

  group_ge_pack(ss, &k);

  return 0;
}

int crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk)
{
  unsigned char t[GROUP_GE_PACKEDBYTES];
  group_ge_pack(t, &group_ge_base);
  return crypto_scalarmult(pk, sk, t);
}

int c_crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk)
{
  group_ge p, k;
  int i,j,l;

  if(group_ge_unpack(&p, pk)) return -1;

  // Set the start value to be the neutral element
  k = group_ge_neutral;

  // Build up the table of multiples of p
  group_ge table[16];
  table[0] = group_ge_neutral;
  for (i = 1; i < 16; i++)
    group_ge_add(&table[i], &table[i-1], &p);

  uint8_t mask = 3;
  uint8_t section;

  for(i=31;i>=0;i--)
  {
    for(j=2;j>=0;j--)
    {
      section = (sk[i] >> (j * 4)) & mask;
      for (l = 0; l < 4; l++)
        group_ge_double(&k, &k);
      
      group_ge_add(&k, &k, &table[section]);
    }

    /*for(j=7;j>=0;j--)
    {
      group_ge_double(&k, &k);
      if((sk[i] >> j) & 1) {
        group_ge_add(&k, &k, &p);
      }
    }*/
  }

  group_ge_pack(ss, &k);

  return 0;
}

/*uint64_t fixed_window_4(uint64_t base, uint64_t exponent)
{
    //int i, j;
    //uint8_t array_size = normal_power(2, 4);
    //uint64_t* table = new uint64_t[array_size];

    //for (i = 0; i < array_size; i++)
    //    table[i] = normal_power(base, i);

    uint64_t result = 1;
    uint64_t mask = array_size - 1;
    uint64_t section;

    for (i = 15;i >= 0;i--)
    {
        section = (exponent >> (i * 4)) & mask;

        for (j = 0; j < 4; j++)
            result = normal_power(result, 2);

        result *= table[section];
    }
    return result;
}*/