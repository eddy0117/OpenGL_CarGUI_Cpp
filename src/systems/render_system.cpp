#include "render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window) {
    
    modelLocation = glGetUniformLocation(shader, "model");

    // 由於成員變數與參數同名，所以使用 this pointer 來指定成員變數
    this->window = window;
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

void RenderSystem::draw_model_ins_mat(
    RenderComponent& renderable, 
    TransformComponent& transform) {

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::rotate(
        model, glm::radians(transform.eulers.z), 
        { 0.0f, 0.0f, 1.0f });
    
    unsigned int ins_VBO;
    glGenBuffers(1, &ins_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, ins_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &model, GL_STATIC_DRAW);

    glBindVertexArray(renderable.VAO);
    
    for(int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
        glVertexAttribDivisor(3 + i, 1);
    }
    glBindVertexArray(renderable.VAO);
    glBindTexture(GL_TEXTURE_2D, renderable.material);

    glDrawArraysInstanced(GL_TRIANGLES, 0, renderable.vertexCount, 1);
    glDeleteBuffers(1, &ins_VBO);
    glBindVertexArray(0);
}

void RenderSystem::draw_line_dots(
    RenderComponent& renderable, 
    std::vector<TransformComponent>& positions) {
    std::vector<glm::mat4> transform_list(positions.size());

    // 將輸入的 vec3(x, y, z) 轉換成 mat4
    std::transform(
        positions.begin(), positions.end(), transform_list.begin(), 
        [&](TransformComponent t) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, t.position);
    
            return model;
        });

    // 創建新的 VBO 來關聯存 instance matrices 的空間
    unsigned int ins_VBO;
    glGenBuffers(1, &ins_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, ins_VBO);

    // 要分配的顯存大小為 sizeof(mat4) * model position 數量
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * transform_list.size(), transform_list.data(), GL_STATIC_DRAW);

    // 調用想要渲染的模型的 VAO
    glBindVertexArray(renderable.VAO);
    
    for(int i = 0; i < 4; i++) {

        // 0, 1, 2 是原本已經設定好的 vertex attribute, 分別是 positions, UV, normal
        // 由於 mat4 為 4 個 vec4, 所以這邊從 index 3 開始設定 3, 4, 5, 6 的 vertex attribute 為 instance matrix 區域範圍
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));

        // 每個實例都更新一次要使用的 vertex attribute
        glVertexAttribDivisor(3 + i, 1);
    }

    glBindTexture(GL_TEXTURE_2D, renderable.material);

    glDrawArraysInstanced(GL_TRIANGLES, 0, renderable.vertexCount, transform_list.size());
    // glDeleteBuffers(1, &ins_VBO);
    glBindVertexArray(0);

}

void RenderSystem::draw_occ_dots(
    RenderComponent& renderable, 
    unsigned int color,
    std::vector<TransformComponent>& positions) {
    std::vector<glm::mat4> transform_list(positions.size());
    std::transform(
        positions.begin(), positions.end(), transform_list.begin(), 
        [&](TransformComponent t) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, t.position);
    
            return model;
        });

    unsigned int ins_VBO;
    glGenBuffers(1, &ins_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, ins_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * transform_list.size(), transform_list.data(), GL_STATIC_DRAW);

    glBindVertexArray(renderable.VAO);
    
    for(int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }
    // glBindVertexArray(renderable.VAO);
    glBindTexture(GL_TEXTURE_2D, color);

    glDrawArraysInstanced(GL_TRIANGLES, 0, renderable.vertexCount, transform_list.size());
    // glDeleteBuffers(1, &ins_VBO);
    glBindVertexArray(0);

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