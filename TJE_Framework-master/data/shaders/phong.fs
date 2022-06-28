//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;
varying vec2 v_coord;

//here create uniforms for all the data we need here
uniform sampler2D u_texture;
uniform vec4 u_color;

//DECLAREM LES VARIABLES DE LIGHT
uniform vec3 ambient, light_pos, sun_color, diff_factor, spec_factor;
uniform float u_time;
uniform float glossiness;
uniform vec3 camera_eye;

void main()
{
	vec2 uv = v_uv;
	vec4 texture_normal = texture2D( u_texture, uv );

	//here write the computations for PHONG.

	//CALCULEM I NORMALITZEM ELS VECTORS L, V i N
	vec3 N = normalize((texture_normal.xyz * 2.0) - vec3(1.0));
	vec3 L = light_pos - v_world_position;
	L = normalize(L);
	vec3 R = -1 * reflect(L, N);
	vec3 V = camera_eye - v_world_position;
	V = normalize(V);

	//ambient
	//vec3 new_ambient = ambient * texture_normal.xyz;

	//diffuse
	vec3 diffuse = diff_factor * max(0.0,dot(N,L)) * sun_color;
	
	//specular
	vec3 specular = spec_factor * sun_color * pow( max(0.0, dot(R,V)), glossiness );
	//vec3 specular = (texture_normal.xyz * texture_normal.w) * pow(clamp(dot(R, V), 0.0, 1.0), 30.0) * diffuse_color * texture_normal.xyz;
	
	//final color
	//texture_normal = mat_color
	vec3 final_color = texture_normal.xyz * (ambient + diffuse) + specular;
	//vec3 Ip = ambient + diffuse + specular;

	
	//set the ouput color por the pixel
	gl_FragColor = vec4(final_color, 1.0);
	//gl_FragColor = vec4(Ip, 1.0);
	
	//gl_FragColor = u_color * texture2D( u_texture, uv );
}
