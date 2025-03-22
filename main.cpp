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

std::vector<int> convertToBase(uint32_t decimal, int base) {
    std::vector<int> digits;

    while (decimal > 0) {
        digits.push_back(decimal % base);
        decimal /= base;
    }

    std::reverse(digits.begin(), digits.end());

    return digits;
}

std::vector<uint32_t> readBytesByFour(const std::vector<uint8_t>& data) {
	const bool littleEndian = isLittleEndian();

	std::vector<uint32_t> values;
	size_t index = 0;

	while (index < data.size()) {
		uint8_t chunk[4] = {0x00, 0x00, 0x00, 0x00};
		const size_t remaining = data.size() - index;
		const size_t copySize = (remaining > 4) ? 4 : remaining;

		for (size_t i = 0; i < copySize; ++i) {
			chunk[i] = data[index + i]; // Если copySize меньше 4, то оставшиеся байты останутся нулевыми
		}

		uint32_t value = 0;

		if (!littleEndian) {
			value = chunk[0] | (chunk[1] << 8) | (chunk[2] << 16) | (chunk[3] << 24);
		} else {
			value = (chunk[0] << 24) | (chunk[1] << 16) | (chunk[2] << 8) | chunk[3];
		}
		values.push_back(value);

		index += copySize;
	}
	return values;
}

std::vector<std::byte> readBytes() {
    std::vector<std::byte> bytes;
    std::byte byte;
    
    while(std::cin.get(reinterpret_cast<char&>(byte))) {
        bytes.push_back(byte);
    }
    
    return bytes;
}

std::vector<char> encodeBytesToASCII85(std::vector<std::byte>& bytes) {
    	std::vector<uint8_t> uint8Bytes;
    	uint8Bytes.reserve(bytes.size());
    
    	for (std::byte b : bytes) {
        	uint8Bytes.push_back(static_cast<uint8_t>(b));
    	}
    	std::vector<uint32_t> values = readBytesByFour(uint8Bytes);  
	std::vector<char> resultLetters;

    	for (uint32_t value : values) {
		if (value == 0) {
			resultLetters.push_back('z');
			continue;
		}
		std::vector<int> letterIndexes = convertToBase(value, 85);
		for (int index : letterIndexes) {
			resultLetters.push_back(alphabet[index]);
		}
    	}
	
	// Correctly handle padding
	size_t originalBytes = (uint8Bytes.size() % 4);
	if (originalBytes != 0) {
    		size_t paddingCount = 5 - (originalBytes * 5 / 4);
    		if (paddingCount > 0) {
        		resultLetters.resize(resultLetters.size() - paddingCount + 1);
    		}
	}
	return resultLetters;
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

	return bytes;
}

std::string decodeASCII85(std::string& codedText) {
	std::string result;
	std::string cleaned = cleanASCII85(codedText);
	std::string cleanedAndPadded = padASCII85String(cleaned);
	if (cleaned.empty()) {
		throw std::invalid_argument("Empty string after removing whitespace characters");
	}

	int padding = (5 - cleaned.length() % 5) % 5;
	
	int start = 0;

	while (start + 4 < cleanedAndPadded.length()) {
		uint32_t value = 0;
		for (int i = 0; i <= start + 4; i++) {
			if (start + i == cleanedAndPadded.length()) {
				break;
			}
			char character = cleanedAndPadded[start + i];
			if (alphabetLookup.count(character) <= 0) {
				throw std::invalid_argument("Invalid non-ASCII85 characters " + std::to_string(cleanedAndPadded[start + i]));
			}
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

int main(int argc, char* argv[])
{
	std::string mode = "e";
	if (argc > 1) {
		std::string arg(argv[1]);
		
		if (arg == "-d") {
			mode = "d";
		}
	}

	if (mode == "d") {
		try {
			initAlphabetLookup();
			std::string buffer = readString();
			std::cout << decodeASCII85(buffer) << std::endl;
		} catch (const std::exception& e) {
			std::cerr << "Decoding Error: " << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	
	if (mode == "e") {
		try {
			std::vector<std::byte> buffer = readBytes();
	
			for (char c : encodeBytesToASCII85(buffer)) {
				std::cout << c;
			}
			std::cout << std::endl;
		} catch (const std::exception& e) {
			std::cerr << "Encoding Error: " << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	
	return 0;
}
