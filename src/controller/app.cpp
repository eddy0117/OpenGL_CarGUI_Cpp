#include "app.h"



App::App() {
	set_up_glfw();
}

App::~App() {
    glDeleteProgram(shader);

    delete cameraSystem;
    // std::terminate();
    glfwTerminate();
}

void App::run() {
	
	//std::thread socket_thread(recv_data, std::ref(queue_json)); 
	std::thread socket_thread(&App::recv_data, this);  

	set_camera();
	
    while (!glfwWindowShouldClose(window)) {
	
		update_camera();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			break;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			// 切換觸發 normal Mode & BEV Mode
			if (!KeyPressed) {
				KeyPressed = true;
				if (mode == "normal") {
					mode = "BEV";
				}
				else {
					mode = "normal";			
				}
				KeyPressed = true;
			}
		}
		else {
			KeyPressed = false;		
		}
		// 處理 eventloop 的所有 event, ESC 跳出指令才會觸發
		
		glfwWaitEvents();

        {
            // 等待有新數據或程式結束
            std::unique_lock<std::mutex> lock(g_mtx);
            
		
            if (!queue_json.empty()) {
                cur_frame_data = queue_json.front();
                queue_json.pop();
            } else if (g_done.load()) {
                std::cout << "[Consumer] No more data. Exiting..." << std::endl;
                break;
            }
        }


        // ============================

		show_ego_car();

		draw_objs();
		draw_lines();
		draw_occ_dots();

		glfwSwapBuffers(window);
	}
}



void App::set_up_glfw() {

    glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	
	window = glfwCreateWindow(640, 480, "OpenGL_CarGUI", NULL, NULL);
	glfwMakeContextCurrent(window);
	
	// 用 GLad 載入當前版本 OpenGL 的所有 functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Couldn't load GLad" << std::endl;
		glfwTerminate();
	}

}

void App::set_up_opengl() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// 設定背景顏色 RGBA
	//Set the rendering region to the actual screen size
	int w,h;
	glfwGetFramebufferSize(window, &w, &h);
	//(left, top, width, height)
	glViewport(0,0,w,h);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	/*
		註冊 shader
		base_shader : 繪製道路物件 & 正常模式的 ego car
		ego_car_shader : 繪製 BEV mode 的 ego car
	*/

	Shader* base_shader = new Shader(
		"src/shaders/vertex_base.vert", 
		"src/shaders/fragment_base.frag");
	Shader* ego_car_shader = new Shader(
		"src/shaders/vertex_ego.vert", 
		"src/shaders/fragment_ego_uni.frag");
	
	// 設定接下來繪製時要用的 shader 程式	
	shader_dict.insert_or_assign("base", base_shader);
	shader_dict.insert_or_assign("ego", ego_car_shader);
	
	shader_dict["base"]->begin();
	// unsigned int projLocation = glGetUniformLocation(shader, "projection");
	projection = glm::perspective(
		45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

	// set ego car position
	ego_car_pos.position = {5.0f, 0.0f, 0.0f};
	ego_car_pos.eulers = {0.0f, 0.0f, 0.0f};


}

void App::set_camera() {
	// 設定 view arr
	view_dict.insert_or_assign("normal", cameraSystem->get_updated_view(
		transformComponents, CamType::NORMAL, *cameraComponent, 16.67f/1000.0f));
	view_dict.insert_or_assign("BEV", cameraSystem->get_updated_view(
		transformComponents, CamType::BEV, *cameraComponent, 16.67f/1000.0f));
	
}

void App::update_camera() {
	view = view_dict[mode];
	shader_dict["base"]->set_proj_view_mat(projection, view);
}

void App::show_ego_car() {
	if (mode == "normal") {
		draw_ego_car();
	}
	else {
		draw_ego_car_BEV();
	}
}

void App::make_systems() {
    cameraSystem = new CameraSystem(window);
    renderSystem = new RenderSystem(shader, window);
}

void App::clear_last_frame_data() {
	cur_frame_objs.clear();
	
}



