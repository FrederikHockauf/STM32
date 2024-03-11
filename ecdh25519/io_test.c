#include <hal.h>
#include <stdio.h>

#include "smult.h"
#include "fe25519.h"


#define SMUL 0
#define BMUL 1
#define FSQ  2
#define FMUL 3
#define RES  4
#define TME  5
#define ERR  255


uint32_t read_uint32_t()
{
  uint32_t val = 0;

  for (int i = 0; i < 4; i++)
    val = val << 8 | hal_getchar();

  return val;
}

void write_uint32_t(uint32_t val)
{
  for (int i = 0; i < 4; i++)
    hal_putchar((val >> ((3-i)*8)) & 0xff);
}


void read_32_byte(uint8_t data[32])
{
  for (int i = 0; i < 32; i++)
    data[31-i] = hal_getchar();
}

void write_32_byte(const uint8_t data[32])
{
  for (int i = 0; i < 32; i++)
    hal_putchar(data[31-i]);
}


int main(void)
{
  hal_setup();

  uint8_t sk[32];
  uint8_t pk[32];
  uint8_t out[32];

  uint8_t a[32];
  uint8_t b[32];
  fe25519 op0, op1, r;

  int ret = 0;

  uint64_t t_start = 0, t_stop = 0;

  while(1) {
    unsigned char opc;

    opc =  hal_getchar();

    switch (opc)
    {
      case BMUL:
        read_32_byte(sk);

        t_start = hal_get_time();
        ret = c_const_crypto_scalarmult_base(out, sk); 
        t_stop = hal_get_time();

        break;
      case SMUL:
        read_32_byte(sk);
        read_32_byte(pk);

        t_start = hal_get_time();
        ret = c_const_crypto_scalarmult(out, sk, pk);
        t_stop = hal_get_time();

        break;
      case FSQ:
        read_32_byte(a);

        fe25519_unpack(&op0, a);

        t_start = hal_get_time();
        fe25519_square(&r, &op0);
        t_stop = hal_get_time();

        fe25519_pack(out, &r);

        break;
      case FMUL:
        read_32_byte(a);
        read_32_byte(b);

        fe25519_unpack(&op0, a);
        fe25519_unpack(&op1, b);

        t_start = hal_get_time();
        fe25519_mul(&r, &op0, &op1);
        t_stop = hal_get_time();

        fe25519_pack(out, &r);

        break;
      case ERR:
        continue;
      default:
        ret = -1;
    }

    if (ret == 0)
    {
      hal_putchar(RES);

      write_32_byte(out);

      hal_putchar(TME);

      write_uint32_t(t_stop - t_start);
    }
    else
      hal_putchar(ERR);
  }

  return 0;
}
