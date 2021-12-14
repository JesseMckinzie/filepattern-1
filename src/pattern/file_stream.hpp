#pragma once
#include "stream.hpp"

class FileStream : public Stream {
    
    public:
        FileStream(const std::string& filename, const std::string&);
        std::vector<std::string> getBlock();
};
