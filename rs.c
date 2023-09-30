#include "rs.h"
#include "gf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint32_t *gf_exp;
extern uint32_t *gf_log;

void poly_quotient_free(poly_quotient_t *to_free) {
  free(to_free->quotient);
  free(to_free->remainder);
  free(to_free);
}

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

// Debugging function.
void print_2d_array(int **arr, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("%d ", arr[i][j]);
    }
    printf("\n");
  }
}

// For optimization purposes, we could add another column and precompute all of
// these.
int diagonal_value(int *divisor, int n) {
  // We don't really need the size of divisor since it's 1 + n <= divisor_len
  int out = 1;
  for (int i = 1; i < 1 + n; i++) {
    out *= divisor[i];
  }
  return out;
}

// HUGE TODO convert to use GALOIS FIELD OPERATIONS
// TODO Convert to work with non-monic divisors
poly_quotient_t *rs_poly_div(int *divisor, int divisor_len, int *dividend,
                             int dividend_len) {
  // Negate divisor
  for (int i = 0; i < divisor_len; i++) {
    divisor[i] *= -1;
  }

  // Allocate 2d array of divisor_len + 1 rows and dividend_len columns
  int **computation_array = malloc(sizeof(int *) * (divisor_len + 1));
  for (int i = 0; i < divisor_len + 1; i++) {
    computation_array[i] = malloc(sizeof(int) * dividend_len);
    memset(computation_array[i], 0, dividend_len);
  }
  // Fill array with relevant values
  // Step 1: place dividend
  for (int i = 0; i < dividend_len; i++) {
    computation_array[0][i] = dividend[i];
  }

  // Loop through every dividend value
  for (int k = 0; k < dividend_len; k++) {
    // Step 2*(k+1): drop the kth value of the dividend (add down the column)
    for (int i = 0; i < divisor_len; i++) {
      computation_array[divisor_len][k] += computation_array[i][k];
    }

    // Step 3*(k+1): place diagonal values
    int j = k + 1; // Column location (1 forward from the dropped location)
    if (k + divisor_len - 1 < dividend_len) {
      for (int i = divisor_len - 1; i > 0; i--) {
        computation_array[i][j] = diagonal_value(divisor, divisor_len - i) *
                                  computation_array[i + 1][j - 1];
        j++;
      }
    }
  }

  poly_quotient_t *poly_quot = malloc(sizeof(poly_quotient_t));
  // Populate the quotient struct with our values.
  poly_quot->quotient_len = divisor_len - 1;
  poly_quot->quotient = malloc(sizeof(int) * poly_quot->quotient_len);
  memcpy(poly_quot->quotient, computation_array[divisor_len],
         sizeof(int) * poly_quot->quotient_len);

  poly_quot->remainder_len = dividend_len - poly_quot->quotient_len;
  poly_quot->remainder = malloc(sizeof(int) * poly_quot->remainder_len);
  memcpy(poly_quot->remainder,
         (computation_array[divisor_len] + poly_quot->quotient_len),
         sizeof(int) * poly_quot->remainder_len);

  // Deallocate our computation array.
  for (int i = 0; i < divisor_len + 1; i++) {
    free(computation_array[i]);
  }
  free(computation_array);

  return poly_quot;

  // print_2d_array(computation_array, divisor_len + 1, dividend_len);
}
