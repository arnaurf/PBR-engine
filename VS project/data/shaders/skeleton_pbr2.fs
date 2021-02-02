
// declare here your constants
#define PI				3.14159265359
#define RECIPROCAL_PI	0.3183098861837697 // 1 / PI

varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;

uniform vec3 u_camera_position;
uniform vec4 u_color;

// Levels of HDRE
uniform samplerCube u_texture;
uniform samplerCube u_texture_prem_0;
uniform samplerCube u_texture_prem_1;
uniform samplerCube u_texture_prem_2;
uniform samplerCube u_texture_prem_3;
uniform samplerCube u_texture_prem_4;

uniform sampler2D u_brdf_text;
uniform sampler2D u_normal_map;
uniform sampler2D u_rough_map;
uniform sampler2D u_metal_map;
uniform sampler2D u_albedo;


// Upload the rest of uniforms
// TO DO
// ...


uniform float u_roughness;
uniform float u_metallic;
uniform float u_alpha;



uniform vec3 u_light_pos;


struct Pbr_material {

	float roughness;
	float metallic;

	vec3 specular;
	vec3 diffuse;
	vec3 N;
	vec3 R;
	vec3 L;
	vec3 H; //normal microfacet
	
};

// don't touch this
// getReflectionColor:	Get pixel from HDRE (passed in separated textures)
//						using a 3D direction and a defined roughness
vec3 getReflectionColor(vec3 r, float roughness)
{
	float lod = roughness * 5.0;

	vec4 color;

	if(lod < 1.0) color = mix( textureCube(u_texture, r), textureCube(u_texture_prem_0, r), lod );
	else if(lod < 2.0) color = mix( textureCube(u_texture_prem_0, r), textureCube(u_texture_prem_1, r), lod - 1.0 );
	else if(lod < 3.0) color = mix( textureCube(u_texture_prem_1, r), textureCube(u_texture_prem_2, r), lod - 2.0 );
	else if(lod < 4.0) color = mix( textureCube(u_texture_prem_2, r), textureCube(u_texture_prem_3, r), lod - 3.0 );
	else if(lod < 5.0) color = mix( textureCube(u_texture_prem_3, r), textureCube(u_texture_prem_4, r), lod - 4.0 );
	else color = textureCube(u_texture_prem_4, r);

	color = pow(color, vec4(1.0/2.2));

	return color.rgb;
}

// don't touch these neither
// perturbNormal:	Modify material normal using normal texture
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv){
	
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p );
	vec2 duv1 = dFdx( uv );
	vec2 duv2 = dFdy( uv );

	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 );
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
	return mat3( T * invmax, B * invmax, N );
}
vec3 perturbNormal( vec3 N, vec3 V, vec2 texcoord, vec3 normal_pixel ){

	normal_pixel = normal_pixel * 255./127. - 128./127.;
	mat3 TBN = cotangent_frame(N, V, texcoord);
	return normalize(TBN * normal_pixel);
}

float G1( vec3 v, vec3 n, float roughness ){
	

	float aux = clamp(dot(n,v),0.001,0.99);
	float k = max(pow(roughness+1.0,2.0)/8.0, 0.001);
	float ret = aux/(aux*(1.0-k)+k);
	return ret;
}


void main()
{
	
	vec4 color = texture2D(u_albedo, v_uv);
	//texture2D(u_albedo, vec2(0.1,0.1))
	vec4 aux = texture2D(u_normal_map, v_uv);
	vec3 l = normalize( u_light_pos - v_world_position );
	vec3 n = normalize(v_normal);
	vec3 v = normalize(u_camera_position - v_world_position);
	vec3 h = normalize(l+v);
	vec3 n2 = normalize(aux.rgb);

	//clamping dots

	float ldoth = clamp(dot(l,h),0.001,0.99);
	float ndoth = clamp(dot(n,h),0.001,0.99);
	float ndotv = clamp(dot(n,v),0.001,0.99);
	float vdoth = clamp(dot(v,h),0.001,0.99);
	float vdotl = clamp(dot(v,l),0.001,0.99);
	float ldotn = clamp(dot(l,n),0.001,0.99);
	float ndotl = clamp(dot(n,l),0.001,0.99);
	


	vec3 f0_specular = vec3(0.04);

	Pbr_material material;

	aux = texture2D(u_rough_map, v_uv);
	material.roughness = aux.r;
	//material.roughness = u_roughness;


	aux = texture2D(u_metal_map, v_uv);
	material.metallic = aux.r;
	//material.metallic = u_metallic;

	material.diffuse = vec3(0.0)*material.metallic + color.rgb *(1.0 - material.metallic);
	material.specular = color.rgb*material.metallic + f0_specular *(1.0 - material.metallic);
	
	//float G = 1/pow(ldoth, 2.0);
	float G = G1(l, n2, material.roughness)*G1(v, n2, material.roughness);
	

	//PUNCTUAL LIGHTING

	//float G = min(min(1.0, 2.0*ndoth*ndotv/vdoth), 2.0*ndoth*ndotl/vdoth);
	float alpha = pow(material.roughness,2.0);
	
	float D = pow(alpha, 2.0)/(PI*pow(pow(ndoth,2.0)*(pow(alpha,2.0)-1.0)+1.0,2.0));
	//float D = (u_alpha + 2)/(2*PI)*pow(ndoth, u_alpha);
	
	//vec3 fresnel = material.specular + (1.0-material.specular)*pow(1.0-dot(l,n),5.0);
	//vec3 fresnel = material.specular + (1.0 - material.specular)*pow(2, (-5.55473*dot(l,n2)-6.98316)*dot(l,n2));
	
	vec3 f_pfacet = G*D*fresnel/(4*clamp(dot(n,l), 0.15, 0.99)*dot(n,v));
	vec3 f_lambert = material.diffuse/PI;
	

	vec3 f_pfacet = G*D*fresnel;
	//vec3 f_pfacet = G*;
	// simple tone-mapping


	vec3 punctual = f_lambert + f_pfacet;



	//IBL ENVIRONMENT LIGHTING
	//float ibl_brdf = material.diffuse/PI;

	vec3 ibl_brdf = getReflectionColor(reflect(v, normalize(n)), material.roughness);
	vec4 text2 =  texture2D(u_brdf_text, vec2(dot(n, v),material.roughness)) ;
	float a = text2.r;
	float b = text2.g;
	vec3 brdf_specular = f0_specular*a + b;

	vec3 environment = ibl_brdf + brdf_specular;
	environment = environment*1;
	//vec3 env = diffuse_component + ibl_brdf * brdf_specular;
	//vec3 environment = vec3(brdf_specular.r*ibl_brdf.r,brdf_specular.g*ibl_brdf.g, brdf_specular.b*ibl_brdf.b);


	vec3 full_light = punctual + environment;



	//color.rgb /= (color.rgb + vec3(1.0));
	
	// gamma correct 
	//color.rgb = pow(color.rgb, vec3(1.0/2.2));

	// Final color
	
	gl_FragColor = vec4(full_light, 1.0);
}