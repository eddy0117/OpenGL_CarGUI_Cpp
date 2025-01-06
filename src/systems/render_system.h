#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/render_component.h"

class RenderSystem {
public:

    RenderSystem(unsigned int shader, GLFWwindow* window);
    
    void update(
        std::unordered_map<unsigned int,TransformComponent> &transformComponents,
        std::unordered_map<unsigned int,RenderComponent> &renderComponents);
    
    void draw_model(
        RenderComponent& renderable, 
        TransformComponent& transform);

    void draw_model_ins_mat(
        RenderComponent& renderable, 
        TransformComponent& transform);

    void draw_line_dots(
        RenderComponent& renderable, 
        std::vector<TransformComponent>& positions);

    void draw_line(
        RenderComponent& renderable, 
        std::vector<float>& x_list,
        std::vector<float>& z_list,
        std::vector<float>& y_list);
    
private:

    unsigned int modelLocation;
    GLFWwindow* window;
};