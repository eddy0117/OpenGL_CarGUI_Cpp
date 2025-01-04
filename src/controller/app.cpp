#include "app.h"

using json = nlohmann::json;
void App::my_func() {
	int i = 0;
	while (1) {
		// std::cout << "Hello this is 老師請問今天有要開會嗎from another thread" << std::endl;
		cur_frame_objs.push_back({
			{"x", "0.0"},
			{"y", std::to_string(-5.0f + 1.0f * i)},
			{"cls", "pedestrian"},
			{"ang", std::to_string(10.0f * i)},
		});
		sleep(1);
		i++;
	}
	
}

App::App() {
    set_up_glfw();
}

App::~App() {
    glDeleteProgram(shader);

    delete cameraSystem;
    delete renderSystem;
    
    glfwTerminate();
}

void App::run() {
	TransformComponent transform;
	transform.position = {5.0f, 0.0f, 0.0f};
	transform.eulers = {0.0f, 50.0f, 0.0f};

	std::thread t1(&App::my_func, this);
	// t1.join();

	cur_frame_objs.push_back({
		{"x", "5.0"},
		{"y", "0.0"},
		{"cls", "car"},
		{"ang", "45.0"},
	});

	cur_frame_objs.push_back({
		{"x", "5.0"},
		{"y", "3.0"},
		{"cls", "pedestrian"},
		{"ang", "0.0"},
	});

	json j;
	j.push_back(cur_frame_objs[0]);
	// str to float
	// std::stof("3.0")
	float ang = 0.0f;
    while (!glfwWindowShouldClose(window)) {
		ang += 1.0f;
		transform.eulers.z = ang;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bool should_close = cameraSystem->update(
            transformComponents, cameraID, *cameraComponent, 16.67f/1000.0f);
		if (should_close) {
			break;
		}

		int i = 0;
		for(auto& [cls, renderable] : model_dict){
			TransformComponent transform;
			transform.position = {5.0f, -6.0f + 2.2f * i, 0.0f};
			transform.eulers = {0.0f, 0.0f, ang};
			renderSystem->draw_model(renderable, transform);
			i++;
		}

		for(auto& obj : cur_frame_objs){
			float coord_x = std::stof(obj["x"]);
			float coord_y = std::stof(obj["y"]);
			float angle = std::stof(obj["ang"]);
			std::string obj_name = obj["cls"];
			TransformComponent transform;
			transform.position = {coord_x, coord_y, 0.0f};
			transform.eulers = {0.0f, 0.0f, angle};
			renderSystem->draw_model(model_dict[obj_name], transform);
		}

		// renderSystem->update(transformComponents, renderComponents);
		// renderSystem->draw_model(model_dict["car"], transform);
		// renderSystem->draw_model(model_dict["pedestrian"], transform2);
		glfwSwapBuffers(window);
	}
}

void App::set_up_glfw() {

    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	
	window = glfwCreateWindow(640, 480, "OpenGL_CarGUI", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Couldn't load opengl" << std::endl;
		glfwTerminate();
	}

}

void App::set_up_opengl() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//Set the rendering region to the actual screen size
	int w,h;
	glfwGetFramebufferSize(window, &w, &h);
	//(left, top, width, height)
	glViewport(0,0,w,h);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    shader = make_shader(
		"../src/shaders/vertex.txt", 
		"../src/shaders/fragment.txt");
    
    glUseProgram(shader);
	unsigned int projLocation = glGetUniformLocation(shader, "projection");
	glm::mat4 projection = glm::perspective(
		45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void App::make_systems() {
    cameraSystem = new CameraSystem(shader, window);
    renderSystem = new RenderSystem(shader, window);
}