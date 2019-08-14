//Implementation of the Advanced Encryption Standard
//Used to encrypt / decrypt data sent by server/client
//128-bit keys are used
//function implementation subject to change, just a rough outline for now

typedef unsigned char byte;

const byte sub_table[256];
const byte inv_sub_table[256];

void aes_encrypt(byte* plaintext, byte* key);
void aes_decrypt(byte* ciphertext, byte* key);

//step functions
void aes_keyexpansion(byte* key, byte roundkeys[][16]);
void aes_addroundkey(byte state[][4], byte* key);
void aes_subbytes(byte state[][4]);
void aes_shiftrows(byte state[][4]);
void aes_mixcolumns(byte state[][4]);
void aes_invsubbytes(byte state[][4]);
void aes_invshiftrows(byte state[][4]);
void aes_invmixcolumns(byte state[][4]);

//helper functions
byte aes_mult_mod(byte a, byte b); //weird multiplication with modulo that AES uses
