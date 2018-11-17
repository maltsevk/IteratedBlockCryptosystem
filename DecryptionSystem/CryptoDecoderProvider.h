#ifndef _CRYPTODECODERPROVIDER_H
#define _CRYPTODECODERPROVIDER_H

#include <vector>

#define BLOCK_SIZE 4

// functions for encryption
int decrypt(const std::string &, unsigned int, std::vector<unsigned short> &, std::string &);
unsigned int convertStringKeyToInt(const std::string &);

// functions for work with files
int readFile(const std::string &, std::string &);
void writeFile(const std::string &, const std::string &);

#endif // _CRYPTODECODERPROVIDER_H