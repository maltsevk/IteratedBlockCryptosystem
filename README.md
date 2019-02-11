# IteratedBlockCryptosystem
It is an iterated block encryptor and decryptor. 

Cryptosystem description.<br>
It uses two Lai-Massey and two Feistel tacts. The round function is SP network.<br>
Key schedule: θ = θ<sub>1</sub> || θ<sub>2</sub>, KS(θ) = (θ<sub>1</sub>, θ<sub>2</sub>, θ<sub>1</sub>, θ<sub>2</sub>).<br>
All S-blocks are the same:<br>
Substitution: (3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5)<br>
Permutation: (5 * i + 9) % 16<br>

Note: this cryptosystem has weak keys (any keys that have the same left and right part).

<br>Encryptor input:
<li>binary file with a opened text to be encrypted;</li>
<li>binary file with a key (4 bytes).</li>
Encryptor output:
<li>binary file with an encrypted text.</li>
<br>Decryptor input:
<li>binary file with a encrypted text to be decrypted;</li>
<li>binary file with a key (4 bytes).</li>
Decryptor output:
<li>binary file with a decrypted text.</li>
