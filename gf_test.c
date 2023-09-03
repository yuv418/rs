#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "gf.h"

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
