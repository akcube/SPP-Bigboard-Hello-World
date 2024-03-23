#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <exception>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

struct chash {
    using u64 = std::uint64_t;
    u64 operator()(const std::string& x) const {
        constexpr u64 fnv_prime = 11400714819323198485ull;
        u64 hash = 0;
        for (const char c : x) {
            hash ^= c;
            hash *= fnv_prime;
        }
        return hash;
    }
};

namespace akcube {
	// using hash_table = __gnu_pbds::cc_hash_table<std::string, int, chash>;
	// using hash_table = __gnu_pbds::gp_hash_table<std::string, int, chash>;
	using hash_table = std::unordered_map<std::string, int, chash>;
	const std::string compute(const std::string &dict_path, const std::string &text_path){
		std::ifstream dict_fs(dict_path), text_fs(text_path);
		const std::string sol_path = std::filesystem::temp_directory_path() / "student_sol.txt";
		std::ofstream sol_fs(sol_path);
		hash_table table;
		std::string tmp;
		for(std::int32_t i=0; dict_fs >> tmp; i++) table[tmp] = i;
		while(text_fs >> tmp){
			if(table.find(tmp) == table.end()) sol_fs << -1 << '\n';
			else sol_fs << table[tmp] << '\n';
		}
		return sol_path;
	}
};
