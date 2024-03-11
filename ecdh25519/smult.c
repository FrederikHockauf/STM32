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

  uint8_t mask = 15;
  uint8_t section;

  for(i=31;i>=0;i--)
  {
    for(j=1;j>=0;j--)
    {
      section = (sk[i] >> (j * 4)) & mask;
      for (l=0; l<4; l++)
        group_ge_double(&k, &k);
      
      group_ge_add(&k, &k, &table[section]);
    }
  }

  group_ge_pack(ss, &k);

  return 0;
}

int c_crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk)
{
  unsigned char t[GROUP_GE_PACKEDBYTES];
  group_ge_pack(t, &group_ge_base);
  return c_crypto_scalarmult(pk, sk, t);
}

int c_const_crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk)
{
  group_ge p, k, addition_point;
  int i,j,l;

  if(group_ge_unpack(&p, pk)) return -1;

  // Set the start value to be the neutral element
  k = group_ge_neutral;

  // Build up the table of multiples of p
  group_ge table[16];
  table[0] = group_ge_neutral;
  for (i = 1; i < 16; i++)
    group_ge_add(&table[i], &table[i-1], &p);

  uint8_t mask = 15;
  uint8_t section;

  for(i=31;i>=0;i--)
  {
    for(j=1;j>=0;j--)
    {
      section = (sk[i] >> (j * 4)) & mask;
      for (l=0; l<4; l++)
        group_ge_double(&k, &k);
      
      point_lookup(&addition_point, table, section);
      group_ge_add(&k, &k, &addition_point);
    }
  }

  group_ge_pack(ss, &k);

  return 0;
}

void point_lookup(group_ge *point, group_ge *table, int pos)
{
    int i, j;
    unsigned char equality;
    *point = table[0];
    uint64_t n_point = 0;

    // Go through the entire table and only move the intended target
    for (i = 0; i < 16; i++)
    {
        equality = integer_equality(i, pos);
        point_conditional_move(point, &table[i], equality);
    }
}

unsigned char integer_equality(int a, int b)
{
  // Compute the bit to determine if an element should be moved
  unsigned long long t = a ^ b;
  t = (-t) >> 63;
  return 1 - t;
}

void point_conditional_move(group_ge* point, group_ge* table_value, unsigned char equality)
{
    unsigned char* c_point = (unsigned char*)point;
    unsigned char* c_value = (unsigned char*)table_value;
    int i;
    equality = -equality;

    // Mask out the values that we're interested in using the equality as a mask
    for (i = 0; i < sizeof(group_ge); i++)
        c_point[i] = (equality & c_value[i]) ^ (~equality & c_point[i]);
}