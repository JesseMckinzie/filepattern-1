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
        std::vector<p::pair<std::map<std::string, std::string>, std::vector<std::string>>> validFiles; // Store files that match given regex
        std::vector<std::vector<p::pair<std::map<std::string, std::string>, std::vector<std::string>>>> validGroupedFiles;

        //std::vector<p::pair<std::map<std::string, std::string>, std::vector<std::string>>> getVariables();
    
        bool invalidFilePath(const std::string&);

        void filePatternToRegex();

        void groupBy(const std::string&);

        std::vector<std::string> split (std::string&, const std::string&);

        std::vector<p::pair<std::map<std::string, std::string>, std::vector<std::string>>> getMatching(std::string);
        
        void printValidFiles();

        std::vector<p::pair<std::map<std::string, std::string>, std::vector<std::string>>> getValidFiles();

        std::string getPattern();

        void setPattern(const std::string&);

        std::string getRegexPattern();
        
        std::vector<std::string> getVariables();

        void printFilePaths();

        void printVariables();

        std::vector<std::vector<std::string>> getValidFilePaths();
        std::map<std::string, std::string> matchFilesLoop(std::map<std::string, std::string>&, 
                                                                    const std::string&,
                                                                    const std::regex&, 
                                                                    std::vector<std::string>&);

};

#endif