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
		
	
	std::unordered_map<std::string, std::vector<std::string>> file_path_list = {
		{"ego_car", {"SUV.obj", "SUV.jpg"}},
		{"pedestrian", {"walking_person.obj", "walking_person.jpg"}},
		{"motorcycyle", {"scooter.obj", "scooter.jpg"}}, 
		{"truck", {"truck.obj", "truck.jpg"}}, 
		{"bus", {"bus.obj", "bus.jpg"}},
		{"traffic_cone", {"cone.obj", "cone.png"}},
		{"car", {"modern_car.obj", "modern_car.jpg"}}
	};

	// 產生 unordered map, key 是 obj name, value 是 (texture idx, VAO idx, vertex count)
	auto model_dict = factory->make_obj_list(file_path_list, {0.0f, 0.0f, 0.0f});

	
	unsigned int cameraEntity = factory->make_camera(
		{-5.0f, 0.0f, 5.0f}, {0.0f, -15.0f, 0.0f});

	CameraComponent* camera = new CameraComponent();
	app->model_dict = model_dict;
	app->cameraComponent = camera;
	app->cameraID = cameraEntity;

	app->set_up_opengl();
	app->make_systems();

	app->run();

	delete factory;
	delete app;
	return 0;
}