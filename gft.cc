// 
// gft: A Go language tab formatting tool.
//      Format a file or all .go files in a directory.
// 
#include <iostream>
#include <cstring>
#include <vector>
#include <filesystem>
#include <fstream>

#define VERSION "gft: MADE 0.0.2 AT (Apr 28 2021 19:15:26)"

using namespace std::filesystem; /* File system in C++ 2017 */
std::string global_path;         /* Path of individual file processing */
int spaces = 4;                  /* The default format is 4 spaces */

// Throw an error
inline void error(std::string message) {
    std::cout << "gft: " << message << std::endl;
    exit(-1);
}

// Load a path
// If it is a directory, return the relative path of all .go files in the dir
// Is not a directory, then the `global_path` variable is set
std::vector<std::string> *load_file(std::string p) {
    path f(p);
    if (!exists(f)) {
        error("file does not exist");
    }
    directory_entry entry(f);
    if (entry.status().type() != file_type::directory) {    /* Not dir */
        global_path = f.relative_path();
        return nullptr;
    }
    directory_iterator iter(f);
    std::vector<std::string> *files = new(std::vector<std::string>);
    /* Iterate through all files in the directory */
    for (auto &i : iter) {
        std::string lp = i.path().relative_path();
        /* Recursive processing */
        if (i.is_directory()) {
            for (auto k : *load_file(lp)) {
                files->push_back(k);
            }
        }
        /* Process files with the .go suffix */
        if (i.path().extension().string() == ".go") {
            files->push_back(lp);
        }
    }
    return files;
}

// Return the count of target in vector
inline int count(std::vector<int> vec, int tar) {
    int count = 0;
    for (auto i : vec) {
        if (i == tar) {
            count++;
        }
    }
    return count;
}

// Written to the file
void write_file(
    std::string p,              /* File path */
    std::vector<char> chars,    /* The char vector of the file */
    std::vector<int> positions  /* The location of the TAB symbol in the file */
) {
    /* Clean up the contents after opening */
    std::ofstream ofs (p, std::ios::in | std::ios::trunc);
    if (ofs.fail()) {
        error("failed to open will be read or written of file");
    }
    for (int i = 0; i < chars.size() - 1; i++) {
        if (count(positions, i)) {
            /* Spaces */
            for (int k = 0; k < spaces; k++) {
                ofs << " ";
            }
        } else {
            /* Normal symbol */
            ofs << chars.at(i);
        }
    }
    static int count = 1;
    std::cout << count++ << " OK: " << p << std::endl; /* END */
}

// Read the file
void read_file(std::string p) {
    std::ifstream ifs (p, std::ifstream::in);
    if (ifs.fail()) {
        error("failed to open file: " + p);
    }
    std::vector<int> positions;
    std::vector<char> chars;
    for (int i = 0; ifs.good(); i++) {
        char c = ifs.get();
        if (c == '\t') {
            positions.push_back(i); /* TAB */
        }
        chars.push_back(c); /* Other symbol */
    }
    ifs.close();
    /* No TAB symbol appears */
    if (positions.empty()) {
        std::cout << "file: " << p << 
            " No tab notation is included" << std::endl;
    } else {
        write_file(p, chars, positions);    /* To written */
    }
}

// Entrance
// 
// gft <FILE | DIR> | <Spaces>
// gft v
// 
int main(int args, const char **arg) {
    if (args < 2) {
        error("please appoint a .go file or dir");
    }
    if (strcmp(arg[1], "v") == 0) {
        std::cout << VERSION << std::endl;
        return 0;
    }
    if (args > 2 && atoi(arg[2]) != 0) {
        spaces = atoi(arg[2]);  /* Custom spaces */
    }
    std::vector<std::string> *files = load_file(arg[1]);    /* Files in dir */
    if (files != nullptr) {
        // Dir
        for (auto i : *files) {
            read_file(i);
        }
        delete files;
    } else {
        // File
        read_file(global_path);
    }
    return 0;
}
