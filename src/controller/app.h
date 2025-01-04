#pragma once
#include "../config.h"

#include "../components/camera_component.h"
#include "../components/render_component.h"
#include "../components/transform_component.h"

#include "../systems/camera_system.h"
#include "../systems/render_system.h"

#include "../view/shader.h"

class App {
public:
    App();
    ~App();
    void run();
    void set_up_opengl();
    void make_systems();

    //Components
    std::unordered_map<unsigned int, TransformComponent> transformComponents;
    // std::unordered_map<unsigned int, PhysicsComponent> physicsComponents;
    CameraComponent* cameraComponent;
    unsigned int cameraID;
    std::unordered_map<unsigned int, RenderComponent> renderComponents;
    std::unordered_map<std::string, RenderComponent> model_dict;

private:
    void set_up_glfw();

    void my_func();

    GLFWwindow* window;

    unsigned int shader;
    std::vector<std::unordered_map<std::string, std::string>> cur_frame_objs;
    //Systems
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;
};