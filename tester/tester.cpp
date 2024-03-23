#include <iostream>
#include <functional>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <exception>
#include <studentlib.h>

void __terminate_gracefully(const std::string &msg) noexcept {
	std::cout << -1 << std::endl;
	std::cerr << msg << std::endl;
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
	#if defined(__cpp_lib_filesystem)
            // Header available
        #else
            __terminate_gracefully("<filesystem> header is not supported by this compiler");
        #endif
	try{
		// Parse arguments
		if(argc < 2) __terminate_gracefully("Usage: ./grader.out <num-dict-words> <optional:seed>");	
		std::random_device rd;
		std::mt19937 rng(argc > 2 ? std::atoi(argv[2]) : rd());
		std::int32_t dict_size = std::atoi(argv[1]);
		// Util func
		std::function<std::string(void)> generateRandomWord = [&](){
			static std::uniform_int_distribution<int> distribution(4, 30);
			static std::unordered_set<std::string> used;
			std::int32_t length = distribution(rng);
			std::string word; word.reserve(length);
			for(std::int32_t i=0; i < length; i++) word += 'a' + (rng() % 26);
			const auto &[it, success] = used.emplace(word);
			if(!success) return generateRandomWord();
			return word;
		};
		// Create dictionary
		std::string dictionary_path = std::filesystem::temp_directory_path() / "dictionary.txt";
		std::ofstream dictionary_fs(dictionary_path);
		std::vector<std::string> words;
		for(std::int32_t i=0; i < dict_size; i++) {
			words.emplace_back(generateRandomWord());
			dictionary_fs << words.back() << '\n';
		}
		dictionary_fs.close();
		// Create text & solution_file
		std::string text_path = std::filesystem::temp_directory_path() / "text.txt";
		std::string sol_path = std::filesystem::temp_directory_path() / "sol.txt";
		std::ofstream text_fs(text_path), sol_fs(sol_path);
		std::int32_t text_size = dict_size * 100;
		for(std::int32_t i=0; i < text_size; i++){
			if(rng() % 4 == 0) { // 25% chance not in dictionary
				text_fs << generateRandomWord() << '\n';
				sol_fs << -1 << '\n';
			}
			else {
				const std::size_t id = rng() % words.size();
				text_fs << words[id] << '\n';
				sol_fs << id << '\n';
			}
		}
		text_fs.close(); sol_fs.close();
		std::vector<std::string>().swap(words);
		// Time your solution's execution time
		auto start = std::chrono::high_resolution_clock::now();
		const std::string student_sol_path = akcube::compute(dictionary_path, text_path);
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		// Verify solution
		std::string diff_cmd = "diff -q ";
		diff_cmd += sol_path + " " + student_sol_path;
		const auto result = system(diff_cmd.c_str());
		std::filesystem::remove(dictionary_path), std::filesystem::remove(text_path);
		std::filesystem::remove(sol_path), std::filesystem::remove(student_sol_path);
		if(result != 0) __terminate_gracefully("Expected solution file and student solution files differ.");
		std::cout << duration.count() << std::endl;
	
	} catch(const std::exception &e){
		__terminate_gracefully(e.what());
	}
	return EXIT_SUCCESS;
}
