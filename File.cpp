#include "File.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <iostream>

File::Text File::ReadFull(const char* file_name)
{
    std::ifstream fs(file_name);
    if (!fs.is_open()) {
        std::cerr << "Failed to load file " << file_name << '\n';
        assert(false);
        return {};
    }

    std::stringstream ss;
    ss << fs.rdbuf();
    return { ss.str() };
}

File::Text_Pair File::ReadFull(const char* file_name1, const char* file_name2)
{
    return std::make_pair(File::ReadFull(file_name1), File::ReadFull(file_name2));
}