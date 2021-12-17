#ifndef Pattern_H
#define Pattern_H
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>
#include <map>
#include "Variables.hpp"

class Pattern {
    protected:
        std::regex regexExpression; // Regex expression
        std::string filePattern;
        std::string regexFilePattern;
        Variables variables;
        bool filesSorted;
    public:
        std::vector<Tuple> validFiles; // Store files that match given regex
        std::vector<std::vector<Tuple>> validGroupedFiles;

        //std::vector<Tuple> getVariables();
    
        bool invalidFilePath(const std::string&);

        void filePatternToRegex();

        void groupBy(const std::string&);

        std::vector<std::string> split (std::string&, const std::string&);

        std::vector<Tuple> getMatching(std::string&);
        
        void printValidFiles();

        std::vector<Tuple> getValidFiles();

        std::string getPattern();

        void setPattern(const std::string&);

        std::string getRegexPattern();
        
        std::vector<std::string> getVariables();

        void printFilePaths();

        void printVariables();

        std::vector<std::vector<std::string>> getValidFilePaths();
        Map matchFilesLoop(Map&, 
                           const std::string&,
                           const std::regex&, 
                           std::vector<std::string>&);

};

#endif