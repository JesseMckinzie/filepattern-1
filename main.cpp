#include "./src/pattern/FilePattern.h"
#include <chrono.h>

using namespace std;

int main(){

    string path = "/home/ec2-user/Dev/PolusData/polus-data/polus/images/TissueNet/standard/train";
    string pattern = "p{p:d}_y{y:d}_r{r:ddd}_c{c:d}.ome.tif";

    auto start = std::chrono::high_resolution_clock::now();
    FilePattern fp = FilePattern(path, pattern);

    fp.getMatching();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken: " << duration << endl;

}