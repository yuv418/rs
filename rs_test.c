#include "gf.h"
#include "rs.h"

#include <stdio.h>

int main(int argc, char **argv) {
  gf_init_op_lut();

  int poly_size = 4;
  gf_poly_print(rs_generator_poly(poly_size), poly_size + 1, true);
}
