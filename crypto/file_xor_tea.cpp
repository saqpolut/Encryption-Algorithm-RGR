#include "file_xor_tea.h"
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

namespace Crypto {
    void process_file(const char* input_path, const char* output_path,
                      const char* key, bool encrypt) {
        ifstream in(input_path, ios::binary);
        ofstream out(output_path, ios::binary);
        
        vector<uint8_t> buffer((istreambuf_iterator<char>(in)), {});
        size_t key_len = strlen(key);
        
        for (size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] ^= key[i % key_len];
        }
        
        out.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }
}
