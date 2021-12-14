#pragma once
#include "Pattern.hpp"

class StringPattern : public Pattern {
    public:
        StringPattern(const std::string&, const std::string&);

        /**
         * Stores and prints the valid filenames from the target directory. 
         */
        void matchFiles();
        void readFile();

    private:
        std::vector<std::string> files;
        std::string fileName;
};