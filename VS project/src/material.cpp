#include "material.h"
#include "texture.h"
#include "application.h"
#include "extra/hdre.h"

StandardMaterial::StandardMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

StandardMaterial::~StandardMaterial()
{

}


void StandardMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);

	shader->setUniform("u_color", color);

	if (texture)
		shader->setUniform("u_texture", texture);
}

void StandardMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void StandardMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
}

WireframeMaterial::WireframeMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

WireframeMaterial::~WireframeMaterial()
{

}

void WireframeMaterial::render(Mesh* mesh, Matrix44 model, Camera * camera)
{
	if (shader && mesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//enable shader
		shader->enable();

		//upload material specific uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}


//-------------------------------------PHONG

PhongMaterial::PhongMaterial() {
	color = vec4(1.f, 1.f, 1.f, 0.2);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
	mat_ambient = Vector3(1, 0.3, 0.3);
	mat_dif = Vector3(1, 0.3, 0.3);
	mat_spec = Vector3(1, 0.3, 0.3);
	mat_shine = 20;

	light_dif = Vector3(1, 1, 1);
	light_spec = Vector3(1, 1, 1);
	light_pos = Vector3(1, 2, 1.5);
}

PhongMaterial::~PhongMaterial() {

}

void PhongMaterial::setUniforms(Camera* camera, Matrix44 model) {
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);

	shader->setUniform("u_color", color);

	shader->setUniform("mat_ambient", mat_ambient);
	shader->setUniform("mat_dif", mat_dif);
	shader->setUniform("mat_spec", mat_spec);
	shader->setUniform("light_pos", light_pos);
	shader->setUniform("light_dif", light_dif);
	shader->setUniform("light_spec", light_spec);
	shader->setUniform("spec", mat_spec);
	shader->setUniform("mat_shine", mat_shine);


	if (texture)
		shader->setUniform("u_texture", texture);

	if (this->normal_map)
		shader->setUniform("normal_map", normal_map);

}

void PhongMaterial::render(Mesh* mesh, Matrix44 model, Camera * camera) {
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void PhongMaterial::renderInMenu() {
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
	ImGui::ColorEdit3("Material Ambient", (float*)&mat_ambient); // Edit 3 floats representing a color
	ImGui::ColorEdit3("Material Difuse", (float*)&mat_dif); // Edit 3 floats representing a color
	ImGui::ColorEdit3("Material Specular", (float*)&mat_spec); // Edit 3 floats representing a color

	ImGui::DragFloat("Material Shine", (float*)&mat_shine, 0.1);
	ImGui::ColorEdit3("Light Difuse", (float*)&light_dif); // Edit 3 floats representing a color
	ImGui::ColorEdit3("Light Specular", (float*)&light_spec); // Edit 3 floats representing a color
	ImGui::DragFloat3("Light position", (float*)&light_pos); // Edit 3 floats representing a color

}


//--------------------------------- Reflective Material

ReflectiveMaterial::ReflectiveMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 0.2);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/reflective.fs");
	

}

ReflectiveMaterial::~ReflectiveMaterial()
{

}

void ReflectiveMaterial::render(Mesh* mesh, Matrix44 model, Camera * camera) {
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}

}

void ReflectiveMaterial::setUniforms(Camera* camera, Matrix44 model) {
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);

	shader->setUniform("u_color", color);
	

	if (texture)
		shader->setUniform("u_texture", texture);



}

void ReflectiveMaterial::renderInMenu() {
	

}



HBRMaterial::HBRMaterial()
{
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/skeleton_pbr.fs");
	Shader::Get("data/shaders/basic-h.vs", "data/shaders/skeleton_pbr_texture.fs");

	this->brdf_text = Texture::Get("data/brdfLUT.png");

	for (int i = 0; i < 6; i++) {
		this->cubemapTex[i] = new Texture();
	}

	color = Vector4(0.5, 0.5, 0.5, 1);
	roughness = 0.01;
	metallic = 0.3;
}
HBRMaterial::~HBRMaterial() {


}
void HBRMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);
	
	shader->setUniform("u_color", color);
	shader->setUniform("u_roughness", roughness);
	shader->setUniform("u_metallic", metallic);

	shader->setUniform("u_light_pos", vec3(8,3,8));

	shader->setUniform("u_texture", cubemapTex[0], 0);
	shader->setUniform("u_texture_prem_0", cubemapTex[1], 1);
	shader->setUniform("u_texture_prem_1", cubemapTex[2], 2);
	shader->setUniform("u_texture_prem_2", cubemapTex[3], 3);
	shader->setUniform("u_texture_prem_3", cubemapTex[4], 4);
	shader->setUniform("u_texture_prem_4", cubemapTex[5], 5);
	shader->setUniform("u_scale_h", scale_h);

	
	if (brdf_text)
		shader->setUniform("u_brdf_text", brdf_text,6);
	else shader->setUniform("u_brdf_text", Texture::getBlackTexture(), 6);

	if(normal_map)
		shader->setUniform("u_normal_map", normal_map,7);
	else shader->setUniform("u_normal_map", Texture::getBlackTexture(), 7);

	if(rough_map)
		shader->setUniform("u_rough_map", rough_map,8);
	else shader->setUniform("u_rough_map", Texture::getBlackTexture(), 8);

	if(metal_map)
		shader->setUniform("u_metal_map", metal_map,9);
	else shader->setUniform("u_metal_map", Texture::getBlackTexture(), 9);

	if (albedo)
		shader->setUniform("u_albedo", albedo, 10);
	else shader->setUniform("u_albedo", Texture::getBlackTexture(), 10);

	if (height_map && this->use_height_map)
		shader->setUniform("u_height_map", height_map, 11);
	else shader->setUniform("u_height_map", Texture::getBlackTexture(), 11);

	if (occlusion_map)
		shader->setUniform("u_ambient_occlusion", occlusion_map, 13);
	else shader->setUniform("u_ambient_occlusion", Texture::getBlackTexture(), 13);

	if (emissive_map)
		shader->setUniform("u_emissive_map", emissive_map, 14);
	else shader->setUniform("u_emissive_map", Texture::getBlackTexture(), 14);

	if (opacity_map) {
		shader->setUniform("u_opacity_map", opacity_map, 12);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

}

void HBRMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void HBRMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
	ImGui::DragFloat("Roughness", (float*)&roughness, 0.005, 0.0, 1.0);
	ImGui::DragFloat("Metallic", (float*)&metallic, 0.005, 0.0, 1.0);
	ImGui::DragFloat("Height-map factor", (float*)&scale_h, 0.005);
	if (ImGui::Checkbox("Use textures", &use_textures)) {
		this->shader  = Shader::Get("data/shaders/basic.vs", "data/shaders/skeleton_pbr_texture.fs");
	}
	if (ImGui::Checkbox("Use height map", &use_height_map) && use_textures == true && this->height_map!=NULL) {
		this->shader = Shader::Get("data/shaders/basic-h.vs", "data/shaders/skeleton_pbr_texture.fs");
	}

	if (use_height_map == false && use_textures == true) {
		this->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/skeleton_pbr_texture.fs");
	}
	if (!this->use_height_map && !this->use_textures) {
		this->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/skeleton_pbr.fs");
	}

}


void HBRMaterial::loadCubemapTex(HDRE* hdre) {
	for (int i = 0; i < 6; i++) {
		sHDRELevel h = hdre->getLevel(i);
		cubemapTex[i]->createCubemap(h.width, h.height, (Uint8**)h.faces);
	}
}