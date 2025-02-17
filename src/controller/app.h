#pragma once
#include "../config.h"

#include "../components/camera_component.h"
#include "../components/render_component.h"
#include "../components/transform_component.h"

#include "../systems/camera_system.h"
#include "../systems/render_system.h"

#include "../view/shader.h"

#include "../threads/socket_thread.h"
class App {
public:
    App();
    ~App();
    void run();
    void set_up_opengl();
    void make_systems();


    //Components
    std::unordered_map<unsigned int, TransformComponent> transformComponents;
    CameraComponent* cameraComponent;
    unsigned int cameraID;
    std::unordered_map<unsigned int, RenderComponent> renderComponents;
    std::unordered_map<std::string, RenderComponent> model_dict;
    std::unordered_map<std::string, unsigned int> color_dict;

private:
    void set_up_glfw();
    void clear_last_frame_data();
    void draw_objs();
    void draw_lines();
    void draw_ego_car();
    void draw_occ_dots();

    std::vector<TransformComponent> line_interpolation(
        std::vector<TransformComponent>& positions, 
        int num_points);

    GLFWwindow* window;

    unsigned int shader;
    std::vector<std::unordered_map<std::string, std::string>> cur_frame_objs;
    std::vector<std::unordered_map<std::string, std::string>> cur_frame_dots;
  
    std::queue<nlohmann::json> queue_json;
    nlohmann::json cur_frame_data;
    //Systems
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;
};