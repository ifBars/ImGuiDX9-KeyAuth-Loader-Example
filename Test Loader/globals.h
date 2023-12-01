#pragma once
#include <unordered_map>

class c_globals {
public:
	bool active = true;
	char user_name[255] = "username";
	char pass_word[255] = "password";
	char license_key[255] = "key";
};

inline c_globals globals;