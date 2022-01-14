#include "InternalVectorPattern.hpp"

using namespace std;

InternalVectorPattern::InternalVectorPattern(const string& path, const string& pattern){
    this->path = path; // store path to target directory

    this->infile.open(path);
    if(!infile.is_open) throw invalid_argument("Invalid path \"" + path + "\".");

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex version of pattern

    VectorPattern::matchFiles();
}