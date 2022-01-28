#pragma once
#include "ExternalPattern.hpp"
#include "vector_parser.hpp"
#include "block.hpp"
#include "util.hpp"


class ExternalVectorPattern: public ExternalPattern {

    private:
        std::regex STITCH_REGEX;
        std::vector<std::string> STITCH_VARIABLES;
        std::ifstream infile;
        std::string path;
        std::ifstream vectorReader;
        int totalFiles;
        bool endOfFile;


    public:
        
        ExternalVectorPattern(const std::string& path, const std::string& pattern, const std::string& blockSize);
        //VectorPattern(const std::string& path, const std::string& pattern);
        void matchFiles();

        static std::string inferPattern(const std::string& path, std::string& variables, const std::string& blockSize);
};