#version 330

// TODO: get color value from vertex shader
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 lightPositionFirst;
uniform vec3 lightPositionSecond;
uniform vec3 lightPositionThird;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

uniform int spot;
uniform float angle;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 L = normalize( lightPositionFirst - world_position );
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );
	vec3 R = reflect( L, world_normal );

	// TODO: define ambient light component
	float ambient_light = material_kd * 0.25;

	// TODO: compute diffuse light component
	float diffuse_light = material_kd * max(dot(L, world_normal), 0);

	// TODO: compute specular light component
	float specular_light = material_ks * pow(max(dot(R, V), 0), material_shininess);


	float cut_off = angle;
	float spot_light = dot(-L, light_direction);
	float spot_light_limit = cos(cut_off); 
	// Quadratic attenuation
	float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
	float light_att_fac = pow(linear_att, 2);
	float aten_fac = 1 / pow(length(lightPositionFirst - world_position), 2);
	vec3 light;
	
	if (spot == 0) {
		light = object_color * (ambient_light + aten_fac * (diffuse_light + specular_light));
	} else {

		if (spot_light > cos(cut_off))
		{	
			light = object_color * (ambient_light + (light_att_fac + aten_fac) * (diffuse_light + specular_light));
		} else {
			light = ambient_light * object_color;
		}
	}

	

	L = normalize( lightPositionSecond - world_position );
	V = normalize( eye_position - world_position );
	H = normalize( L + V );
	R = reflect( L, world_normal );

	// TODO: define ambient light component
	ambient_light = material_kd * 0.25;

	// TODO: compute diffuse light component
	diffuse_light = material_kd * max(dot(L, world_normal), 0);

	// TODO: compute specular light component
	specular_light = material_ks * pow(max(dot(R, V), 0), material_shininess);


	cut_off = angle;
	spot_light = dot(-L, light_direction);
	spot_light_limit = cos(cut_off); 
	// Quadratic attenuation
	linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
	light_att_fac = pow(linear_att, 2);
	aten_fac = 1 / pow(length(lightPositionSecond - world_position), 2);
	
	if (spot == 0) {
		light = object_color * (ambient_light + aten_fac * (diffuse_light + specular_light));
	} else {

		if (spot_light > cos(cut_off))
		{	
			light += object_color * (ambient_light + (light_att_fac + aten_fac) * (diffuse_light + specular_light));
		} else {
			light += ambient_light * object_color;
		}
	}



	L = normalize( lightPositionThird - world_position );
	V = normalize( eye_position - world_position );
	H = normalize( L + V );
	R = reflect( L, world_normal );

	// TODO: define ambient light component
	ambient_light = material_kd * 0.25;

	// TODO: compute diffuse light component
	diffuse_light = material_kd * max(dot(L, world_normal), 0);

	// TODO: compute specular light component
	specular_light = material_ks * pow(max(dot(R, V), 0), material_shininess);


	aten_fac = 1 / pow(length(lightPositionThird - world_position), 2);
	
	light += object_color * (ambient_light + aten_fac * (diffuse_light + specular_light));
	


	// TODO: write pixel out color
	out_color = vec4(light, 1);
}