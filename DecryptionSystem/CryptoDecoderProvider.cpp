
#include "CryptoDecoderProvider.h"

#include <iostream>
#include <fstream>

int readFile(const std::string & path, std::string & buffer)
{
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open()) {
		std::cout << "[ ERROR ] Can not open the file" << std::endl;
		return 0;
	}

	buffer = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	if (buffer.size() == 0) {
		std::cout << "[ ERROR ] The file is empty" << std::endl;
		return 0;
	}

	std::cout << "[ SUCCESS ] The data was read from " << path.c_str() << std::endl;
	return buffer.size();
}

void writeFile(const std::string & text, const std::string & filename)
{
	std::ofstream file(filename, std::ios::binary);
	file.write(text.c_str(), text.size());
	file.close();

	std::cout << "[ SUCCESS ] The encrypted text was written to " << filename.c_str() << std::endl;
}

unsigned int convertStringKeyToInt(const std::string & keyString)
{
	unsigned int key = 0;

	for (int i = 0; i < BLOCK_SIZE; i++)
		key |= (unsigned int)keyString[i] << ((BLOCK_SIZE - (i + 1)) * 8);

	return key;
}

// this function has the form of SP cryptosystem
unsigned short roundFunction(unsigned short subBlock, unsigned short key, std::vector<unsigned short> & substitutionBox)
{
	unsigned short ultimateResult = 0;
	unsigned short result = subBlock ^ key;
	unsigned short newBitPosition;
	unsigned short currentBit;

	// substitution (S)
	result = (substitutionBox[(result >> 12) % 16] << 12) +
		(substitutionBox[(result >> 8) % 16] << 8) +
		(substitutionBox[(result >> 4) % 16] << 4) +
		(substitutionBox[(result >> 0) % 16] << 0);

	// permutation (P)
	for (int i = 0; i < 16; i++) {
		newBitPosition = (5 * i + 9) % 16;
		currentBit = (result >> (15 - i)) % 2;
		ultimateResult |= (currentBit << (15 - newBitPosition));
	}

	return ultimateResult;
}

unsigned int decryptBlock(unsigned int block, unsigned int key, std::vector<unsigned short> & substitutionBox)
{
	// dividing the block in the reverse order
	unsigned short leftPart = (unsigned short)(block);
	unsigned short rightPart = (unsigned short)(block >> 16);
	unsigned short roundFunctionResult;

	// record the ker in the reverse order
	unsigned short keyArray[2] = { unsigned short(key), (unsigned short)(key >> 16) };

	// Feistel substitution (two times)
	for (int i = 0; i < 2; i++) {
		roundFunctionResult = roundFunction(leftPart, keyArray[i], substitutionBox);
		roundFunctionResult ^= rightPart;
		rightPart = leftPart;
		leftPart = roundFunctionResult;
	}

	std::swap(leftPart, rightPart);

	// Lai-Massey substitution (two times)
	for (int i = 0; i < 2; i++) {
		roundFunctionResult = roundFunction(leftPart ^ rightPart, keyArray[i], substitutionBox);
		leftPart ^= roundFunctionResult;
		rightPart ^= roundFunctionResult;
	}

	return ((unsigned int)leftPart << 16) | rightPart;
}

unsigned int getLeftPart(unsigned int number, short partSize)
{
	return (number >> ((sizeof(unsigned int) - partSize) * 8)) <<
		((sizeof(unsigned int) - partSize) * 8);
}

unsigned int getRightPart(unsigned int number, short partSize)
{
	return (number << ((sizeof(unsigned int) - partSize) * 8)) >>
		((sizeof(unsigned int) - partSize) * 8);
}

unsigned int getBlock(const std::string & openedText, int blockIndex)
{
	unsigned int symbol;
	unsigned int block = 0;
	int limit = (openedText.size() - blockIndex * BLOCK_SIZE < BLOCK_SIZE) ?
		(openedText.size() % BLOCK_SIZE) : BLOCK_SIZE;

	for (int i = 0; i < limit; i++) {
		symbol = (unsigned char)(openedText[blockIndex * BLOCK_SIZE + i]);
		block |= (symbol << ((BLOCK_SIZE - (i + 1)) * 8));
	}

	return block;
}

void appendBlockToString(std::string & text, unsigned int block, int blockSize)
{
	for (int i = 0; i < blockSize; i++)
		text += (char)(block >> ((BLOCK_SIZE - (i + 1)) * 8));
}

int decrypt(const std::string & cipherText, unsigned int key, std::vector<unsigned short> & substitutionBox, std::string & decryptedText)
{
	unsigned int block, result;
	unsigned short residualLength = cipherText.size() % BLOCK_SIZE;
	int nBlocks = (residualLength == 0) ?
		cipherText.size() / BLOCK_SIZE :
		cipherText.size() / BLOCK_SIZE - 1;

	for (int blockIndex = 0; blockIndex < nBlocks; blockIndex++) {
		// taking the block from the opened text
		block = getBlock(cipherText, blockIndex);

		// decryption by Lai-Massey and Feistel
		result = decryptBlock(block, key, substitutionBox);
		appendBlockToString(decryptedText, result, BLOCK_SIZE);
	}

	// processing the last two blocks if have 
	if (residualLength != 0) {
		unsigned int auxiliaryBlock, lastCipherBlock;

		// taking the penultimate block
		block = getBlock(cipherText, nBlocks);
		result = decryptBlock(block, key, substitutionBox);
		auxiliaryBlock = getRightPart(result, BLOCK_SIZE - residualLength);

		// taking the last block
		block = getBlock(cipherText, nBlocks + 1);
		lastCipherBlock = getLeftPart(result, residualLength);
		result = decryptBlock(block | auxiliaryBlock, key, substitutionBox);

		appendBlockToString(decryptedText, result, BLOCK_SIZE);
		appendBlockToString(decryptedText, lastCipherBlock, residualLength);
	}

	return 0;
}