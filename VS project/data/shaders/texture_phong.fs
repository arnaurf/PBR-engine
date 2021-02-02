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

varying vec3 v_position;
varying vec3 normal;
//here create uniforms for all the data we need here

varying vec2 v_uv;
varying vec2 norm_coord;

uniform sampler2D u_texture;
uniform sampler2D normal_map;

void main()
{

	vec2 v_coord = v_uv;
	
	//read from the varying vars (the ones from the vertex shader)
	vec4 color = texture2D( u_texture, v_coord );

	vec4 norm_vec4 = texture2D( normal_map, v_coord )*vec4(2.0)-1.0;
	vec3 norm_vec3 = normalize((u_model * norm_vec4).xyz);
	vec3 L = normalize(light_pos - v_position);
	vec3 V = normalize(u_camera_position - v_position);
	vec3 R = -reflect(L, norm_vec3);
	float LdotN = dot(L, norm_vec3);
	float RdotV = dot(R, V);

	vec3 ambient = color.xyz * mat_ambient.x;
	vec3 diffuse = color.xyz * mat_dif.x * vec3(max(0.0f, LdotN)) * light_dif;
	vec3 specular = color.a * mat_spec.x * vec3(pow(max(0.0f, RdotV), mat_shine))* light_spec;
	
	vec3 v_color = ambient + diffuse + specular;

	//set the ouput color por the pixel
	gl_FragColor = vec4( v_color, 1.0 );
}
