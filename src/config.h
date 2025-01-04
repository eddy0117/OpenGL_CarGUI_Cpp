#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <vector>
#include <queue>
#include <unordered_map>

#include <fstream>
#include <sstream>
#include <string>

#include <thread>
// #include <unistd.h>
#include <chrono>
#include <nlohmann/json.hpp>
#include <sys/socket.h>

std::vector<std::string> split(std::string line, std::string delimiter);