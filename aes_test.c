#include <stdio.h>
#include "aes.h"

int main() {
  byte key[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  byte round_keys[11][16];
  byte plaintext[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};

  byte state[4][4];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      state[j][i] = plaintext[(i * 4) + j];  //column-major traversal
  }

  //print real quick in row-major order
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      printf("%x ", state[i][j]);
    printf("\n");
  }
  printf("\n");

  //mixcolumns
  aes_mixcolumns(state);

  //print result
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      printf("%x ", state[i][j]);
    printf("\n");
  }
  printf("\n");

  //invmixcolumns
  aes_invmixcolumns(state);

  //print result (should be original state? but isn't?)
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      printf("%x ", state[i][j]);
    printf("\n");
  }

  //test to see if mult_mod works as intended
  //printf("%x\n", aes_mult_mod(0x01, 0xBF));

  return 0;
}
