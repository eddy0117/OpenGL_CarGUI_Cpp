#include "render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window) {
    
    modelLocation = glGetUniformLocation(shader, "model");
    this->window = window;
}
    
void RenderSystem::update(
    std::unordered_map<unsigned int,TransformComponent> &transformComponents,
    std::unordered_map<unsigned int,RenderComponent> &renderComponents) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (auto& [entity,renderable] : renderComponents) {

        TransformComponent& transform = transformComponents[entity];
        glm::mat4 model = glm::mat4(1.0f);
	    model = glm::translate(model, transform.position);
	    model = glm::rotate(
            model, glm::radians(transform.eulers.z), 
            { 0.0f, 0.0f, 1.0f });
        glUniformMatrix4fv(
		    modelLocation, 1, GL_FALSE, 
		    glm::value_ptr(model));
        
        glBindTexture(GL_TEXTURE_2D, renderable.material);
        glBindVertexArray(renderable.VAO);
	    glDrawArrays(GL_TRIANGLES, 0, renderable.vertexCount);
    }
	glfwSwapBuffers(window);
}

void RenderSystem::draw_model(
    RenderComponent& renderable, 
    TransformComponent& transform) {

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::rotate(
        model, glm::radians(transform.eulers.z), 
        { 0.0f, 0.0f, 1.0f });
    glUniformMatrix4fv(
        modelLocation, 1, GL_FALSE, 
        glm::value_ptr(model));
    
    glBindTexture(GL_TEXTURE_2D, renderable.material);
    glBindVertexArray(renderable.VAO);

    // 向 GPU 發出 DrawCall 指令
    glDrawArrays(GL_TRIANGLES, 0, renderable.vertexCount);

}

void RenderSystem::draw_line(
    RenderComponent& renderable, 
    std::vector<float>& x_list,
    std::vector<float>& z_list,
    std::vector<float>& y_list) {
    float last_x;
    float last_z;
    float last_y;
    glBindTexture(GL_TEXTURE_2D, renderable.material);
    glBegin(GL_LINE);

    for(int i = 0; i <= x_list.size(); i++) {
        if (i == 0) {
            last_x = x_list[i];
            last_z = z_list[i];
            last_y = y_list[i];
        }
        else {
            glVertex3f(last_x, last_z, last_y);
            glVertex3f(x_list[i], z_list[i], y_list[i]);
        }
    }
    glEnd();

}