#include "rs.h"
#include "gf.h"

#include <stdlib.h>

int *rs_generator_poly(int num_sym) {
  // [ 1, a³ ] for example
  int *generator_term = malloc(2 * sizeof(int));
  generator_term[0] = 1;

  int *output_product = malloc(2 * sizeof(int));
  output_product[0] = 1;
  output_product[1] = gf_exp[0];

  int output_product_len = 2;

  for (int i = 1; i < num_sym; i++) {
    generator_term[1] = gf_exp[num_sym];

    // How 2 cause memory leak yay. TODO just put it into a temp var and free
    // the old output product
    output_product =
        gf_poly_mul(generator_term, 2, output_product, output_product_len);
    output_product_len = 1 + output_product_len;
  }
}
