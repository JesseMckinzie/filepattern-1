#ifndef ExternalFilePattern_H
#define ExternalFilePattern_H
#include <chrono>
#include "ExternalPattern.hpp"
#include "fs_stream.hpp"
#include "sort.hpp"

//namespace fs = std::filesystem;

class ExternalFilePattern : public ExternalPattern {
    public:
        ExternalFilePattern(const std::string& path, const std::string& filePattern, const std::string& blockSize, bool recursive);

        /**
         * Stores and prints the valid filenames from the target directory. 
         */
        void matchFiles(const bool&, const std::string&);

        void printFiles();
        std::vector<p::pair> getMatching(std::string);
        int getCounter();
        std::vector<p::pair> getValidFilesBlock();
        void groupBy(const std::string& groupBy);
        std::vector<variableFileMap> get();

    private: 
        std::string path;
        std::filesystem::directory_iterator iterator; // File iterator for given path
        std::filesystem::recursive_directory_iterator recursiveIterator;
        FilesystemStream stream;
        bool getMap(std::ifstream&, variableFileMap&);

        //std::string fileFromPath(const std::string&);
        
        bool recursive;

        std::map<std::string, std::string> matchFilesLoop(std::map<std::string, std::string>&, const std::string&, const std::regex&, std::vector<std::string>&);
        void matchFilesOneDir(bool);
        void matchFilesMultDir(bool);
        int totalFiles;
        int mapSize;
        std::ifstream infile;
        std::string validFilesPath;
};
#endif