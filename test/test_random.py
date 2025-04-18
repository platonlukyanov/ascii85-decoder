import unittest
import base64
import subprocess
import random
import string

ASCII85_EXEC_PATH = './ascii85'

RANDOM_SEED = 42;

random.seed(RANDOM_SEED)

def generate_random_string():
    length = random.randint(1, 20)
    alphabet = string.ascii_letters + 'йцукенгшщзхъфывапролджэячсмитьбю12345'
    word = ''.join(random.choice(alphabet) for _ in range(length))
    return word

def encode_to_ascii85_with_exec(text: str):
    result = subprocess.run([ASCII85_EXEC_PATH], input=text, capture_output=True, text=True)

    return {
        "content": result.stdout.strip(),
        "code": result.returncode,
    }

def decode_from_ascii85_with_exec(text: str):
    result = subprocess.run([ASCII85_EXEC_PATH, "-d"], input=text, capture_output=True, text=True)
    return {
        "content": result.stdout.strip(),
        "code": result.returncode,
    }

class TestASCII85Exec(unittest.TestCase):
    def test_encoding(self):
        test_cases = ["Hello", "Привет", "", "123", "У меня есть 5% от всей компании", "!@#$%^"]

        
        random_test_cases = [generate_random_string() for i in range(100)]
        test_cases.extend(random_test_cases)

        for test_string in test_cases:
            with self.subTest(text=test_string):
                expected_ascii85 = base64.a85encode(test_string.encode("utf8")).decode('ascii')
                result = encode_to_ascii85_with_exec(test_string)
                self.assertEqual(result["code"], 0)
                self.assertEqual(result["content"], expected_ascii85)

    def test_decoding_positive(self):
        test_original = ["Hello", "Привет", "123", "У меня есть 5% от всей компании", "!@#$%^"]
        random_test_cases = [generate_random_string() for i in range(100)]
        test_original.extend(random_test_cases)
        test_cases = list(map(lambda x: base64.a85encode(x.encode("utf-8")).decode('ascii'), test_original))
    
        for original, encoded in zip(test_original, test_cases):
            with self.subTest(text=encoded):
                result = decode_from_ascii85_with_exec(encoded)
                self.assertEqual(result["code"], 0)
                self.assertEqual(result["content"], original)

    def test_decoding_negative(self):
        test_failtures = ["", "Привет"]

        for test_failture in test_failtures:
            with self.subTest(text=test_failture):
                result = decode_from_ascii85_with_exec(test_failture)
                self.assertEqual(result["code"], 1)
    

if __name__ == '__main__':
    unittest.main()    
