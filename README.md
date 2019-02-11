# IteratedBlockCryptosystem
It is an iterated block encryptor and decryptor. 

Cryptosystem description.
It uses two Lai-Massey and two Feistel tacts. The round function is SP network.
Key schedule: 0 = 01 || 02, KS(0) = (01, 02, 01, 02).
All S-blocks are the same:
Substitution: (3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5)
Permutation: (5 * i + 9) % 16

Note:
This cryptosystem has weak keys (any keys that have the same left and right part).

Encryptor
Input: 
 - binary file with a opened text to be encrypted;
 - binary file with a key (4 bytes).
Output:
 - binary file with an encrypted text.

Decryptor
Input: 
 - binary file with a encrypted text to be decrypted;
 - binary file with a key (4 bytes).
Output:
 - binary file with a decrypted text.
