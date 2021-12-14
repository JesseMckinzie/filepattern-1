#pragma once
#include "stream.hpp"

class FilesystemStream : public Stream {

    public:
        FilesystemStream(const std::string&, bool, const std::string&);
        std::vector<std::string> getBlock();

    private:
        fs::directory_iterator directory_iterator;
        fs::recursive_directory_iterator recursive_directory_iterator;
        bool recurisve;
        fs::directory_iterator end;
        fs::recursive_directory_iterator rec_end;
};
