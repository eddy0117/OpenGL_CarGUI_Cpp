#include "shader.h"

void switch_to_shader(Shader* target_shader) {
	glUseProgram(0);
	target_shader->begin();
}


Shader::Shader(const std::string& filepath, const std::string& fragment_filepath) {
	// 建立一個可執行的對象容器, 並分別把 vertex shader & fragment shader 放進去
	shader = glCreateProgram();
	shader = make_shader(filepath, fragment_filepath);
}

Shader::~Shader() {
	glDeleteProgram(shader);
};

void Shader::begin() {
	glUseProgram(shader);
}

void Shader::end() {
	glUseProgram(0);
}

void Shader::draw_model(
    RenderComponent& renderable, 
    TransformComponent& transform) {

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::rotate(
        model, glm::radians(transform.eulers.z), 
        { 0.0f, 0.0f, 1.0f });
    unsigned int modelLocation = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(
        modelLocation, 1, GL_FALSE, 
        glm::value_ptr(model));
    
    glBindTexture(GL_TEXTURE_2D, renderable.material);
    glBindVertexArray(renderable.VAO);

    // 向 GPU 發出 DrawCall 指令
    glDrawArrays(GL_TRIANGLES, 0, renderable.vertexCount);

}

void Shader::Uniform1i(const GLchar* var_name, const unsigned int value) {
	unsigned int var_loc = glGetUniformLocation(shader, var_name);
	glUniform1i(var_loc, value);	
}

void Shader::Uniform4x4fv(const GLchar* var_name, const glm::mat4& value) {
	unsigned int var_loc = glGetUniformLocation(shader, var_name);
	glUniformMatrix4fv(var_loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::Uniform3fv_arr(const GLchar* var_name, const GLfloat* value_arr, const unsigned int num) {
	unsigned int var_loc = glGetUniformLocation(shader, var_name);
	glUniform3fv(var_loc, num, value_arr);
}

void Shader::set_proj_view_mat(const glm::mat4& proj_mat, const glm::mat4& view_mat) {
	Uniform4x4fv("projection", proj_mat);
	Uniform4x4fv("view", view_mat);
}

unsigned int Shader::make_shader(
    const std::string& vertex_filepath, const std::string& fragment_filepath) {

	//To store all the shader modules
	std::vector<unsigned int> modules;

	// (Compile)
	// 分別編譯 vertex shader 和 fragment shader 為中間檔案

	modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));
	modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));


	for (unsigned int shaderModule : modules) {
		glAttachShader(shader, shaderModule);
	}
	// (Link)
	// 將放進容器的 shader 檔案進行串接, 成為完整的可執行程式
	glLinkProgram(shader);

	//Check the linking worked
	int success;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) {
		char errorLog[1024];
		glGetProgramInfoLog(shader, 1024, NULL, errorLog);
		std::cout << "Shader linking error:\n" << errorLog << '\n';
	}

	// 容器已經建立好並串接程可執行檔, 原本編譯好的 shader 中間檔案可以刪除
	for (unsigned int shaderModule : modules) {
		glDeleteShader(shaderModule);
	}

	return shader;

}

unsigned int Shader::make_module(const std::string& filepath, unsigned int module_type) {
	
	std::ifstream file;
	std::stringstream bufferedLines;
	std::string line;

	file.open(filepath);
	while (std::getline(file, line)) {
		//std::cout << line << std::endl;
		bufferedLines << line << '\n';
	}
	std::string shaderSource = bufferedLines.str();
	const char* shaderSrc = shaderSource.c_str();
	bufferedLines.str("");
	file.close();

	unsigned int shaderModule = glCreateShader(module_type);
	glShaderSource(shaderModule, 1, &shaderSrc, NULL);
	glCompileShader(shaderModule);

	int success;
	glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
	if (!success) {
		char errorLog[1024];
		glGetShaderInfoLog(shaderModule, 1024, NULL, errorLog);
		std::cout << "Shader Module compilation error:\n" << errorLog << std::endl;
	}

	return shaderModule;
}