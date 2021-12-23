#pragma once
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
        std::vector<std::string> variables;


    public:
        std::vector<Tuple> validFiles; // Store files that match given regex
        std::vector<std::vector<Tuple>> validGroupedFiles;

        /**
         * @brief Convert to pattern to regex.
         * 
         * Creates a version of the pattern with regex to match files to. For example, 
         * if the pattern contains {variable:d}, this is changed to [0-9] in the regex pattern.
         */
        void filePatternToRegex();

        Tuple getVariableMapMultDir(const std::string& filePattern, const std::smatch& sm);

        Tuple getVariableMap(const std::string& filePath, const std::smatch& sm);

        std::string getPattern();

        void setPattern(const std::string&);

        std::string getRegexPattern();
        
        std::vector<std::string> getVariables();

        void printVariables();
};