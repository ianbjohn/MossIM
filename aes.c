#include <stdio.h>
#include "aes.h"

/** TODO: Convert all the types to their cross-platform "uint32" "uint8" counterparts **/

const byte sub_table[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x4c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0xaf, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

const byte inv_sub_table[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x8a, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xca, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};


void aes_encrypt(byte* plaintext, byte* key) {
  //will return the given plaintext as ciphertext

  //move the plaintext block into the state
  //might be able to optimize slightly by just leaving the block as-is and doing single-array indexing math, if easy enough
  byte state[4][4];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      state[j][i] = plaintext[(i * 4) + j];  //column-major traversal
  }

  //get round keys
  byte roundkeys[11][16];  //11 rounds for 128-bit key
  aes_keyexpansion(key, roundkeys);

  //initial addroundkey
  aes_addroundkey(state, roundkeys[0]);

  //main bulk of it
  for (int i = 1; i < 9; i++) {
    aes_subbytes(state);
    aes_shiftrows(state);
    aes_mixcolumns(state);
    aes_addroundkey(state, roundkeys[i]);
  }

  //final round
  aes_subbytes(state);
  aes_shiftrows(state);
  aes_addroundkey(state, roundkeys[10]);

  //copy state back into plaintext array as ciphertext
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      plaintext[(i * 4) + j] = state[j][i];
  }
}

void aes_decrypt(byte* ciphertext, byte* key) {
  //will return the given ciphertext as plaintext
  //same as encryption, just done in reverse-order and with inverse operations

  byte state[4][4];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      state[j][i] = ciphertext[(i * 4) + j];
  }

  byte roundkeys[11][16];
  aes_keyexpansion(key, roundkeys);

  aes_addroundkey(state, roundkeys[10]);
  aes_invshiftrows(state);
  aes_invsubbytes(state);

  for (int i = 9; i >= 1; i--) {
    aes_addroundkey(state, roundkeys[i]);
    aes_invmixcolumns(state);
    aes_invshiftrows(state);
    aes_invsubbytes(state);
  }

  aes_addroundkey(state, roundkeys[0]);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      ciphertext[(i * 4) + j] = state[j][i];
  }
}

//step functions
void aes_keyexpansion(byte* key, byte roundkeys[][16]) {
  //expand the key into 11 round keys

  //round constant LUT
  //technically act as 32-bit words, but with how we've written things, and since only the MSB is nonzero, we can treat these as bytes
  const byte round_constants[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

  //generate the new round keys
  //the general algorithm here works on a single array of integers for all the keys, rather than 2d array of bytes.
    //A buffer therefore makes things way easier.
  //4 * 11 = 44 words needed for the new keys

  byte roundkeys_temp[176]; //44 * 4 = 176 bytes needed

  for (int i = 0; i < 44; i++) {
    if (i < 4) {
      roundkeys_temp[i * 4] = key[i * 4];
      roundkeys_temp[(i * 4) + 1] = key[(i * 4) + 1];
      roundkeys_temp[(i * 4) + 2] = key[(i * 4) + 2];
      roundkeys_temp[(i * 4) + 3] = key[(i * 4) + 3];
    }
    else if (i % 4 == 0) {
      //word[i - 4] ^ subword(rotword(word[i - 1]) ^ rconst[i / 4]
      roundkeys_temp[i * 4] = roundkeys_temp[(i - 4) * 4] ^ sub_table[roundkeys_temp[((i - 1) * 4) + 1]] ^ round_constants[(i / 4) - 1];
      roundkeys_temp[(i * 4) + 1] = roundkeys_temp[((i - 4) * 4) + 1] ^ sub_table[roundkeys_temp[((i - 1) * 4) + 2]];
      roundkeys_temp[(i * 4) + 2] = roundkeys_temp[((i - 4) * 4) + 2] ^ sub_table[roundkeys_temp[((i - 1) * 4) + 3]];
      roundkeys_temp[(i * 4) + 3] = roundkeys_temp[((i - 4) * 4) + 3] ^ sub_table[roundkeys_temp[(i - 1) * 4]];
    }
    else {
      roundkeys_temp[i * 4] = roundkeys_temp[(i - 4) * 4] ^ roundkeys_temp[(i - 1) * 4];
      roundkeys_temp[(i * 4) + 1] = roundkeys_temp[((i - 4) * 4) + 1] ^ roundkeys_temp[((i - 1) * 4) + 1];
      roundkeys_temp[(i * 4) + 2] = roundkeys_temp[((i - 4) * 4) + 2] ^ roundkeys_temp[((i - 1) * 4) + 2];
      roundkeys_temp[(i * 4) + 3] = roundkeys_temp[((i - 4) * 4) + 3] ^ roundkeys_temp[((i - 1) * 4) + 3];
    }
  }

  //copy the buffer to the actual roundkeys
  for (int i = 0; i < 176; i++)
    roundkeys[i / 16][i % 16] = roundkeys_temp[i];
}

void aes_addroundkey(byte state[][4], byte* roundkey) {
  //XOR each byte of the state with the respective byte of the subkey state
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      state[j][i] ^= roundkey[(j * 4) + i];
  }
}

void aes_subbytes(byte state[][4]) {
  //replace each byte in the state with the S-box entry corresponding to its index
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      state[i][j] = sub_table[state[i][j]];
  }
}

void aes_shiftrows(byte state[][4]) {
  byte temp;

  //row 1
  temp = state[1][0];
  state[1][0] = state[1][1];
  state[1][1] = state[1][2];
  state[1][2] = state[1][3];
  state[1][3] = temp;
  //row 2
  temp = state[2][0];
  state[2][0] = state[2][2];
  state[2][2] = temp;
  temp = state[2][1];
  state[2][1] = state[2][3];
  state[2][3] = temp;
  //row 3
  temp = state[3][3];
  state[3][3] = state[3][2];
  state[3][2] = state[3][1];
  state[3][1] = state[3][0];
  state[3][0] = temp;
}

void aes_mixcolumns(byte state[][4]) {
  //basic matrix-vector multiplication of each column to diffuse the cipher
  //try and make this parallelizeable

  byte tempstate[4][4]; //store the result here so the initial state isn't altered during the calculation

  for (int i = 0; i < 4; i++) {
    tempstate[0][i] = (2 * state[0][i]) ^ (3 * state[1][i]) ^ state[2][i] ^ state[3][i];
    tempstate[1][i] = state[0][i] ^ (2 & state[1][i]) ^ (3 * state[2][i]) ^ state[3][i];
    tempstate[2][i] = state[0][i] ^ state[1][i] ^ (2 * state[2][i]) ^ (3 * state[3][i]);
    tempstate[3][i] = (3 * state[0][i]) ^ state[1][i] ^ state[2][i] ^ (2 * state[3][i]);

    //copy column to original state - isn't needed anymore in following calculations
    state[0][i] = tempstate[0][i];
    state[1][i] = tempstate[1][i];
    state[2][i] = tempstate[2][i];
    state[3][i] = tempstate[3][i];
  }
}

void aes_invsubbytes(byte state[][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      state[i][j] = inv_sub_table[state[i][j]];
  }
}

void aes_invshiftrows(byte state[][4]) {
  byte temp;

  //row 1
  temp = state[1][3];
  state[1][3] = state[1][2];
  state[1][2] = state[1][1];
  state[1][1] = state[1][0];
  state[1][0] = temp;
  //row 2
  temp = state[2][2];
  state[2][2] = state[2][0];
  state[2][0] = temp;
  temp = state[2][3];
  state[2][3] = state[2][1];
  state[2][1] = temp;
  //row 3
  temp = state[3][0];
  state[3][0] = state[3][1];
  state[3][1] = state[3][2];
  state[3][2] = state[3][3];
  state[3][3] = temp;
}

void aes_invmixcolumns(byte state[][4]) {
  //this might need to be altered. Debug and see what happens. We might need to XOR or modulus here.
  byte tempstate[4][4];

  for (int i = 0; i < 4; i++) {
    tempstate[0][i] = (14 * state[0][i]) ^ (11 * state[1][i]) ^ (13 * state[2][i]) ^ (9 * state[3][i]);
    tempstate[1][i] = (9 * state[0][i]) ^ (14 * state[1][i]) ^ (11 * state[2][i]) ^ (13 * state[3][i]);
    tempstate[2][i] = (13 * state[0][i]) ^ (9 * state[1][i]) ^ (14 * state[2][i]) ^ (11 * state[3][i]);
    tempstate[3][i] = (11 * state[0][i]) ^ (13 * state[1][i]) ^ (9 * state[2][i]) ^ (14 * state[3][i]);

    state[0][i] = tempstate[0][i];
    state[1][i] = tempstate[1][i];
    state[2][i] = tempstate[2][i];
    state[3][i] = tempstate[3][i];
  }
}
