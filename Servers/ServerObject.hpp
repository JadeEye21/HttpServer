#include <stdio.h>
#include <string>

#include "Utilities/Utils.hpp"

namespace HDE{
    struct serverObject{
        int new_socket = -1;
        char buffer[30000];
        std::unique_ptr<utl::URL> url;
        std::string first_line;
    };

}