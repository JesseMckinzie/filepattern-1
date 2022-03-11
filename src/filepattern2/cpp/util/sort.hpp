/**
 * @file sort.hpp
 * @author Jesse McKinzie (Jesse.McKinzie@axleinfo.com)
 * @brief External memory merge sort
 * @version 0.1
 * @date 2021-12-23
 * 
 * 
 */

#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <filesystem>
#include <map>
//#include "file_stream.hpp"
#include "block.hpp"
#include "util.hpp"

namespace fs = std::filesystem;

enum Structure {std_map, std_vector};

typedef std::vector<std::string> vector_string;
class ExternalMergeSort {

    public:

        /**
         * @brief Construct a new External Merge Sort object to sort a .txt file 
         * using at most blockSize of main memory.
         * 
         * Sorts a .txt file by line if Structure=std_vector and by supplied variable
         * if Structure=std_map is used. The file is first sorted into n chunks where 
         * n=total_file_size/block_size. The n sorted blocks are then merged using an 
         * iterative 2-way merge. Once the n files are merged into one file, the file
         * copied to a file with the supplied output name.
         *
         * @param structure Data structure to sort (std_vector or std_map)
         * @param inputFileName Path to input file 
         * @param outputFileName Path to output file (will be created if doesn't exist)
         * @param blockSize Maximum amount of memory, format is "<size> <unit>" where 
         * unit is either B, KB, MB, or GB. Format 
         * @param variable Variable to sort by if std_map is passed
         * @param mapSize Size of map if std_map is used
         */
        ExternalMergeSort(const Structure& structure, 
                          const std::string& inputFileName, 
                          const std::string& outputFileName, 
                          const std::string& blockSize, 
                          const std::string& variable,
                          const int mapSize);

    private:
        vector_string vec; // Vector to store blocks of files
        std::string inputFileName; // Store the input path 
        std::string outputFileName; // Store the output path
        vector_string filesToMerge; // Vector to keep track of files to merge
        vector_string newFiles; // Vector to store files that were merged
        long double blockSize; // Max size of memory to use at once
        std::string tmpdir; // Create a directory to store temporary files
        std::string sortVariable; // Variable to sort by if std_map
        std::string blockSizeStr; // String input of blocksize
        int mapSize; // Number of variables in the map
        Map tempMap; 

        /**
         * @brief Calculate the current size of a vector that contains strings
         * 
         * @param stringSize Size of the string to be added to the vector 
         * @param previousSize Size of vector before adding another string
         * @return double The number of bytes the vector with the new string added takes up
         */
        long currentSize(const int& stringSize, const long& previousSize);

        /**
         * @brief Write a string to an output file in the temporary directory.
         * 
         * @param output String to write to output 
         */
        void writeTmpFile(std::string& output);
        
        /**
         * @brief Sort a file of strings.
         * 
         */
        void sortFile();

        /**
         * @brief Sort a file of maps.
         * 
         */
        void sortMapFile();

        /**
         * @brief Two-way iterative merge of maps containing maps
         * 
         */
        void mergeMaps();

        /**
         * @brief Write a string to an output file.
         * 
         */
        void writeFile(const std::string&);

        /**
         * @brief Merge two files that contain strings.
         * 
         */
        void merge();

        /**
         * @brief Write a vector of maps to an output file in the temporary directory.
         * 
         * @param file ofstream to write map to
         * @param vec Vector of maps to write output to
         */
        void writeMapTmpFile(std::ofstream& file, 
                             const std::vector<Tuple>& vec);

        /**
         * @brief Get a block of maps from an input file that takes up less than or equal 
         blockSize bytes of memory. Returns true if not end of file and false otherwise.
         * 
         * @return true If not end of file.
         * @return false If end of file.
         */
        bool getMapBlock(std::ifstream&, std::vector<Tuple>&);
        
        /**
         * @brief Merge two files of maps together.
         * 
         * @param fileName1 First file to merge
         * @param fileName2 Second file to merge
         * @param outFile Output file with merged files
         */
        void twoWayMergeMaps(const std::string& fileName1, const std::string& fileName2, const std::string& outFile);

};
