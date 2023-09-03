#pragma once

#include <stdbool.h>
#include <stdint.h>

static uint32_t gf_exp[256];
static uint32_t gf_log[256];

void print_n_bits(uint32_t input, int bits);
void gf_poly_print(int *p, int p_len, bool hex);
int gf_add(int x, int y);
int gf_sub(int x, int y);
int gf_mul(int x, int y);
int gf_mul_reduce(int x, int y);
int gf_init_op_lut();
int gf_mul_lut(int x, int y);
int gf_div_lut(int x, int y);
int gf_pow_lut(int base, int exp);
int gf_inv_lut(int x);
int bit_len(int x);
int *gf_poly_scalar_mul(int *p, int poly_len, int x);
int *gf_poly_add(int *p, int p_len, int *q, int q_len);
int *gf_poly_mul(int *p, int p_len, int *q, int q_len);
int gf_poly_eval(int *p, int p_len, int value);
