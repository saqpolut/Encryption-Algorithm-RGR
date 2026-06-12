#include "plugin_loader_rsa_enigma.h"

bool load_plugin(const char* filename, Plugin& p) {
    p.handle = LOAD_LIB(filename);
    if (!p.handle) return false;
    
    p.get_name = (GetInfoFunc)GET_SYM(p.handle, "plugin_get_name");
    p.get_format = (GetInfoFunc)GET_SYM(p.handle, "plugin_get_key_format");
    p.gen_key = (GenKeyFunc)GET_SYM(p.handle, "plugin_generate_key");
    p.process = (ProcessFunc)GET_SYM(p.handle, "plugin_process_data");
    
    return p.get_name && p.get_format && p.gen_key && p.process;
}

void free_plugin(Plugin& p) {
    if (p.handle) {
        FREE_LIB(p.handle);
        p.handle = nullptr;
    }
}