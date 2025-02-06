#include "app.h"

void App::draw_objs() {
	// 繪製道路物件
	int obj_scale = OBJ_SCALE;
	for(auto& obj: cur_frame_data["obj"]) {
		float coord_x = -obj["x"].get<float>() * obj_scale;	
		float coord_y = -obj["y"].get<float>() * obj_scale + 5;		
		float angle = obj["ang"].get<float>() + 180;
		std::string obj_name = obj["cls"].get<std::string>();

		// 計算與原點距離 (給 BEV mode 使用)
		if(obj_name == "car") {
			float distance = cal_distance({coord_y, coord_x}, {0.0f, 0.0f});
			
			if (distance < MAX_DANGER_DISTANCE) 
			dangerous_objs.push_back({distance, {coord_y, coord_x}});	
		}
		

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
	int line_scale = LINE_SCALE;
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
				t.position = {(y_list[idx] * line_scale - (line_scale / 2.0f) + 5),
							  (x_list[idx] * line_scale - (line_scale / 2.0f)), 
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

void App::draw_ego_car_BEV() {

	// 切換 shader 需要重新傳 Uniform 變數
	switch_to_shader(shader_dict["ego"]);
	shader_dict["ego"]->set_proj_view_mat(projection, view);

	unsigned int num_objs = dangerous_objs.size();

	// 將 distance 做升冪排序
	sort(dangerous_objs.begin(), dangerous_objs.end(), 
	[](const std::pair<float, std::pair<float, float>>& a, 
	   const std::pair<float, std::pair<float, float>>& b) {
			return a.first < b.first;
	   });

	// 保留 top-10 elements
	if(num_objs >= 10) {
		dangerous_objs.resize(10);
		num_objs = dangerous_objs.size();
	}
		
	for(int i = 0; i < num_objs; i++){
		tempArray[i * 3 + 0] = dangerous_objs[i].second.first;
		tempArray[i * 3 + 1] = dangerous_objs[i].second.second;
		tempArray[i * 3 + 2] = 2.0f;
	}

	// 設定光照數量(int)和光源位置(float array)至 shader 的 uniform 變數
	shader_dict["ego"]->Uniform1i("numLights", num_objs);
	shader_dict["ego"]->Uniform3fv_arr("lightPositions", tempArray, num_objs);
	
	shader_dict["ego"]->draw_model(model_dict["ego_car"], ego_car_pos);
	
	// 清除工作
	memset(tempArray, 0, sizeof(tempArray));
	dangerous_objs.clear();

	// 切換回 base shader
	switch_to_shader(shader_dict["base"]);
	shader_dict["base"]->set_proj_view_mat(projection, view);
	
}

void App::draw_ego_car() {
	TransformComponent transform;
	transform.position = {5.0f, 0.0f, 0.0f};
	transform.eulers = {0.0f, 0.0f, 0.0f};
	// renderSystem->draw_model(model_dict["ego_car"], transform);
	renderSystem->draw_model_ins_mat(model_dict["ego_car"], transform);
}