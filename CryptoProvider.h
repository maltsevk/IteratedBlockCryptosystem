#ifndef _CRYPTOPROVIDER_H
#define _CRYPTOPROVIDER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define BLOCK_SIZE 4

int encrypt(const std::string &, uint32_t, std::string &);
uint32_t convertStringKeyToInt(const std::string &);

int readFile(const std::string &, std::string &);
void writeFile(const std::string &, const std::string &);

#endif // _CRYPTOPROVIDER_H