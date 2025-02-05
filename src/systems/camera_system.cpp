#include "camera_system.h"

CameraSystem::CameraSystem(GLFWwindow* window) {
    this->window = window;
    // glUseProgram(shader);
    // viewLocation = glGetUniformLocation(shader, "view");
}

glm::mat4 CameraSystem::get_updated_view(
    std::unordered_map<CamType, TransformComponent> &transformComponents,
    const CamType& cam_type, CameraComponent& cameraComponent, float dt) {

    glm::vec3& pos = transformComponents[cam_type].position;
    glm::vec3& eulers = transformComponents[cam_type].eulers;
    // glm::vec3 pos = {0.0f, 0.0f, 0.0f};
    // glm::vec3 eulers = {0.0f, 0.0f, 0.0f};
    float theta = glm::radians(eulers.z);
    float phi = glm::radians(eulers.y);

    glm::vec3& right = cameraComponent.right;
    glm::vec3& up = cameraComponent.up;
    glm::vec3& forwards = cameraComponent.forwards;

    forwards = {
        glm::cos(theta) * glm::cos(phi),
        glm::sin(theta) * glm::cos(phi),
        glm::sin(phi)
    };
    right = glm::normalize(glm::cross(forwards, global_up));
    up = glm::normalize(glm::cross(right, forwards));
    // pos.z += 5.0f;
    glm::mat4 view = glm::lookAt(pos, pos + forwards, up);


    return view;
}