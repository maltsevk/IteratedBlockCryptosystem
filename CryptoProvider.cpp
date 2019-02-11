
#include "CryptoProvider.h"

int readFile(const std::string & path, std::string & buffer)
{
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open()) {
		std::cout << "[-] Can not open the file" << std::endl;
		return 0;
	}

	buffer = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	if (buffer.size() == 0) {
		std::cout << "[-] The file is empty" << std::endl;
		return 0;
	}

	file.close();
	std::cout << "[+] The data was read from " << path.c_str() << std::endl;
	return buffer.size();
}

void writeFile(const std::string & text, const std::string & filename)
{
	std::ofstream file(filename, std::ios::binary);
	file.write(text.c_str(), text.size());
	file.close();

	std::cout << "[+] The encrypted text was written to " << filename << std::endl;
}

uint32_t convertStringKeyToInt(const std::string & keyString)
{
	uint32_t key = 0;

	for (int i = 0; i < BLOCK_SIZE; i++) {
		key |= (uint32_t)keyString[i] << ((BLOCK_SIZE - (i + 1)) * 8);
	}

	return key;
}

uint16_t substitution(uint16_t inputValue)
{
	std::vector<uint16_t> substitutionBox = { 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 };

	return (substitutionBox[(inputValue >> 12) % 16] << 12) +
		(substitutionBox[(inputValue >> 8) % 16] << 8) +
		(substitutionBox[(inputValue >> 4) % 16] << 4) +
		(substitutionBox[(inputValue >> 0) % 16] << 0);
}

uint16_t permutation(uint16_t inputValue)
{
	uint16_t result = 0;
	uint16_t newBitPosition;
	uint16_t currentBitValue;

	for (int i = 0; i < 16; i++) {
		newBitPosition = (5 * i + 9) % 16;
		currentBitValue = (inputValue >> (15 - i)) % 2;
		result |= (currentBitValue << (15 - newBitPosition));
	}

	return result;
}

uint16_t roundFunction(uint16_t subBlock, uint16_t key)
{
	uint16_t substitutionResult = substitution(subBlock ^ key);
	uint16_t permutationResult = permutation(substitutionResult);

	return permutationResult;
}

uint32_t laiMasseyScheme(uint32_t block, uint16_t key)
{
	uint16_t leftPart = (uint16_t)(block >> 16);
	uint16_t rightPart = (uint16_t)(block);

	uint16_t roundFunctionResult = roundFunction(leftPart ^ rightPart, key);
	leftPart ^= roundFunctionResult;
	rightPart ^= roundFunctionResult;

	return ((uint32_t)leftPart << 16) | rightPart;
}

uint32_t feistelNetwork(uint32_t block, uint16_t key)
{
	uint16_t leftPart = (uint16_t)(block >> 16);
	uint16_t rightPart = (uint16_t)(block);

	uint16_t roundFunctionResult = roundFunction(rightPart, key);
	roundFunctionResult ^= leftPart;
	leftPart = rightPart;
	rightPart = roundFunctionResult;

	return ((uint32_t)leftPart << 16) | rightPart;
}

uint32_t encryptBlock(uint32_t block, uint32_t key)
{
	uint16_t keys[2] = { (uint16_t)(key >> 16), (uint16_t)key };
	uint32_t result;
	
	result = laiMasseyScheme(block, keys[0]);
	result = laiMasseyScheme(result, keys[1]);
	result = feistelNetwork(result, keys[0]);
	result = feistelNetwork(result, keys[1]);

	// return in the reverse order (involutivity)
	result = (result << 16) | (result >> 16);
	return result;
}

uint32_t getLeftPart(uint32_t number, short partSize)
{
	uint8_t shift = (BLOCK_SIZE - partSize) * 8;
	return (number >> shift) << shift;
}

uint32_t getRightPart(uint32_t number, short partSize)
{
	uint8_t shift = (BLOCK_SIZE - partSize) * 8;
	return (number << shift) >> shift;
}

uint32_t getBlock(const std::string & openedText, int blockIndex)
{
	uint32_t symbol;
	uint32_t block = 0;
	int limit = (openedText.size() - blockIndex * BLOCK_SIZE < BLOCK_SIZE) ?
		(openedText.size() % BLOCK_SIZE) : BLOCK_SIZE;

	for (int i = 0; i < limit; i++) {
		symbol = (unsigned char)(openedText[blockIndex * BLOCK_SIZE + i]);
		block |= (symbol << ((BLOCK_SIZE - (i + 1)) * 8));
	}

	return block;
}

void appendBlockToString(std::string & text, uint32_t block, int blockSize)
{
	for (int i = 0; i < blockSize; i++)
		text += (char)(block >> ((BLOCK_SIZE - (i + 1)) * 8));
}

int encrypt(const std::string & openedText, uint32_t key, std::string & cipherText)
{
	uint32_t block, result;
	uint16_t residualLength = openedText.size() % BLOCK_SIZE;
	int blockCount = (residualLength == 0) ?
		openedText.size() / BLOCK_SIZE : 
		openedText.size() / BLOCK_SIZE - 1;

	for (int blockIndex = 0; blockIndex < blockCount; blockIndex++) {
		// taking the block from the opened text
		block = getBlock(openedText, blockIndex);

		// encryption by Lai-Massey and Feistel
		result = encryptBlock(block, key);
		appendBlockToString(cipherText, result, BLOCK_SIZE);
	}

	// processing the last two blocks if have not full block
	if (residualLength != 0) {
		uint32_t auxiliaryBlock, lastCipherBlock;

		// taking the penultimate block
		block = getBlock(openedText, blockCount);
		result = encryptBlock(block, key);
		auxiliaryBlock = getRightPart(result, BLOCK_SIZE - residualLength);

		// taking the last block
		block = getBlock(openedText, blockCount + 1);
		lastCipherBlock = getLeftPart(result, residualLength);
		result = encryptBlock(block | auxiliaryBlock, key);

		appendBlockToString(cipherText, result, BLOCK_SIZE);
		appendBlockToString(cipherText, lastCipherBlock, residualLength);
	}

	return 0;
}