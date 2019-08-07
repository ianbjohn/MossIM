#include "aes.h"

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
  byte roundkeys[10][32];  //10 rounds for 128-bit key
  aes_keyexpansion(key, roundkeys);

  //initial addroundkey
  aes_addroundkey(state, roundkeys[0]);

  //main bulk of it
  for (int i = 0; i < 9; i++) {
    aes_subbytes(state);
    aes_shiftrows(state);
    aes_mixcolumns(state);
    aes_addroundkey(state, roundkeys[i]);
  }

  //final round
  aes_subbytes(state);
  aes_shiftrows(state);
  aes_addroundkey(state, roundkeys[9]);
}

void aes_decrypt(byte* ciphertext, byte* key) {
  //will return the given ciphertext as plaintext
  //same as encryption, just done in reverse-order and with inverse operations

  byte state[4][4];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++)
      state[j][i] = ciphertext[(i * 4) + j];
  }

  byte roundkeys[10][32];
  aes_keyexpansion(key, roundkeys);

  aes_addroundkey(state, roundkeys[9]);
  aes_invshiftrows(state);
  aes_invsubbytes(state);

  for (int i = 9; i >= 0; i--) {
    aes_addroundkey(state, roundkeys[i]);
    aes_invmixcolumns(state);
    aes_invshiftrows(state);
    aes_invsubbytes(state);
  }

  aes_addroundkey(state, roundkeys[0]);
}

//step functions
void aes_keyexpansion(byte* key, byte** roundkeys) {

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

}

void aes_mixcolumns(byte** state) {

}

void aes_invsubbytes(byte** state) {

}

void aes_invshiftrows(byte** state) {

}

void aes_invmixcolumns(byte** state) {

}
