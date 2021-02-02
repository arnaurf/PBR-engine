#ifndef MATERIAL_H
#define MATERIAL_H

#include "framework.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "extra/hdre.h"

class Material {
public:

	Shader* shader = NULL;
	Texture* texture = NULL;
	vec4 color;

	virtual void setUniforms(Camera* camera, Matrix44 model) = 0;
	virtual void render(Mesh* mesh, Matrix44 model, Camera * camera) = 0;
	virtual void renderInMenu() = 0;
};

class StandardMaterial : public Material {
public:

	StandardMaterial();
	~StandardMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};



class PhongMaterial : public Material {
public:

	PhongMaterial();
	~PhongMaterial();

	Vector3 mat_ambient; //No using when texture
	Vector3 mat_dif;	 //No using when texture
	Vector3 mat_spec;	 //No using when texture
	float mat_shine;

	Vector3 light_pos;
	Vector3 light_dif;
	Vector3 light_spec;

	Texture* normal_map = NULL;

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};


class ReflectiveMaterial : public Material {
public:

	ReflectiveMaterial();
	~ReflectiveMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};

class HBRMaterial : public StandardMaterial {
public:

	HBRMaterial();
	~HBRMaterial();

	Texture* cubemapTex[6];
	float roughness;
	float metallic;
	float scale_h = 0.15;
	Texture* normal_map = NULL;
	Texture* rough_map = NULL;
	Texture* metal_map = NULL;
	Texture* albedo = NULL;
	Texture* brdf_text = NULL;
	Texture* height_map = NULL;

	bool use_textures = 0;
	bool use_height_map = 0;

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
	void loadCubemapTex(HDRE* hdre);
};

class WireframeMaterial : public StandardMaterial {
public:

	WireframeMaterial();
	~WireframeMaterial();

	void render(Mesh* mesh, Matrix44 model, Camera * camera);
};

#endif