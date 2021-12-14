#pragma once 
#include "Pattern.hpp"

namespace fs = std::filesystem;

class FilePattern : public Pattern {
    public:
        FilePattern(const std::string&, const std::string&, bool);

        /**
         * Stores and prints the valid filenames from the target directory. 
         */
        void matchFiles(const bool&, const std::string&);
        void printFiles();
    private: 
        std::string path;
        fs::directory_iterator iterator; // File iterator for given path
        fs::recursive_directory_iterator recursiveIterator;
        bool recursive;

        //std::map<std::string, std::string> matchFilesLoop(std::map<std::string, std::string>&, const std::string&, const std::regex&, std::vector<std::string>&);
        void matchFilesOneDir(bool);
        void matchFilesMultDir(bool);
};