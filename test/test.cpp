
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cstdint>

extern const std::string alphabet;
extern std::unordered_map<char, int> alphabetLookup;
void initAlphabetLookup();
bool isLittleEndian();
std::vector<int> convertToBase(uint32_t decimal, int base);
std::vector<uint32_t> readBytesByFour(const std::vector<uint8_t>& data);
std::vector<char> encodeBytesToASCII85(std::vector<std::byte>& bytes);
std::string decodeASCII85(std::string& codedText);
std::vector<uint8_t> convertIntToBytes(uint32_t value, size_t numberOfBytes, int padding = 0);

class LookupTest : public ::testing::Test {
protected:
    void SetUp() override {
        initAlphabetLookup();
    }
};

TEST_F(LookupTest, AlphabetInitialization) {
    ASSERT_EQ(alphabetLookup.size(), alphabet.size());
    EXPECT_EQ(alphabetLookup['A'], 32);
}

TEST(EndianTest, ConsistentDetection) {
    bool result = isLittleEndian();
    
    union {
        uint32_t i;
        uint8_t c[4];
    } test = {0x01020304};
    
    bool actual = (test.c[0] == 0x04);
    ASSERT_EQ(result, actual);
}

TEST(ByteConversion, PartialChunkHandling) {
    std::vector<uint8_t> input = {0x01, 0x02, 0x03};
    auto result = readBytesByFour(input);
    
    uint32_t expected = 0x01020300;
    
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], expected);
}

TEST(EncodingDecoding, RoundTrip) {
    std::vector<std::byte> original = {
        std::byte{'T'}, std::byte{'e'}, std::byte{'s'}, std::byte{'t'}
    };
    
    auto encoded = encodeBytesToASCII85(original);
    std::string encodedStr(encoded.begin(), encoded.end());
    
    std::string decoded = decodeASCII85(encodedStr);
    std::vector<std::byte> decodedBytes(
        reinterpret_cast<std::byte*>(decoded.data()),
        reinterpret_cast<std::byte*>(decoded.data() + decoded.size())
    );
    
    ASSERT_EQ(original, decodedBytes);
}

TEST(DecodingTest, InvalidCharacterHandling) {
    initAlphabetLookup();
    std::string invalidInput = "Abc~";
    
    EXPECT_THROW({
        decodeASCII85(invalidInput);
    }, std::invalid_argument);
}

TEST(BaseConversion, EdgeCases) {
    struct TestCase {
        uint32_t input;
        int base;
        std::vector<int> expected;
    };
    
    TestCase cases[] = {
        {0, 85, {}},
        {1, 2, {1}},
        {84, 85, {84}},
        {85, 85, {1, 0}}
    };
    
    for (const auto& tc : cases) {
        auto result = convertToBase(tc.input, tc.base);
        EXPECT_EQ(result, tc.expected) 
            << "Input: " << tc.input << " Base: " << tc.base;
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
