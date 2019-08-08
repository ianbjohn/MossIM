#include <stdio.h>
#include "aes.h"

int main() {
  byte key[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  byte round_keys[11][16];

  printf("%x\n", round_keys);

  aes_keyexpansion(key, round_keys);

  for (int i = 0; i < 11; i++)
    printf("Round Key %d: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", i,
        round_keys[i][0], round_keys[i][1], round_keys[i][2], round_keys[i][3],
        round_keys[i][4], round_keys[i][5], round_keys[i][6], round_keys[i][7],
        round_keys[i][8], round_keys[i][9], round_keys[i][10], round_keys[i][11],
        round_keys[i][12], round_keys[i][13], round_keys[i][14], round_keys[i][15]);
  return 0;
}
