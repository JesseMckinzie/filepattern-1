#pragma once
#include <regex>
#include <tuple>
#include <variant>
#include <algorithm>
#include <iostream>
#include <fstream>
 
typedef std::variant<int, std::string> Types;
typedef std::map<std::string, Types> Map;
typedef std::tuple<Map , std::vector<std::string>> Tuple;

/**
 * @brief String methods
 * 
 */
namespace s {

        struct AnyGet {
                std::string operator()(int value) { return std::to_string(value); }
                std::string operator()(const std::string& value) { return value; }
        };

        inline bool is_number(const std::string &s) {
                return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
        }
        
        inline std::string to_string(const Types& input) {
                return std::visit(AnyGet{}, input);
        }

        inline int size(const Types& input){
                return input.index() == 0 ? sizeof(int) : sizeof(std::string) + s::to_string(input).length();
        }       

        /**
        * @brief Get the basename of a filepath.
        * 
        * @param filePath Filepath to find the basename of.
        * @return string The basename of the filepath.
        */
        inline std::string getBaseName(const std::string& filePath){
                if(filePath.find('/') == std::string::npos) return filePath;

                int i = filePath.size()-1;
                std::string file;
                while(filePath[i] != '/'){
                        file.insert(0, 1, filePath[i]); 
                        --i;
                }     
                return file;
        }

        /**
         * @brief Split a string on a character
         *
         * Splits a string into a vector of strings along a character. 
         * From https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
         *
         * @param s String to be split
         * @param delimiter Character to split string on
         * @return std::vector<std::string> Vector of split strings
         */
        inline std::vector<std::string> split(std::string& s, const std::string& delimiter) {
                size_t pos_start = 0, pos_end, delim_len = delimiter.length();
                std::string token;
                std::vector<std::string> res;

                while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
                        token = s.substr (pos_start, pos_end - pos_start);
                        pos_start = pos_end + delim_len;
                        res.push_back (token);
                }

                res.push_back (s.substr (pos_start));
                return res;
        }

        inline bool replace(std::string& str, const std::string& from, const std::string& to) {
                size_t start_pos = str.find(from);
                if(start_pos == std::string::npos)
                        return false;
                str.replace(start_pos, from.length(), to);
                return true;
        }

        /**
         * @brief Checks if a string ends in another string.
         * 
         * From: https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
         * 
         * @param value 
         * @param ending 
         * @return true 
         * @return false 
         */
        inline bool endsWith(std::string const & value, std::string const & ending) {

                if (ending.size() > value.size()) return false;
                return std::equal(ending.rbegin(), ending.rend(), value.rbegin());

        }

        inline void removeSpaces(std:: string& str){
                str.erase(remove(str.begin(), str.end(), ' '), str.end());
        }

        /**
         * @brief 
         * 
         * From https://stackoverflow.com/questions/6143824/add-leading-zeros-to-string-without-sprintf
         *
         * @param s 
         * @param desiredLength 
         */
        inline void padWithZeros(std::string& s, unsigned int desiredLength){
                unsigned int number_of_zeros = desiredLength - s.length(); // add 2 zeros

                s.insert(0, number_of_zeros, '0');
        }

        inline int getCount(const std::string& str, const char c){
                int count = 0;
                for(const auto& s: str){
                        if(s == c) ++count;
                }
                return count;
        }

        inline bool contains(const std::string& str, const char c){
                for(const auto& s: str){
                        if(s == c) return true;
                }
                return false;
        }
};

namespace m {

        /**
         * @brief Get a map from a .txt file 
         * 
         * Gets a map from a txt file that contains variables mapped to values.
         *
         * @param infile Input stream
         * @param mapping Map to be modified
         * @return true The end of the file has not been reached and the map is modified
         * @return false The end of the file has been reached and the mao is not modified
         */
        inline bool getMap(std::ifstream& infile, Tuple& member, int mapSize){
                std::string str;
                Map map;

                std::string key, value;
                int valueLength;
                size_t pos;

                std::get<1>(member).clear();
                while(std::getline(infile, str)){

                        // if map is correct size, return 
                        if (map.size() == (mapSize)) {
                        std::get<0>(member) = map;
                        str.pop_back();
                        std::get<1>(member).push_back(str);
                        return true;
                        } 

                        // map variable to value 
                        pos = str.find(":");
                        key = str.substr(0, pos);
                        valueLength = str.length() - pos;
                        value = str.substr(pos+1, valueLength);

                        map[key] = value;
                        //size += valueLength + pos;
                }

                return false;
        }

        inline long getSize(Tuple& mapping){
                long size = 0;
                for(const auto& mem: std::get<0>(mapping)){
                        size += sizeof(mem.first);
                        if(s::is_number(s::to_string(mem.second))) size += sizeof(int);
                        else size += sizeof(std::string) + s::to_string(mem.second).length();
                }
                size += sizeof(std::vector<std::string>);
                for(const auto& str: std::get<1>(mapping)){
                        size += str.length();
                }
                return size;
        }

        /**
         * @brief Write map to output file. 
         * 
         * @param file Output file 
         * @param mapping Map to write to output file
         */
        inline void writeMap(std::ofstream& file, Tuple& mapping){

                for(const auto& element: std::get<0>(mapping)){
                        file << element.first << ":" << s::to_string(element.second) << '\n';
                }

                for(const auto& element: std::get<1>(mapping)){
                        file << element << "," << '\n';
                } 

        }

        inline void preserveType(Tuple& mapping){
                Types value;
                for(auto& p: std::get<0>(mapping)){
                        value = p.second;
                        if(s::is_number(s::to_string(p.second))) value = std::stoi(s::to_string(p.second));
                        p.second = value;
                }
        }

        inline int getMinIdx(std::vector<Tuple>& vec, std::string& variable){
                Types min = std::get<0>(vec[0])[variable]; 

                int idx = 0;
                int minIdx = 0;
                for(auto& v : vec){
                        if(std::get<0>(v)[variable] < min){
                                min = std::get<0>(v)[variable];
                                minIdx = idx;
                        }
                        ++idx;
                }
                return minIdx;
        }

        inline int getMaxIdx(std::vector<Tuple>& vec, std::string& variable){
                Types max = std::get<0>(vec[0])[variable]; 

                int idx = 0;
                int maxIdx = 0;
                for(auto& v : vec){
                        if(std::get<0>(v)[variable] > max){
                                max = std::get<0>(v)[variable];
                                maxIdx = idx;
                        }
                        ++idx;
                }
                return maxIdx;
        }
}
