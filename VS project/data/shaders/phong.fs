//this var comes from the vertex shader
//they are interpolated by pixel according to the distance from the vertex
uniform vec3 light_pos;	
uniform vec3 light_dif;	
uniform vec3 light_spec;	


uniform vec4 u_color;
uniform vec3 mat_ambient;
uniform vec3 mat_dif;		
uniform vec3 mat_spec;		
uniform float mat_shine;	
uniform vec3 u_camera_position;
uniform mat4 u_model;
uniform float spec;

varying vec3 v_world_position;
varying vec3 v_normal;


void main()
{
	
	//read from the varying vars (the ones from the vertex shader)
	vec3 N = normalize(v_normal);
	vec3 L = normalize(light_pos - v_world_position);
	vec3 V = normalize(u_camera_position - v_world_position);
	vec3 R = -reflect(L, N);
	float LdotN = dot(L, N);
	float RdotV = dot(R, V);

	vec3 ambient = mat_ambient * u_color;
	vec3 diffuse = mat_dif * u_color * vec3(max(0.0f, LdotN)) * light_dif;
	vec3 specular = mat_spec * u_color * vec3(pow(max(0.0f, RdotV), mat_shine))* light_spec;
	
	vec3 v_color = ambient + diffuse + specular;

	//set the ouput color por the pixel
	gl_FragColor = vec4( v_color, 1.0 );
}
