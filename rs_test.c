#include "gf.h"
#include "rs.h"

#include <stdio.h>

int main(int argc, char **argv) {
  gf_init_op_lut();

  int poly_size = 4;
  gf_poly_print(rs_generator_poly(poly_size), poly_size + 1, true);

  // Test division
  int dividend[4] = {1, -12, 0, -42};
  int divisor[3] = {1, 1, -3};

  // int dividend[4] = {6, 5, 0, -7};
  // int divisor[3] = {3, -2, -1};
  poly_quotient_t *quotient = rs_poly_div(divisor, 3, dividend, 4);
  print_poly_quotient(quotient);
  poly_quotient_free(quotient);
}
