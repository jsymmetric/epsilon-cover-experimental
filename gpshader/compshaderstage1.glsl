
#version 460 core

layout(local_size_x = 1,local_size_y = 1,local_size_z = 1) in;

layout(std430,binding = 0) buffer rotdata{
	vec3 scale;
	vec4 rotation[10*1000];
};
layout(std140,binding = 1) buffer facedata{
	vec4 Faces[100*1000*3];
};

layout(std430,binding = 2) buffer usefulfloats{
	float rs;
	float rc;
	float xpos;
	float ypos;	
	float zpos;
};
layout(std430, binding = 8) buffer tracking{
	int tracker[10];
};
layout(std430, binding = 9) buffer finalCubes{
	ivec3 highpos;
	ivec3 midpos;
	int granularEnd;
	int toplevel[3*1000*1000*5];
	//ivec4 granular[3*1000*1000];
};
layout(std430, binding = 10) buffer finalCubes0{
	int toplevel1[3*1000*1000*5];
};
layout(std430, binding = 11) buffer finalCubes1{
	ivec4 granular1[3*1000*1000];
};
layout(std430, binding = 12) buffer finalCubes2{
	ivec4 granular2[3*1000*1000];
};

layout(std430, binding = 13) buffer pixels{
	ivec2 pixelWidth;
	vec4 pixelBuffer[1920*1080];
};
layout(std430, binding =14) buffer floatDebug{
    vec4 floatd[1000*1000];
};
layout(std430, binding = 15) buffer transform{
	mat3 projection;
};
layout(std430, binding =16) buffer intDebug{
    ivec4 intd[1000*1000];
};
layout(std430, binding = 17) buffer lattice{
	int latticeInc;
	ivec4 finalLattice[400*1000*13];
};
layout(std430, binding = 18) buffer lattice2{
	ivec4 finalLattice2[400*1000*13];
};
layout(std430, binding = 19) buffer lattice3{
	ivec4 finalLattice3[400*1000*13];
};
layout(std430, binding = 20) buffer lattice4{
	ivec4 finalLattice4[400*1000*13];
};
layout(std430, binding = 21) buffer lattice5{
	ivec4 finalLattice5[400*1000*13];
};
layout(std430, binding = 22) buffer lattice6{
	ivec4 finalLattice6[400*1000*13];
};
layout(std430, binding = 23) buffer lattice7{
	ivec4 finalLattice7[400*1000*13];
};
layout(std430, binding = 24) buffer lattice8{
	ivec4 finalLattice8[400*1000*13];
};
layout(std430, binding = 25) buffer lattice9{
	ivec4 finalLattice9[400*1000*13];
};
layout(std430, binding = 26) buffer lattice10{
	ivec4 finalLattice10[400*1000*13];
};
layout(std430, binding = 27) buffer lattice11{
	ivec4 finalLattice11[400*1000*13];
};
layout(std430, binding = 28) buffer lattice12{
	ivec4 finalLattice12[400*1000*13];
};
layout(std430, binding = 29) buffer lattice13{
	ivec4 finalLattice13[400*1000*13];
};
layout(std430, binding = 30) buffer lattice14{
	ivec4 finalLattice14[400*1000*13];
};
layout(std430, binding = 31) buffer lattice15{
	ivec4 finalLattice15[400*1000*13];
};
void main(){
	atomicAdd(tracker[9],1);
	uint x = gl_WorkGroupID.x*3;
	
	vec3 vert1 = Faces[x].xyz;
	x += 1;
	vec3 vert2 = Faces[x].xyz;
	x+= 1;
	vec3 vert3 = Faces[x].xyz;
	
	vec3 line1 = vert1-vert3;
	vec3 line2 = vert2 - vert3;
	vec3 dest = vec3(0,0,1);

	vec3 normal = cross(line1,line2);
	vec3 norm = normalize(normal);
	if (dot(norm, dest) < 0.0) {
		norm = -norm;   // flip to reduce rotation angle
	}
	vec3 a = normalize(cross(dest,norm));
	float b = dot(dest,norm);
	float c = 1.0/(1.0+b);
	float theta = acos(b/(length(dest)*length(norm)));
	mat3 id = mat3(1.0);
	mat3 om = mat3(
		0,-a.z,a.y,
		a.z,0,-a.x,
		-a.y,a.x,0
	);
	mat3 om2 = om*om;
	mat3 rot = id + sin(theta)*om + (1 - cos(theta))*om2;
	/*int tracloc = atomicAdd(tracker[7],2);
	floatd[tracloc].xyzw = vec4(vert1,x);//vec4((a1*b2 - a2*b1)/disc,gl_WorkGroupID.x,gl_WorkGroupID.y,gl_WorkGroupID.z);
	//floatd[tracloc].w = float(10000);
	floatd[tracloc+1].xyz = cross(vec3(-1,1,0),vec3(0,1,0));
	*/
	x = gl_WorkGroupID.x*3;
	vert1 = rot*vert1;
	vert2 = rot*vert2;
	vert3 = rot*vert3;
	//vert1 = vert1*cos(theta) + cross(a,vert1)*sin(theta) + a*dot(a,vert1)*(1-cos(theta));
	//vert2 = vert2*cos(theta) + cross(a,vert2)*sin(theta) + a*dot(a,vert2)*(1-cos(theta));
	//vert3 = vert3*cos(theta) + cross(a,vert3)*sin(theta) + a*dot(a,vert3)*(1-cos(theta));
	Faces[x].xyz = vert1;
	Faces[x+1].xyz = vert2;
	Faces[x+2].xyz = vert3;
	rotation[gl_WorkGroupID.x] = vec4(a.x,a.y,a.z,theta);

}