#pragma once
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
        std::vector<std::pair<std::string, std::pair<std::string, int>>> variables;

        void addNode(const std::string& variable, const std::string& regex, const int& dist) {
                variables.push_back(std::make_pair(variable, std::make_pair(regex, dist)));
        }

        int getNumberOfVariables(){return variables.size();}

        std::string getVariable(const int& index){
                if(index >= variables.size()){
                        std::cout << "Out of bounds error in Variables::getVariables()" << std::endl;
                }
                return variables[index].first;
        }
        std::string getRegex(const int& index){
                if(index >= variables.size()){
                        std::cout << "Out of bounds error in Variables::getRegex()" << std::endl;
                }
                return variables[index].second.first;
        }
        int getDistance(const int& index){
                if(index >= variables.size()){
                        std::cout << "Out of bounds error in Variables::getDistance()" << std::endl;
                }
                return variables[index].second.second;
        }

        std::vector<std::string> parseRegex(const int& index){
                std::vector<std::string> parsedRegex;
                std::string thisRegex = getRegex(index);

                std::string expression = "";
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

        int getLength(){
                return variables.size();
        }
};