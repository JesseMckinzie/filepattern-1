#pragma once
#include <regex>
#include <tuple>
#include <variant>
#include <algorithm>

typedef std::variant<int, std::string> Types;
typedef std::map<std::string, Types> Map;
typedef std::tuple<Map , std::vector<std::string>> Tuple;

struct AnyGet {
    std::string operator()(int value) { return std::to_string(value); }
    std::string operator()(const std::string& value) { return value; }
};

namespace s {
        inline bool is_number(const std::string &s) {
                return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
        }
        
        inline std::string to_string(const Types& input) {
                return std::visit(AnyGet{}, input);
        }

        inline int size(const Types& input){
                return input.index() == 0 ? sizeof(int) : sizeof(std::string) + s::to_string(input).length();
        }       
};

struct Variables {
        //std::vector<std::pair<std::string, std::pair<std::string, int>>> variables;
        std::vector<std::tuple<std::string, std::string, int, std::regex>> variables;

        void addNode(const std::string& variable, const std::string regex, const int& dist) {
                //variables.push_back(std::make_pair(variable, std::make_pair(regex, dist)));
                variables.push_back(std::make_tuple(variable, regex, dist, std::regex(regex)));
        }

        int getNumberOfVariables(){return variables.size();}

        std::string getVariable(const int& index){
                if(index >= variables.size()){
                        std::cout << "Out of bounds error in Variables::getVariables()" << std::endl;
                }
                return std::get<0>(variables[index]);
        }
        std::string getStringRegex(const int& index){
                if(index >= variables.size()){
                        std::cout << "Out of bounds error in Variables::getRegex()" << std::endl;
                }
                return std::get<1>(variables[index]);
        }
        int getDistance(const int& index){
                if(index >= variables.size()){
                        std::cout << "Out of bounds error in Variables::getDistance()" << std::endl;
                }
                return std::get<2>(variables[index]);
        }
        int length(const int& index){
                if(index >= variables.size()){
                        std::cout << "Out of bounds error in Variables::length()" << std::endl;
                }
                std::string reg = std::get<1>(variables[index]);
                return std::count(reg.begin(), reg.end(), '[');
        }

        std::regex getRegex(const int& index){
                if(index >= variables.size()){
                        std::cout << "Out of bounds error in Variables::length()" << std::endl;
                }
                return std::get<3>(variables[index]);
        }
        /*
        std::vector<std::regex> parseRegex(const int& index){
                std::vector<std::regex> parsedRegex;
                std::string thisRegex = getRegex(index);

                std::regex expression = "";
                for(const auto& c : thisRegex){
                        if(c == '[' || isdigit(c) || isalpha(c) || c == '-'){
                                expression += c;
                        } else if (c == ']') {
                                expression += c;
                                parsedRegex.push_back(expression);
                                expression = "";
                        } else {
                                throw std::invalid_argument("Invlaid regular expression in file pattern");
                        }
                }
                return parsedRegex;
        }
        */

        int getLength(){
                return variables.size();
        }
};