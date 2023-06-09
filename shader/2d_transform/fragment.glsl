#version 330 core


in vec3 color;
in vec2 coord;

uniform sampler2D mTexture1;
uniform sampler2D mTexture2;

out vec4 fragColor;

void main()
{
   // fragColor = texture(mTexture1, coord);
   
   // fragColor = texture(mTexture1, coord) * vec4(color, 1.0);
   fragColor = mix(texture(mTexture1, coord), texture(mTexture2, coord), 0.4);
   // fragColor = texture(mTexture1, coord) * 0.8 + texture(mTexture2, coord) * 0.2;
   // fragColor = vec4(aColor, 1.0f);
   // fragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
   // fragColor = color;
}