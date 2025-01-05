#include "app.h"

App::App() {
	set_up_glfw();
}

App::~App() {
    glDeleteProgram(shader);

    delete cameraSystem;
    delete renderSystem;
    // std::terminate();
    glfwTerminate();
}

void App::run() {
	
	std::thread socket_thread(recv_data, std::ref(queue_j)); 

	cameraSystem->update(
		transformComponents, cameraID, *cameraComponent, 16.67f/1000.0f);

    while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			break;
		}
		// 處理 eventloop 的所有 event, ESC 跳出指令才會觸發
		glfwPollEvents();

		if (!queue_j.empty()) {
			clear_last_frame_data();

			cur_frame_data = queue_j.front();
			queue_j.pop();
			// std::cout << cur_frame_data["obj"] << std::endl;
		}
		

		draw_ego_car();

		draw_screen();
		
		glfwSwapBuffers(window);
	}
}

void App::draw_screen() {
	// 繪製道路物件
	for(auto& obj: cur_frame_data["obj"]) {
		float coord_x = -obj["y"].get<float>() * 35 + 5;
		float coord_y = -obj["x"].get<float>() * 35 ;
		float angle = obj["ang"].get<float>() + 180;
		std::string obj_name = obj["cls"].get<std::string>();
		TransformComponent transform;
		transform.position = {coord_x, coord_y, 0.0f};
		transform.eulers = {0.0f, 0.0f, angle};
		renderSystem->draw_model(model_dict[obj_name], transform);
	}

	// 繪製道路地圖線
	for(auto& dot: cur_frame_data["dot"]) {
		std::vector<float> x_list = dot["x"].get<std::vector<float>>();
		std::vector<float> y_list = dot["y"].get<std::vector<float>>();
		std::vector<float> z_list(x_list.size(), 0);
		std::string dot_class = std::to_string(dot["cls"].get<int>());

		for (int i; i <= x_list.size(); i++) {
			x_list[i] = x_list[i] * 70 - 35;
			y_list[i] = y_list[i] * 70 - 35;
			z_list[i] -= 0.5;
		}
		renderSystem->draw_line(model_dict[dot_class], x_list, y_list, z_list);
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

    // 設定接下來繪製時要用的 shader 程式
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

void App::clear_last_frame_data() {
	cur_frame_objs.clear();
}

void App::draw_ego_car() {
	TransformComponent transform;
	transform.position = {5.0f, 0.0f, 0.0f};
	transform.eulers = {0.0f, 0.0f, 0.0f};
	renderSystem->draw_model(model_dict["ego_car"], transform);
}

