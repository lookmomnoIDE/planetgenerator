#version 330 core
out vec4 FragColor;

uniform vec4 newColor;
//uniform sampler2D texture1;
//uniform sampler2D texture2;

in vec4 ourColor;
//in vec2 TexCoord;

void main()
{
	FragColor = vec4(ourColor);// * newColor;
	//FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * vec4(ourColor) * vec4(newColor);

}