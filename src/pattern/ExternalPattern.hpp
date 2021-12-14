#ifndef ExternalPattern_H
#define ExternalPattern_H
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>
#include <map>
#include "Variables.hpp"
//#include "sort.h"

typedef p::pair<std::map<std::string,std::string>, std::vector<std::string>> variableFileMap;

class ExternalPattern {
    
    protected:
        std::regex regexExpression; // Regex expression
        std::string filePattern;
        std::string regexFilePattern;
        Variables variables;
        bool filesSorted;
        long blockSize;

    public:
        std::vector<p::pair<std::map<std::string, std::string>, std::vector<std::string>>> validFiles; // Store files that match given regex
        std::vector<std::vector<p::pair<std::map<std::string, std::string>, std::vector<std::string>>>> validGroupedFiles;

        bool invalidFilePath(const std::string&);

        void filePatternToRegex();

        //void groupBy(const std::string&);

        std::vector<std::string> split (std::string&, const std::string&);
        
        void printValidFiles();

        std::string getPattern();

        void setPattern(const std::string&);

        std::string getRegexPattern();

};

#endif