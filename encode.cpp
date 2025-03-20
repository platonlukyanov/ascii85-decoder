#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

const std::string alphabet = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstu";

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

int main()
{
	std::vector<std::byte> buffer = readBytes();
	
	for (char c : encodeBytesToASCII85(buffer)) {
		std::cout << c;
	}
	std::cout << std::endl;
	return 0;
}
