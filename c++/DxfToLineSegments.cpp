/*
* PTHaptics
* August 20, 2016
* Developer: Samuel Soares
*/

//Include all necessary headers
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct VPoint {
    double x; 
    double y;
}; 

void getPoints(string filename); 
string trim(string str); 

int main(int argc, char *argv[])
{
    getPoints("../dxf/lowercase_alphabet.dxf"); 
    return 0; 
}

void getPoints (string filename){
    vector<VPoint> points; 
    
    VPoint point; 
    ifstream input(filename);
    for (string line; getline(input, line);){
        VPoint point; 
        
        string zero = "0";
        string tZero = trim(zero); 
        
        string tLine = trim(line); 
        int res = tLine.compare(zero); 
       std::cout << tLine <<  " = 0 ? " << res << endl; 
       //std::cout << tLine << endl; 

        if (line.compare("0") == 0){
            cout << "here\n"; 
            getline(input, line);
            //tLine = trim(line); 
            if (line.compare("10") == 0){ // start x
                getline(input, line); 
                //tLine = trim(line); 
                point.x = stod(line); 
                
                getline(input, line); 
                //tLine = trim(line); 
                if (line.compare("20") == 0){
                    getline(input, line); 
                    point.y = stod(trim(line)); 
                    
                    points.push_back(point); 
                    std::cout << "x=" << point.x << ", y=" << point.y << "\n"; 
                }
            }
        }
    }
}

string trim(string str){
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}
