#pragma once

typedef struct PolynomialQuotient {
  int *quotient;
  int quotient_len;
  int *remainder;
  int remainder_len;
} poly_quotient_t;

void poly_quotient_free(poly_quotient_t *to_free);

int *rs_generator_poly(int num_sym);
poly_quotient_t *rs_poly_div(int *divisor, int divisor_len, int *dividend,
                             int dividend_len);
