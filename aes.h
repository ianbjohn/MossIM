//Implementation of the Advanced Encryption Standard
//Used to encrypt / decrypt data sent by server/client
//128-bit keys are used
//function implementation subject to change, just a rough outline for now

typedef unsigned char byte;

void aes_encrypt(byte* plaintext, byte* key);
void aes_decrypt(byte* ciphertext, byte* key);

//step functions
void aes_keyexpansion(byte* key, byte** roundkeys);
void aes_addroundkey(byte** state, byte* key);
void aes_subbytes(byte** state);
void aes_shiftrows(byte** state);
void aes_mixcolumns(byte** state);
void aes_invsubbytes(byte** state);
void aes_invshiftrows(byte** state);
void aes_invmixcolumns(byte** state);
