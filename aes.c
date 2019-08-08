#include "aes.h"

/** TODO: Convert all the types to their cross-platform "uint32" "uint8" counterparts **/

#define SUBTABLE_LOOKUP(b) sub_table[(b >> 4) ^ (b & 0x0f)] //high nibble determines column, low nibble row
#define INVSUBTABLE_LOOKUP(b) inv_sub_table[(b >> 4) ^ (b & 0x0f)]

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
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/*
const byte inv_sub_table[256] = {

};
*/


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
void aes_keyexpansion(byte* key, byte** roundkeys) {
  //expand the key into 10 round keys

  //constant LUT (don't quite know what these are for yet)
  const byte round_counstants[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

  //generate the new round keys
  //the general algorithm here works on a single array of integers for all the keys, rather than a 2d array of bytes
  //din't want to just copy over what's essentially redundant data and then copy it back, hence all the pointer crazyness 
  //4 * 11 = 44 words needed for the new keys
  //DEBUG TO MAKE SURE THIS WORKS AS INTENDED!!!!!!!!!!!!!!!!!!!!!!!!
  for (int i = 0; i < 44; i++) {
    if (i < 4)
      ((unsigned int** ) roundkeys)[i / 11][i % 4] = ((unsigned int* ) key)[i];
    else if (i % 4 == 0) {
      unsigned int temp = ((unsigned int** ) roundkeys)[(i - 1) / 11][(i - 1) % 4];
      temp = aes_keyexpansion_rotword(temp);
      temp = aes_keyexpansion_subword(temp);
      ((unsigned int** ) roundkeys)[(i - 4) / 11][(i - 4) % 4] ^= temp;
    }
    else
      ((unsigned int** ) roundkeys)[(i - 4) / 11][(i - 4) % 4] ^= ((unsigned int** ) roundkeys)[(i - 1) / 11][(i - 1) % 4];
  }
}

//key expansion helper functions
unsigned int aes_keyexpansion_rotword(unsigned int w) {
  //shifts the bytes of a given 32-bit word to the left
  byte* rot = (byte* ) &w;
  byte temp;

  temp = rot[0];
  rot[0] = rot[1];
  rot[1] = rot[2];
  rot[2] = rot[3];
  rot[3] = temp;
}

unsigned int aes_keyexpansion_subword(unsigned int w) {
  //idk what this does yet
}

void aes_addroundkey(byte** state, byte* roundkey) {
  //XOR each byte of the state with the respective byte of the subkey state
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      state[j][i] ^= roundkey[(j * 4) + i];
  }
}

void aes_subbytes(byte** state) {

}

void aes_shiftrows(byte** state) {
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

void aes_mixcolumns(byte** state) {

}

void aes_invsubbytes(byte** state) {

}

void aes_invshiftrows(byte** state) {
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

void aes_invmixcolumns(byte** state) {

}
