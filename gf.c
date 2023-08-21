#include <stdio.h>

void print_n_bits(int input, int bits) {
  for (int i = 0; i < bits; i++) {
    printf("%d", (input >> (bits - 1 - i)) & 0b1);
  }
}

int gf_add(int x, int y) { return x ^ y; }

int gf_sub(int x, int y) { return x ^ y; }

int gf_mul(int x, int y) {
  // For every bit i in y (start at LSB)
  // If the bit at i is equal to 1, then add (x << i) to the total.

  int total = 0;

  // Loop through every bit in y.
  for (int i = 0; i < sizeof(y) * 8; i++) {
    // If the ith bit from the right (LSB) is equal to 1,
    if ((y >> i) & 0b1) {
      // Add x << i to the total.
      total = gf_add(total, x << i);
    }
  }

  return total;
}

int bit_len(int x) {
  int int_size = (sizeof(x) * 8) - 1;
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
  for (int i = xy_mul_len - prime_mod_len; i >= 0; i--) {
    // Check if divisible at this position
    if (xy_mul & (1 << (i + prime_mod_len - 1))) {
      xy_mul = gf_sub(xy_mul, prime_mod << i);
    }
  }
  return xy_mul;
}

int main(int argc, char **argv) {
  int x = 0b0101;
  int y = 0b0110;

  printf("x + y = ");
  int xy_result = gf_add(x, y);
  print_n_bits(xy_result, 4);
  printf(" (total used bits: %d)\n", bit_len(xy_result));

  int x1 = 0b10001001;
  int y1 = 0b00101010;

  printf("x1 * y1 = ");
  int x1y1_result = gf_mul_reduce(x1, y1);
  print_n_bits(x1y1_result, bit_len(x1y1_result));
  printf("\n");
}
