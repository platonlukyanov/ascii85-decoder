#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>

const std::string alphabet = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstu";

std::unordered_map<char, int> alphabetLookup;

void initAlphabetLookup() {
    for (int i = 0; i < alphabet.size(); ++i) {
        alphabetLookup[alphabet[i]] = i;
    }
}

bool isLittleEndian() {
	uint32_t num = 1;
	return (*(uint8_t*)&num == 1);
}

std::vector<int> convertToBase(int decimal, int base) {
    std::vector<int> digits;

    while (decimal > 0) {
        digits.push_back(decimal % base);
        decimal /= base;
    }

    std::reverse(digits.begin(), digits.end());

    return digits;
}

std::string readString() {
	std::string resultingStr;
	char currentChar; 
    	
	while(std::cin.get(currentChar)) {
		resultingStr += currentChar;
    	}
    
    	return resultingStr;
}

std::string cleanASCII85(std::string& ascii85Text) {
	std::string result;

	for (char character : ascii85Text) {
		if (character == 'z') {
			result.append("!!!!!");
			continue;
		}

		if (character == ' ' || character == '\n' || character == '\r') {
			continue;
		}
		result.push_back(character);
	}

	return result;
}

std::string padASCII85String(std::string& cleanedAscii85Text) {
	std::string result = cleanedAscii85Text;
	int paddingNeeded = (5 - result.length() % 5) % 5;

	for (int i = 0; i < paddingNeeded; i++) {
		result.append("u");
	}
	return result;
}

std::vector<uint8_t> convertIntToBytes(uint32_t value, size_t numberOfBytes, int padding = 0) {
	bool isLE = isLittleEndian();
	std::vector<uint8_t> bytes;
	bytes.reserve(numberOfBytes);
	for (size_t i = 0; i < numberOfBytes - padding; ++i) {
		size_t shift = !isLE ? i * 8 : (numberOfBytes - 1 - i) * 8;
		uint8_t byte = (value >> shift) & 0xFF;
		bytes.push_back(byte);
	}

	for (size_t i = 0; i < padding; ++i) {
		bytes.push_back(0x00);
	}

	return bytes;
}

std::string decodeASCII85(std::string& codedText) {
	std::string result;
	std::string cleaned = cleanASCII85(codedText);
	std::string cleanedAndPadded = padASCII85String(cleaned);

	int padding = (5 - cleaned.length() % 5) % 5;
	
	int start = 0;

	while (start + 4 < cleanedAndPadded.length()) {
		uint32_t value = 0;
		for (int i = 0; i <= start + 4; i++) {
			int character = cleanedAndPadded[start + i];
			int multiplier = alphabetLookup[character];
			if (multiplier < 0) {
				continue;
			}
			int degree = std::pow(85, 4 - i);
			value += multiplier * degree;	
		}
		start += 5;
		std::vector<uint8_t> bytes = convertIntToBytes(value, 4, start == cleanedAndPadded.length() ? padding : 0);

		result.insert(result.end(), bytes.begin(), bytes.end());
	}

	return result;
}

int main()
{
	initAlphabetLookup();
	std::string buffer = readString();
	std::cout << decodeASCII85(buffer) << std::endl;
	return 0;
}
