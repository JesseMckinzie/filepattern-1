#include "vector_parser.hpp"

using namespace std; 

string VectorParser::getFileName(string& stitchingVectorLine){
    string temp = stitchingVectorLine;
    s::removeSpaces(temp);
    return s::split(s::split(temp, ";")[0], ":")[1];
}

void VectorParser::parseVectorLine(Tuple& tup,
                                    string& stitchingVectorLine, 
                                    const vector<string>& STITCH_VALRIABLES, 
                                    const regex& STITCH_REGEX,
                                    vector<string> variables) {

    smatch sm;
    if (regex_search(stitchingVectorLine, sm, STITCH_REGEX)){   
        for(int i = 0; i < STITCH_VALRIABLES.size(); ++i){
            variables.push_back(STITCH_VALRIABLES[i]);
            if(s::is_number(s::to_string(sm[i+1]))){
                get<0>(tup)[STITCH_VALRIABLES[i]] = stoi(sm[i+1]);
            } else {
                get<0>(tup)[STITCH_VALRIABLES[i]] = sm[i+1];
            }
        }
    } 

}