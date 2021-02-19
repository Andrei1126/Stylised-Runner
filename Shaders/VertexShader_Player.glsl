	#version 330

// TODO: get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_color;
layout(location = 2) in vec2 v_coord;
layout(location = 3) in vec3 v_normal;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Clock;
uniform float Noise;

// TODO: output values to fragment shader
out vec3 frag_color;
out vec3 frag_position;
out vec2 frag_coord;
out vec3 frag_normal;

void main()
{
	// TODO: send output to fragment shader
	frag_color = Clock * v_color;
	frag_position = v_position;
	frag_coord = v_coord;
	frag_normal = v_normal;

	if(Noise == 0){
		frag_position = v_position;	
	}
	else{
		// aplic functia de zgomot
		frag_position = Clock * v_position;
	}

	// TODO: compute gl_Position
	gl_Position = Projection * View * Model * vec4(frag_position, 1.0);
}
