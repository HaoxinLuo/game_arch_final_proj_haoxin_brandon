#version 400

uniform vec3 u_color;

in vec3 color;

void main(void)
{
	gl_FragColor = vec4(color, 1.0);
}