#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/render_component.h"

unsigned int make_module(const std::string& filepath, unsigned int module_type);

class Shader {
public:

    Shader(const std::string& filepath, const std::string& fragment_filepath);
    ~Shader();

    void begin();
    void end();
    void Uniform1i(const GLchar* var_name, const unsigned int value);
    void Uniform4x4fv(const GLchar* var_name, const glm::mat4& value);
    void Uniform3fv_arr(const GLchar* var_name, const GLfloat* value_arr, const unsigned int num);
    void set_proj_view_mat(const glm::mat4& proj_mat, const glm::mat4& view_mat);

    void draw_model(
        RenderComponent& renderable, 
        TransformComponent& transform);
private:

    unsigned int shader;
    

    unsigned int make_shader(
        const std::string& vertex_filepath, const std::string& fragment_filepath);
    unsigned int make_module(const std::string& filepath, unsigned int module_type);
};

void switch_to_other_shader(Shader* target_shader);