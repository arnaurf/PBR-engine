uniform vec4 u_color;

uniform samplerCube u_texture;

uniform vec3 u_camera_position;

uniform vec3 light_pos;

varying vec3 v_normal;
varying vec3 v_world_position;


void main()
{
	
	vec3 ray_vec = normalize( u_camera_position - v_world_position);

	//vec3 L = normalize(light_pos - v_world_position);

	vec3 normal = normalize(v_normal);
	vec3 R = reflect(ray_vec,normal);

	vec4 ref = textureCube(u_texture, R);
	gl_FragColor = vec4(ref);
}