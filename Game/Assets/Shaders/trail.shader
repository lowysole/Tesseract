--- trailVertex

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV0;

uniform mat4 proj;
uniform mat4 view;

out vec2 uv0;

void main()
{
	gl_Position = proj*view*vec4(vertexPosition.xyz, 1.0);
	uv0 = vertexUV0;

}

--- trailFragment

in vec2 uv0;

uniform sampler2D diffuse;
uniform vec4 inputColor;

out vec4 outColor;

void main()
{	
	//outColor = inputColor;
	outColor = texture2D(diffuse, uv0);
}
