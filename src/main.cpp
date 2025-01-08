#include "config.h"
#include "controller/app.h"

#include "components/camera_component.h"
#include "components/render_component.h"
#include "components/transform_component.h"

#include "factories/factory.h"

int main() {

	App* app = new App();
	Factory* factory = new Factory(
		app->renderComponents, app->transformComponents);
		
	std::unordered_map<std::string, std::string> colors_path_list = {
		{"0", "0_0_0.png"},
		{"1", "255_120_50.png"},
		{"2", "255_192_203.png"},
		{"3", "255_255_0.png"},
		{"4", "0_150_245.png"},
		{"5", "0_255_255.png"},
		{"6", "255_127_0.png"},
		{"7", "255_0_0.png"},
		{"8", "255_240_150.png"},
		{"9", "135_60_0.png"},
		{"10", "160_32_240.png"},
		{"11", "50_50_50.png"},
		{"12", "139_137_137.png"},
		{"13", "75_0_75.png"},
		{"14", "150_240_80.png"},
		{"15", "230_230_250.png"},
		{"16", "0_175_0.png"},
	};

	std::unordered_map<std::string, std::vector<std::string>> file_path_list = {
		{"ego_car", {"SUV.obj", "SUV.jpg"}},
		{"pedestrian", {"walking_person.obj", "walking_person.jpg"}},
		{"motorcycyle", {"scooter.obj", "scooter.jpg"}}, 
		{"truck", {"truck.obj", "truck.jpg"}}, 
		{"bus", {"bus.obj", "bus.jpg"}},
		{"traffic_cone", {"cone.obj", "cone.png"}},
		{"car", {"modern_car.obj", "modern_car.jpg"}},
		{"0", {"cube.obj", "side.png"}},
		{"1", {"cube.obj", "crossroad.png"}},
		{"2", {"cube.obj", "roadline.png"}},
		{"occ_dot", {"cube4m.obj", "cube4m_frame.png"}}

	};

	// 產生 unordered map, key 是 obj name, value 是 (texture idx, VAO idx, vertex count)
	app->model_dict = factory->make_obj_list(file_path_list, {0.0f, 0.0f, 0.0f});
	app->color_dict = factory->make_color_list(colors_path_list);
	
	unsigned int cameraEntity = factory->make_camera(
		{-5.0f, 0.0f, 5.0f}, {0.0f, -15.0f, 0.0f});

	CameraComponent* camera = new CameraComponent();
	


	app->cameraComponent = camera;
	app->cameraID = cameraEntity;

	app->set_up_opengl();
	app->make_systems();

	app->run();

	delete factory;
	delete app;
	return 0;
}