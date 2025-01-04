#include "app.h"

using json = nlohmann::json;
// frame_queue 要以 ref 傳入
void App::my_func(std::queue<std::vector<std::unordered_map<std::string, std::string>>> &frame_queue) {
	int i = 0;
	json j;
	std::ifstream jfile("../json/result_vec_ordered.json");
	jfile >> j;
	jfile.close();
	
	for(auto& frame: j){
		
		std::vector<std::unordered_map<std::string, std::string>> queue_frame_objs;
		for(auto& obj: frame){

			queue_frame_objs.push_back({
				{"x", std::to_string(obj["x"].get<float>() / 682)},
				{"y", std::to_string(obj["y"].get<float>() / 682)},
				{"cls", obj["class"].get<std::string>()},
				{"ang", std::to_string(obj["distance_ang"].get<float>() + 90)},
			});
		}
		frame_queue.push(queue_frame_objs);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

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
	
	std::thread t1(&App::my_func, this, std::ref(frame_queue));
	// t1.join();

	// cur_frame_objs.push_back({
	// 	{"x", "5.0"},
	// 	{"y", "0.0"},
	// 	{"cls", "car"},
	// 	{"ang", "45.0"},
	// });

	// cur_frame_objs.push_back({
	// 	{"x", "5.0"},
	// 	{"y", "3.0"},
	// 	{"cls", "pedestrian"},
	// 	{"ang", "0.0"},
	// });

	// json j;
	// j.push_back(cur_frame_objs[0]);
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

		// int i = 0;
		// for(auto& [cls, renderable] : model_dict){
		// 	TransformComponent transform;
		// 	transform.position = {5.0f, -6.0f + 2.2f * i, 0.0f};
		// 	transform.eulers = {0.0f, 0.0f, ang};
		// 	renderSystem->draw_model(renderable, transform);
		// 	i++;
		// }
		
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