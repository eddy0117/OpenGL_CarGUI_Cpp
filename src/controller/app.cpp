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
	
	std::thread socket_thread(recv_data, std::ref(queue_json)); 

	// 開始時設定一次相機視角就好
	cameraSystem->update(
		transformComponents, cameraID, *cameraComponent, 16.67f/1000.0f);

    while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			break;
		}
		// 處理 eventloop 的所有 event, ESC 跳出指令才會觸發
		glfwPollEvents();

		//如果佇列 queue_json 不為空 ，取出新數據更新到 cur_frame_data
		if (!queue_json.empty()) {
			clear_last_frame_data();
			cur_frame_data = queue_json.front();
			queue_json.pop();
		}
		
		
        // ============================
        //   車用儀表板渲染區域 
		draw_ego_car();
		draw_objs();
		draw_lines();
		draw_occ_dots();

		glfwSwapBuffers(window);
		// ============================

	}
}

void App::draw_objs() {
	// 繪製道路物件
	int obj_scale = 40;
	for(auto& obj: cur_frame_data["obj"]) {
		float coord_x = -obj["x"].get<float>() * obj_scale;	
		float coord_y = -obj["y"].get<float>() * obj_scale + 5;		
		float angle = obj["ang"].get<float>() + 180;
		std::string obj_name = obj["cls"].get<std::string>();
		TransformComponent transform;
		transform.position = {coord_y, coord_x , 0.0f};
		transform.eulers = {0.0f, 0.0f, angle};
		// renderSystem->draw_model(model_dict[obj_name], transform);
		renderSystem->draw_model_ins_mat(model_dict[obj_name], transform);
	}	
}

void App::draw_lines() {
// 繪製道路地圖線
	// 一次畫一條線的所有點 (調用實例矩陣畫法)
	int scale = 35;
	for(auto& dot: cur_frame_data["dot"]) {

		std::vector<float> x_list = dot["x"].get<std::vector<float>>();
		std::vector<float> y_list = dot["y"].get<std::vector<float>>();
		std::vector<float> z_list(x_list.size(), 0);
		std::string dot_class = std::to_string(dot["cls"].get<int>());

		// transform 裡最多兩個輸入範圍
		std::vector<TransformComponent> positions(x_list.size());
		std::vector<size_t> indices(x_list.size());
		// 將 indices 初始化為 (0 ~ x_list.size())
		std::iota(indices.begin(), indices.end(), 0);
		std::transform(
			indices.begin(), indices.end(), positions.begin(),
			[&](size_t idx) {
				TransformComponent t;
				t.position = {(y_list[idx] * scale - (scale / 2.0f) + 5),
							  (x_list[idx] * scale - (scale / 2.0f)), 
							  z_list[idx]
							  };
				t.eulers = {0.0f, 0.0f, 0.0f};
				return t;
			});
		positions = line_interpolation(positions, 15);
		renderSystem->draw_line_dots(model_dict[dot_class], positions);
	}
}

void App::draw_occ_dots() {
	// 繪製 3d occupancy dots
	int scale = 35;
	float dot_density = 1.88f;
	std::vector<std::string> cls_black_list = {"2", "4", "6", "7", "10", "16"};
	for(auto& dot_list: cur_frame_data["occ"].items()) {
		
		// 過濾掉黑名單類別
		if (std::find(cls_black_list.begin(), cls_black_list.end(), dot_list.key()) != cls_black_list.end()) {
			continue;
		}
		std::vector<std::vector<float>> vox_coord_list = dot_list.value();
		// std::cout << vox_coord_list[0][0] << std::endl;
		// transform 裡最多兩個輸入範圍
		std::vector<TransformComponent> positions(vox_coord_list.size());
		std::vector<size_t> indices(vox_coord_list.size());
		// 將 indices 初始化為 (0 ~ x_list.size())
		std::iota(indices.begin(), indices.end(), 0);
		std::transform(
			indices.begin(), indices.end(), positions.begin(),
			[&](size_t idx) {
				TransformComponent t;
				t.position = {(vox_coord_list[idx][1] - 100) / dot_density, 
							 -(vox_coord_list[idx][0] - 100) / dot_density,
							  (vox_coord_list[idx][2] / dot_density) - 5.0f,
							 
							  };
				t.eulers = {0.0f, 0.0f, 0.0f};
				return t;
			});
		renderSystem->draw_occ_dots(model_dict["occ_dot"], color_dict[dot_list.key()], positions);
	}
}

std::vector<TransformComponent> App::line_interpolation(
	std::vector<TransformComponent>& positions, 
	int num_points) {
	std::vector<TransformComponent> result;

	// 確保至少有兩個點進行插值
	if (positions.size() < 2 || num_points <= 0) {
		return result;
	}

	// 遍歷所有相鄰點對進行插值
	for (size_t i = 0; i < positions.size() - 1; ++i) {
		const auto& start = positions[i];
		const auto& end = positions[i + 1];

		// 在 [0, 1) 區間生成 `num_points` 個比例值
		for (int j = 0; j < num_points; ++j) {
			float t = static_cast<float>(j) / static_cast<float>(num_points);

			// 插值計算位置
			TransformComponent interpolated;
			interpolated.position = {
				start.position[0] + t * (end.position[0] - start.position[0]),
				start.position[1] + t * (end.position[1] - start.position[1]),
				start.position[2] + t * (end.position[2] - start.position[2])
			};

			interpolated.eulers = start.eulers; 

			result.push_back(interpolated);
		}
	}

	return result;
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
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
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

	shader = make_shader(
		"src/shaders/vertex_i.txt", 
		"src/shaders/fragment.txt"
	);

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
	// renderSystem->draw_model(model_dict["ego_car"], transform);
	renderSystem->draw_model_ins_mat(model_dict["ego_car"], transform);
}

