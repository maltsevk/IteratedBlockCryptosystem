
#include "CryptoProvider.h"

#include <iostream>
#include <windows.h>

#define CIPHERTEXT_FILENAME "ciphertext.txt"

int main(int argc, char* argv[])
{
	std::vector<unsigned short> substitutionBox = { 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 };
	std::string openedText, cipherText;
	std::string keyString;
	unsigned int key, result;

	// reading opened text from file
	result = readFile(argv[1], openedText);
	if (result == 0) {
		return 1;
	}
	else if (openedText.size() / BLOCK_SIZE < 2) {
		std::cout << "[ ERROR ] The opened text length is too short" << std::endl;
	}

	// reading key from file
	result = readFile(argv[2], keyString);
	if (result == 0) {
		return 1;
	}
	else if (keyString.size() != BLOCK_SIZE) {
		std::cout << "[ ERROR ] The key length is wrong" << std::endl;
	}

	key = convertStringKeyToInt(keyString);

	// encryption via two Lai-Massey and two Feistel cycles
	encrypt(openedText, key, substitutionBox, cipherText);

	writeFile(cipherText, CIPHERTEXT_FILENAME);

	return 0;
}