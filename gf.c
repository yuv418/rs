#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t gf_exp[256];
static uint32_t gf_log[256];

void print_n_bits(uint32_t input, int bits) {
  for (int i = 0; i < bits; i++) {
    printf("%d", (input >> (bits - 1 - i)) & 0b1);
  }
}

void gf_poly_print(int *p, int p_len, bool hex) {
  for (int i = p_len - 1; i >= 0; i--) {
    if (hex) {
      printf("%02x x^%d", p[p_len - i - 1], i);
    } else {
      printf("%02d x^%d", p[p_len - i - 1], i);
    }

    if (i > 0) {
      printf(" + ");
    }
  }
  printf("\n");
}
int gf_add(int x, int y) { return x ^ y; }

int gf_sub(int x, int y) { return x ^ y; }

int gf_mul(int x, int y) {
  // For every bit i in y (start at LSB)
  // If the bit at i is equal to 1, then add (x << i) to the total.

  uint32_t total = 0;

  // Loop through every bit in y.
  for (uint32_t i = 0; i < sizeof(y) * 8; i++) {
    // If the ith bit from the right (LSB) is equal to 1,
    if ((y >> i) & 0b1) {
      // Add x << i to the total.
      total = gf_add(total, x << i);
    }
  }

  return total;
}

uint32_t bit_len(int x) {
  uint32_t int_size = (sizeof(x) * 8) - 1;
  while (int_size >= 0) {
    if ((x >> int_size) & 0b1) {
      // Most significant bit found.
      break;
    }
    int_size--;
  }
  return int_size + 1;
}

int gf_mul_reduce(int x, int y) {
  // Multiply
  int xy_mul = gf_mul(x, y);

  // Multiply, then perform modular reduction
  // Reduction step
  int prime_mod = 0b100011101;            // Length 9
  int prime_mod_len = bit_len(prime_mod); // will always be 9
  // Find MSB for xy_mul, shift prime_mod right, add, etc.
  // Do this X times until the bit len is 8

  int xy_mul_len = bit_len(xy_mul);

  if (xy_mul_len >= prime_mod_len) {
    for (int i = xy_mul_len - prime_mod_len; i >= 0; i--) {
      // Check if divisible at this position
      if (xy_mul & (1 << (i + prime_mod_len - 1))) {
        xy_mul = gf_sub(xy_mul, prime_mod << i);
      }
    }
  }
  return xy_mul;
}

void gf_init_op_lut() {
  uint32_t current_mult = 1;
  for (int i = 0; i < 256; i++) {
    gf_exp[i] = current_mult; // current_mul = alpha ** i
    gf_log[current_mult] = i;
    current_mult = gf_mul_reduce(current_mult, 2);
  }
}

int gf_mul_lut(int x, int y) {
  // Lookup sum of exponents, mod 255
  return gf_exp[(gf_log[x] + gf_log[y]) % 255];
}

// Compute x / y. Note there is no handling for division by 0
int gf_div_lut(int x, int y) {
  if (y == 0) {
    return -1;
  }
  if (x == 0) {
    return 0;
  }

  return gf_exp[(gf_log[x] - gf_log[y]) % 255];
}

// base ** exp = (a ** log_a(base)) ** m == a ** (mlog_a(base))
int gf_pow(int base, int exp) { return gf_exp[(gf_log[base] * exp) % 255]; }
// 1 / x == a ** (-log_a(x))
int gf_inv(int x) { return gf_exp[(-1 * gf_log[x]) % 255]; }

// Polynomials
int *gf_poly_scalar_mul(int *p, int poly_len, int x) {
  int *ret;
  ret = malloc(poly_len * sizeof(int));
  for (int i = 0; i < poly_len; i++) {
    ret[i] = gf_mul_lut(p[i], x);
  }
  return ret;
}

// Adds allocating and returning a new vector
int *gf_poly_add(int *p, int p_len, int *q, int q_len) {
  int *ret;
  // P is larger
  int ret_size = 0;
  if (p_len > q_len) {
    ret_size = p_len;
  } else {
    ret_size = q_len;
  }

  ret = malloc(ret_size * sizeof(int));
  memcpy(ret, p,
         p_len * sizeof(int)); // Does this copy into the correct places?
  // gf_poly_print(ret, ret_size);
  // For each value in the smaller polynomial, add them together
  int offset = ret_size - q_len; // offset is 0 if q is larger
  for (int i = 0; i < q_len; i++) {
    ret[i + offset] = gf_add(ret[i + offset], q[i]);
  }
  return ret;
}

int *gf_poly_mul(int *p, int p_len, int *q, int q_len) {
  // Allocate output poly of degree (deg(p) + deg(q)), and size deg(p) + deg(q)
  // + 1 Subtract one because p_len + q_len is actually deg(p) + deg(q) + 2, and
  // you need 1 slot for the 0 exponent, so subtract 1.
  int ret_size = (p_len + q_len - 1) * sizeof(int);
  int *ret = malloc(ret_size);
  memset(ret, 0, ret_size);

  for (int i = 0; i < p_len; i++) {
    for (int j = 0; j < q_len; j++) {
      ret[i + j] = gf_add(ret[i + j], gf_mul_lut(p[i], q[j]));
    }
  }
  return ret;
}

// Horner's polynomial evaluation
int gf_poly_eval(int *p, int p_len, int value) {
  // Remember, the first element is the one with the highest degree.
  int eval_running = 0;
  // Skip the constant
  for (int i = 0; i < p_len - 1; i++) {
    eval_running = gf_mul_lut(gf_add(eval_running, p[i]), value);
  }
  eval_running = gf_add(eval_running, p[p_len - 1]);
  return eval_running;
}

int main(int argc, char **argv) {
  // Set up multiplication
  gf_init_op_lut();

  // Tests
  int x = 0b0101;
  int y = 0b0110;

  printf("x + y = ");
  int xy_result = gf_add(x, y);
  print_n_bits(xy_result, 4);
  printf(" (total used bits: %d)\n", bit_len(xy_result));

  int x1 = 0b10001001;
  int y1 = 0b00101010;

  printf("x1 * y1 = ");
  int x1y1_result = gf_mul_lut(x1, y1);
  print_n_bits(x1y1_result, bit_len(x1y1_result));
  printf("\n");

  // Polynomial tests.
  int test_poly[5] = {0x1, 0xf, 0x36, 0x78, 0x40};
  int test_poly2[3] = {0xf, 0xe, 0x3};
  int test_poly3[2] = {0xf, 0};
  printf("Printing polynomial 1: ");
  gf_poly_print(test_poly, 5, false);

  printf("Printing polynomial 2: ");
  gf_poly_print(test_poly2, 3, false);

  printf("Printing polynomial 3: ");
  gf_poly_print(test_poly3, 2, false);

  printf("Scalar multiplication of polynomial: ");
  int *test_poly_scalar_mult = gf_poly_scalar_mul(test_poly, 5, 2);
  gf_poly_print(test_poly_scalar_mult, 5, false);

  printf("Addition of polynomials: ");
  int *test_poly_add = gf_poly_add(test_poly2, 3, test_poly, 5);
  // This probably doesn't look right, but remember add is XOR and not +
  gf_poly_print(test_poly_add, 5, false);

  printf("Multiplication of polynomials: ");
  int *test_poly_mult = gf_poly_mul(test_poly, 5, test_poly2, 3);
  gf_poly_print(test_poly_mult, 7, false);

  printf("Evaluation of polynomial 1 at 3: ");
  int test_eval_poly = gf_poly_eval(test_poly, 5, 3);
  printf("%d\n", test_eval_poly, false);
}
