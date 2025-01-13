#pragma once
#include "../config.h"

unsigned int make_module(const std::string& filepath, unsigned int module_type);


class Shader {
public:

    Shader(const std::string& filepath, const std::string& fragment_filepath);
    ~Shader();

    void begin();
    void end();
    void Uniform4x4fv(const GLchar* var_name, const glm::mat4& value);

private:

    unsigned int shader;

    unsigned int make_shader(
        const std::string& vertex_filepath, const std::string& fragment_filepath);
    unsigned int make_module(const std::string& filepath, unsigned int module_type);
};