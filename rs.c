#include "rs.h"
#include "gf.h"

#include <stdio.h>
#include <stdlib.h>

extern uint32_t *gf_exp;
extern uint32_t *gf_log;

int *rs_generator_poly(int num_sym) {
  // [ 1, a³ ] for example
  int *generator_term = malloc(2 * sizeof(int));
  generator_term[0] = 1;

  int *output_product = malloc(2 * sizeof(int));
  output_product[0] = 1;
  output_product[1] = gf_exp[0];

  int output_product_len = 2;

  for (int i = 1; i < num_sym; i++) {
    generator_term[1] = gf_exp[i];
    gf_poly_print(generator_term, 2, false);

    // How 2 cause memory leak yay. TODO just put it into a temp var and free
    // the old output product
    int *temp_output_product =
        gf_poly_mul(generator_term, 2, output_product, output_product_len);
    free(output_product);
    output_product = temp_output_product;

    output_product_len = 1 + output_product_len;
  }

  return output_product;
}

int *rs_poly_div(int *divisor, int divisor_len, int *dividend,
                 int divident_len) {
  // Divisors are offset +1 when they are added in
  // synthetic division. Divisor coefficient is negated.

  int *quotient = malloc(sizeof(int) * divisor_len);
}
