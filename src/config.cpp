#include "config.h"
#include <cstddef>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::vector<std::string> split(std::string line, std::string delimiter) {

    std::vector<std::string> splitLine;

    size_t pos = 0;
    std::string token;
    while((pos = line.find(delimiter)) != std::string::npos) {
        token = line.substr(0,pos);
        splitLine.push_back(token);
        line.erase(0, pos + delimiter.length());
    }
    splitLine.push_back(line);

    return splitLine;
}

float cal_distance(std::pair<float, float> p1, std::pair<float, float> p2) {
  
   
    float distance = sqrt(pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2));
    return distance;
}