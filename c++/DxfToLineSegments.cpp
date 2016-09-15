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
    getPoints("../dxf/lowercase_c.dxf"); 
    return 0; 
}

void getPoints (string filename){
    vector<VPoint> points; 
    
    VPoint point; 
    ifstream input(filename);
    for (string line; getline(input, line);){
        VPoint point; 
        string tLine = trim(line);

        if (tLine.compare("0") == 1){
            getline(input, line);
            tLine = trim(line); 
            if (tLine.compare("10") == 1){ // start x
                getline(input, line); 
                tLine = trim(line); 
                point.x = stod(line); 
                
                getline(input, line); 
                tLine = trim(line); 
                if (tLine.compare("20") == 1){
                    getline(input, line); 
                    point.y = stod(trim(line)); 
                    
                    points.push_back(point); 
                    cout << "x=" << point.x << ", y=" << point.y << endl; 
                }
            }
        }
    }
}

std::string trim(std::string str){
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}
