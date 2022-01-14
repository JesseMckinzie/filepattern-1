#pragma once
#include "InternalPattern.hpp"
#include "vector_parser.hpp"
#include "util.hpp"


class VectorPattern: public InternalPattern {

    private:
        std::regex STITCH_REGEX;
        std::vector<std::string> STITCH_VARIABLES;
        std::ifstream infile;
        std::string path;

    public:
        
        VectorPattern(const std::string& path, const std::string& pattern);
        //VectorPattern(const std::string& path, const std::string& pattern);
        void matchFiles();
};