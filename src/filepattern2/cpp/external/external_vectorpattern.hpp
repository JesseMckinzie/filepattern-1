#pragma once
#include "external_pattern.hpp"
#include "../util/vector_parser.hpp"
#include "../util/block.hpp"
#include "../util/util.hpp"


class ExternalVectorPattern: public ExternalPattern {

    private:
        std::regex STITCH_REGEX; // regex to match stitching vector line
        std::vector<std::string> STITCH_VARIABLES; // variable names for stitching vector 
        std::ifstream infile; // stream for validFiles.txt
        std::string path; // path to stitching vector
        std::ifstream vectorReader; //// stream to read stitching vector

    public:
        /**
         * @brief Constructor of ExternalVectorPattern
         * 
         * @param path Path to stitching vector
         * @param pattern Pattern to match filenames to
         * @param blockSize Maximum amount of memory to use
         */
        ExternalVectorPattern(const std::string& path, const std::string& pattern, const std::string& blockSize);

        /**
         * @brief Destuctor of ExternalVectorPattern. Removes temporay directories and files.
         * 
         */
        ~ExternalVectorPattern();

        /**
         * @brief Match files from the stitching vector to the pattern
         * 
         */
        void matchFiles();

        /**
         * @brief Guesses the pattern that the filename follows in a stitching vector.
         * 
         * @param path Path to stitching vector
         * @param variables Vairble names. Optional
         * @param blockSize Maximum amount of memory to use
         * @return std::string Guess of the pattern
         */
        static std::string inferPattern(const std::string& path, std::string& variables, const std::string& blockSize);
};