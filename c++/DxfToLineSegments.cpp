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

vector<VPoint> getPoints(string filename); 
string trim(string str); 

int main(int argc, char *argv[])
{
    vector<VPoint> points = getPoints("../dxf/lowercase_c.dxf"); 
    
   // int i = 1; 
    //for (std::vector<VPoint>::iterator it = points.begin() ; i != points.end(); ++it){
      //  cout << i++ << ": x=" << *it.x << ", y=" << *it.y << endl; 
    //}
    
    return 0; 
}

vector<VPoint> getPoints (string filename){
    vector<VPoint> points; 
    int count = 0; 
    
    ifstream input(filename);
    for (string line; getline(input, line);){
        VPoint point; 
        string tLine = trim(line);

        if (tLine.compare("0") == 1){
            getline(input, line);
            tLine = trim(line); 
            if (tLine.compare("10") == 1){ 
                getline(input, line); 
                tLine = trim(line); 
                point.x = stod(line); 
                
                getline(input, line); 
                tLine = trim(line); 
                if (tLine.compare("20") == 1){
                    getline(input, line); 
                    point.y = stod(trim(line)); 
                    
                    count++;
                    points.push_back(point); 
                    cout << count << ": x=" << point.x << ", y=" << point.y << endl; 
                    
                    // get second point (endpoint of line segment)
                    getline(input, line); 
                    tLine = trim(line);
                    VPoint point2; 
                    if (tLine.compare("11") == 1){ 
                        getline(input, line); 
                        tLine = trim(line); 
                        point2.x = stod(line); 
                        
                        getline(input, line); 
                        tLine = trim(line); 
                        if (tLine.compare("21") == 1){
                            getline(input, line); 
                            point2.y = stod(trim(line)); 
                            
                            count++;
                            points.push_back(point2); 
                           cout << count << ": x=" << point2.x << ", y=" << point2.y << endl; 
                        }
                    }
                }
            }
        }
    }
    
    return points; 
}

string trim(string str){
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}
