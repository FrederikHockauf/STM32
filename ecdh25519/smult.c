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

