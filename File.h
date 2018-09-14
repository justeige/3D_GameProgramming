#pragma once

#include <string>
#include <optional>  // needs c++17 compiler...

/// TODO move into file specific module (if any such file emerges!)
namespace File {

using Text = std::optional<std::string>;
using Text_Pair = std::pair<Text, Text>;

Text      ReadFull(const char* file_name);
Text_Pair ReadFull(const char* file_name1, const char* file_name2);

}