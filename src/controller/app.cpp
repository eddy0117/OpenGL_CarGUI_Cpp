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
	TransformComponent transform;
	transform.position = {5.0f, 0.0f, 0.0f};
	transform.eulers = {0.0f, 50.0f, 0.0f};
	
	std::thread socket_thread(recv_data, std::ref(frame_queue)); 

	float ang = 0.0f;
	cameraSystem->update(
		transformComponents, cameraID, *cameraComponent, 16.67f/1000.0f);

    while (!glfwWindowShouldClose(window)) {
		ang += 1.0f;
		transform.eulers.z = ang;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//TODO 可將 cameraSystem Update 移至迴圈外 (由於view不會改變)
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			break;
		}
		// 處理 eventloop 的所有 event, ESC 跳出指令才會觸發
		glfwPollEvents();

		
		if (!frame_queue.empty()) {
			std::vector<std::unordered_map<std::string, std::string>> queue_frame_objs;
			queue_frame_objs = frame_queue.front();
			frame_queue.pop();
			queue_frame_objs.push_back({
				{"x", "0.0"},
				{"y", "0.0"},
				{"cls", "ego_car"},
				{"ang", "180.0"},
			});
			cur_frame_objs = queue_frame_objs;
		}	
		

		
	
		for(auto& obj : cur_frame_objs){
			float coord_y = -std::stof(obj["x"]) * 35 ;
			float coord_x = -std::stof(obj["y"]) * 35 + 5;
			float angle = std::stof(obj["ang"]) + 180;
			std::string obj_name = obj["cls"];
			TransformComponent transform;
			transform.position = {coord_x, coord_y, 0.0f};
			transform.eulers = {0.0f, 0.0f, angle};
			renderSystem->draw_model(model_dict[obj_name], transform);
		}
	
		
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

