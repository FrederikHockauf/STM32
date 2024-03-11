#ifndef SMULT_H
#define SMULT_H

int crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk);

int crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk);

int c_crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk);

int c_crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk);

int c_const_crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk);

void point_lookup(group_ge *point, group_ge *table, int pos);

unsigned char integer_equality(int a, int b);

void point_conditional_move(group_ge* point, group_ge* table_value, unsigned char equality);



#endif