
#include "CryptoDecoderProvider.h"

#include <iostream>

#define DECRYPTED_TEXT_FILENAME "decrypted_text.txt"

int main(int argc, char* argv[])
{
	std::string decryptedText, cipherText;
	std::string keyString;
	unsigned int key, result;

	// reading ciphertext from file
	result = readFile(std::string(argv[1]), cipherText);
	if (result == 0) {
		return 1;
	}
	else if (cipherText.size() / BLOCK_SIZE < 2) {
		std::cout << "[ ERROR ] The ciphertext length is too short" << std::endl;
	}

	// reading key from file
	result = readFile(std::string(argv[2]), keyString);
	if (result == 0) {
		return 1;
	}
	else if (keyString.size() != BLOCK_SIZE) {
		std::cout << "[ ERROR ] The key length is wrong" << std::endl;
	}

	key = convertStringKeyToInt(keyString);

	// decryption via two Lai-Massey and two Feistel cycles
	decrypt(cipherText, key, decryptedText);

	writeFile(decryptedText, DECRYPTED_TEXT_FILENAME);

	return 0;
}