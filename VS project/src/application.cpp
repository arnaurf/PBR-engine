#include "application.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "volume.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include "extra/hdre.h"
#include "includes.h"

#include <cmath>

Application* Application::instance = NULL;
Camera* Application::camera = nullptr;

Shader* sh_simple;
Shader* sh_text;
Shader* sh_text_height;

Application::Application(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;
	render_debug = true;
	render_wireframe = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	// OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	// Create camera
	camera = new Camera();
	camera->lookAt(Vector3(15.f, 15.0f, 25.f), Vector3(0.f, 0.0f, 0.f), Vector3(0.f, 1.f, 0.f));
	camera->setPerspective(45.f,window_width/(float)window_height,0.1f,10000.f);

	// Create scene node 
	SceneNode * node = new SceneNode("Rendered node");
	

	// Set mesh and manipulate model matrix
	node->mesh = Mesh::Get("data/meshes/sphere.obj");
	node->model.setScale(4, 4, 4);

	//node->mesh = Mesh::Get("data/meshes/export.OBJ");
	//node->model.setScale(0.1,0.1, 0.1);

	// Create node material
	
	
	HDRE * hdre = new HDRE("data/environments/environment.hdre");

	//create GPU texture to store environment info

	Texture * cubemapTex = new Texture();
	//This only stores the first level(Specular reflection -> 0 roughness)

	cubemapTex->createCubemap(hdre->width, hdre->height, (Uint8**)hdre->getFaces(0));

	Skybox* sky = new Skybox();
	root.push_back(sky);
	root.push_back(node);

	//------------------- HBRE ---------------
	
	HBRMaterial * material = new HBRMaterial();
	material->loadCubemapTex(hdre);
	node->material = material;

	/*
	
	material->metal_map = Texture::Get("data/models/ball/metalness.png");
	material->normal_map = Texture::Get("data/models/ball/normal.png");
	material->rough_map = Texture::Get("data/models/ball/roughness.png");
	material->albedo = Texture::Get("data/models/ball/albedo.png");
	
	material->metal_map = Texture::Get("data/pbr/metalgrid/metalgrid2_metallic.png");
	material->normal_map = Texture::Get("data/pbr/metalgrid/metalgrid2_normal-dx.png");
	material->rough_map = Texture::Get("data/pbr/metalgrid/metalgrid2_roughness.png");
	material->albedo = Texture::Get("data/pbr/metalgrid/metalgrid2_AO.png");
	
	*/
	material->albedo = Texture::Get("data/pbr/militar/military-panel1-albedo.png");
	material->normal_map = Texture::Get("data/pbr/militar/military-panel1-nmap-ogl.png");
	material->metal_map = Texture::Get("data/pbr/militar/military-panel1-metalness.png");
	material->rough_map = Texture::Get("data/pbr/militar/military-panel1-metalness.png");
	material->occlusion_map = Texture::Get("data/pbr/militar/military-panel1-ao.png");
	material->height_map = Texture::Get("data/pbr/militar/military-panel1-height_.png");
	material->emissive_map = Texture::Get("data/pbr/militar/military-panel1-emissive_power.png");
	
	
	/*
	material->metal_map = Texture::Get("data/pbr/cloth/worn-blue-burlap-Metallic.png");
	material->normal_map = Texture::Get("data/pbr/cloth/worn-blue-burlap-Normal-dx.png");
	material->rough_map = Texture::Get("data/pbr/cloth/worn-blue-burlap-Roughness.png");
	material->albedo = Texture::Get("data/pbr/cloth/worn-blue-burlap-albedo.png");
	material->height_map = Texture::Get("data/pbr/cloth/worn-blue-burlap-Height.png");
	
	
	material->albedo = Texture::Get("data/pbr/lantern/lantern_Base_Color.png");
	material->normal_map = Texture::Get("data/pbr/lantern/lantern_Normal_OpenGL.png");
	material->metal_map = Texture::Get("data/pbr/lantern/lantern_Metallic.png");
	material->rough_map = Texture::Get("data/pbr/lantern/lantern_Roughness.png");
	material->opacity_map = Texture::Get("data/pbr/lantern/lantern_Opacity.png");
	*/


	sh_simple = Shader::Get("data/shaders/basic.vs", "data/shaders/skeleton_pbr.fs");
	sh_text = Shader::Get("data/shaders/basic.vs", "data/shaders/skeleton_pbr_texture.fs");
	sh_text_height = Shader::Get("data/shaders/basic.vs", "data/shaders/skeleton_pbr_texture.fs");



	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Application::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < root.size(); i++) {
		root[i]->render(camera);

		if(render_wireframe)
			root[i]->renderWireframe(camera);
	}

	//Draw the floor grid
	if(render_debug)
		drawGrid();
}

void Application::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * 10; //the speed is defined by the seconds_elapsed so it goes constant
	float orbit_speed = seconds_elapsed *5.5;
	/*
	for (int i = 0; i < root.size(); i++) {
)		if (typeid(root[i]) == typeid(HBRMaterial)) {
			HBRMaterial* aux = (HBRMaterial*)root[i];
			if (aux->use_height_map && aux->use_textures) {
				aux->shader = sh_text_height;
			}
			else if (aux->use_height_map && !aux->use_textures) {
				aux->shader = sh_text;
			}
			else if (!aux->use_height_map && !aux->use_textures) {
				aux->shader = sh_simple;
			}
		};

	}
	*/
	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT && !ImGui::IsAnyWindowHovered() 
		&& !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())) //is left button pressed?
	{
		camera->orbit(-Input::mouse_delta.x * orbit_speed, Input::mouse_delta.y * orbit_speed);
	}

	//async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) camera->moveGlobal(Vector3(0.0f, -1.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_LCTRL)) camera->moveGlobal(Vector3(0.0f,  1.0f, 0.0f) * speed);

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: render_debug = !render_debug; break;
		case SDLK_F5: Shader::ReloadAll(); break; 
	}
}

void Application::onKeyUp(SDL_KeyboardEvent event)
{
}

void Application::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Application::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Application::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Application::onMouseWheel(SDL_MouseWheelEvent event)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (event.type)
	{
		case SDL_MOUSEWHEEL:
		{
			if (event.x > 0) io.MouseWheelH += 1;
			if (event.x < 0) io.MouseWheelH -= 1;
			if (event.y > 0) io.MouseWheel += 1;
			if (event.y < 0) io.MouseWheel -= 1;
		}
	}

	if(!ImGui::IsAnyWindowHovered() && event.y)
		camera->changeDistance(event.y * 0.5);
}

void Application::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

