#include "ExternalFilePattern.hpp"
#include "FilePattern.hpp"
#include "StringPattern.hpp"
#include <chrono>
//#include <cstddef>

using namespace std;

int main(){
    /*
    ExternalMergeSort sorter = ExternalMergeSort("test10000.txt", "testoutput.txt", "10 GB");
    */

    string path = "/home/ec2-user/Dev/temp/data/Small_Fluorescent_Test_Dataset/image-tiles";
    string pattern = "img_r00{r:d}_c00{c:d}.tif";
    
    //string path = "/home/ec2-user/Dev/PolusData/polus-data/polus/images/TissueNet/standard/train";
    //string pattern = "p{p:d}_y{y:d}_r{r:ddd}_c{c:d}.ome.tif";

    FilePattern fp = FilePattern(path, pattern, false); 
    fp.printValidFiles();
    //fp.printFiles();
    //fp.groupBy("y");
    
    //vector<variableFileMap> files = fp.get();
    /*
    while(files.size() != 0){
        
        for(const auto& map: files){
            for(const auto& element: map.first){
                cout << element.first << element.second << endl;
            }
            cout << map.second[0];
            cout << endl;
        }
        
        
        files = fp.get();
    }
    */

    //string path = "/home/ec2-user/Dev/PolusData/polus-data/polus/images/TissueNet/standard/train";
    //string pattern = "p{p:d}_y{y:d}_r{r:d}_c{c:d}.ome.tif";

    /*
    auto start = chrono::high_resolution_clock::now();
    FilePattern fp = FilePattern(path, pattern, false);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start);
    cout << duration.count() << endl;
    */
    //StringPattern fp = StringPattern("test.txt", pattern);
    //fp.groupBy("r");
    //fp.printValidFiles();
    //fp.printFilePaths();

    /*
    auto match = fp.getMatching("p=1");
    for(const auto& pair: match){
        for(const auto& map: pair.first){
            cout << map.first << ": " << map.second << endl;
        }
        cout << endl;
    }
    */
    
    /*
    fp.groupBy("x");
    fp.printValidFiles();
    fp.printVariables();
    fp.printFilePaths();
    */

    exit(0);
    
}