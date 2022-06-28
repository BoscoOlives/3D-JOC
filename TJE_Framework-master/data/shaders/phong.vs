//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;

//vars to pass to the pixel shader
varying vec2 v_coord;
varying vec3 v_wPos;


//here create uniforms for all the data we need here

void main()
{	
	vec3 wPos = (model * gl_Vertex).xyz;
	v_wPos = wPos;

	v_coord = gl_MultiTexCoord0.xy;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(wPos,1.0); //output of the vertex shader
}