#include "aes.h"

/** TODO: Convert all the types to their cross-platform "uint32" "uint8" counterparts **/


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
  byte roundkeys[11][32];  //11 rounds for 128-bit key
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

  byte roundkeys[11][32];
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

  //constant LUT
  const byte round_counstants[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

  //generate the new round keys
  //the general algorithm here works on a single array of integers for all the keys, rather than a 2d array of bytes
  //din't want to just copy over what's essentially redundant data and then copy it back, hence all the pointer crazyness 
  //4 * 11 = 44 words needed for the new keys
  //DEBUG TO MAKE SURE THIS WORKS AS INTENDED!!!!!!!!!!!!!!!!!!!!!!!!
  for (int i = 0; i < 44; i++) {
    if (i < 4)
      ((unsigned int** ) roundkeys)[i / 11][i % 11] = ((unsigned int* ) key)[i];
    else if (i % 4 == 0) {
      unsigned int temp = ((unsigned int** ) roundkeys)[(i - 1) / 11][(i - 1) % 11];
      temp = aes_keyexpansion_rotword(temp);
      temp = aes_keyexpansion_subword(temp);
      ((unsigned int** ) roundkeys)[(i - 4) / 11][(i - 4) % 11] ^= temp;
    }
    else
      ((unsigned int** ) roundkeys)[(i - 4) / 11][(i - 4) % 11] ^= ((unsigned int** ) roundkeys)[(i - 1) / 11][(i - 1) % 11];
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
