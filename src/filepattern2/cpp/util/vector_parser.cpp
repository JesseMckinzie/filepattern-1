#include "vector_parser.hpp"

using namespace std; 

string VectorParser::getFileName(string& stitchingVectorLine){
    string temp = stitchingVectorLine;
    s::removeSpaces(temp); 
    return s::split(s::split(temp, ";")[0], ":")[1]; // return only the filename from the line
}

void VectorParser::parseVectorLine(Tuple& tup,
                                    string& stitchingVectorLine, 
                                    const vector<string>& STITCH_VALRIABLES, 
                                    const regex& STITCH_REGEX,
                                    vector<string> variables) {

    smatch sm;
    // if the line from the stitching vector matching the stitching vector regex pattern
    if (regex_search(stitchingVectorLine, sm, STITCH_REGEX)){   
        for(int i = 0; i < STITCH_VALRIABLES.size(); ++i){
            variables.push_back(STITCH_VALRIABLES[i]); // add stitching vector variables to variables list
            // maintain datatype of vriable
            if(s::is_number(s::to_string(sm[i+1]))){
                get<0>(tup)[STITCH_VALRIABLES[i]] = stoi(sm[i+1]);
            } else {
                get<0>(tup)[STITCH_VALRIABLES[i]] = sm[i+1];
            }
        }
    } 

}