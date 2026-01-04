#version 460 core

in vec2 texcoord0;
out vec4 color;
layout(std430, binding = 11) buffer pixels{
	ivec2 pixelWidth;
	vec4 pixelBuffer[1960*1080];
};
uniform layout(size4x32,binding = 0) coherent iimage2D uSampler;
void main()
{	
	ivec2 si = ivec2(1920,1080);
	int x = int(texcoord0.x*si.x);
	int y = int(texcoord0.y*si.y);
	ivec2 coord = ivec2(x,y);
	//color = pixelBuffer[ coord.x*1080 + coord.y];
	//if(color.w == 300){
	//float rsin = *asin(rs);
	color =  imageLoad(uSampler,ivec2(coord.x,coord.y));
	//}
	//vec4 color = vec4(100,100,0,0);
}