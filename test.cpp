#include <iostream>
#include <map>

using namespace std;

int main(){
    map<string, string> map1;
    map<string, string> map2;

    map1["var1"] = "1";
    map1["var2"] = "2";
    map1["var3"] = "3";

    map2["var1"] = "5";
    map2["var2"] = "6";
    map2["var3"] = "7";

    if(map1[""] < map2[""]) cout << "True" << endl;
    else if(map1[""] == map2[""]) cout << "mid" << endl;
    else cout << "False" << endl;

}