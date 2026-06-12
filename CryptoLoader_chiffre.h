#ifndef CRYPTOLOADER_CHIFFRE_H
#define CRYPTOLOADER_CHIFFRE_H

#include "ICryptoAlgorithm_chiffre.h"
#include <string>

bool load_plugin(const std::string& libraryPath);
void unload_plugin();
ICryptoAlgorithm* get_algorithm();
bool is_plugin_loaded();

#endif