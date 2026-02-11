
#version 460 core

layout(local_size_x = 1,local_size_y = 1,local_size_z = 1) in;

layout(std430,binding = 0) buffer rotdata{
	vec3 scale;
	vec4 rot[10*1000];
};
layout(std430,binding = 1) buffer facedata{
	vec4 Faces[100*1000*3];
};
layout(std430,binding = 2) buffer trilist{
	int tripos1[1000*5000];
};
layout(std430,binding = 3) buffer trilist2{
	int tripos2[1000*5000];
};
layout(std430,binding = 4) buffer trilist3{
	int tripos3[1000*5000];
};
layout(std430,binding = 5) buffer cubelist{
	ivec3 cubepos1[1000];
};
layout(std430,binding = 6) buffer cubelist2{
	ivec3 cubepos2[1000];
};
layout(std430,binding = 7) buffer cubelist3{
	ivec3 cubepos3[1000];
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
	mat4 projection;
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
uniform layout(size4x32,binding = 0) coherent iimage2D uSampler;
vec3 corner[8];
int adj[12][6];
int triiter = 0;
void set_lattice(int pos, int adj, ivec4 add){
	int t = pos;
	if(t  < 400*1000){
		finalLattice[pos*13 + adj] = add;
	}
	if(t >= 400*1000 && t  < 800*1000){
		finalLattice2[pos*13 + adj - 400*1000*13] = add;
	}
	if(t >= 800*1000 && t  < 1200*1000){
		finalLattice3[pos*13 + adj - 800*1000*13] = add;
	}
	if(t >= 1200*1000 && t  < 1600*1000){
		finalLattice4[pos*13 + adj -1200*1000*13] = add;
	}
	if(t >= 1600*1000 && t  < 2000*1000){
		finalLattice5[pos*13 + adj-1600*1000*13] = add;
	}
	if(t >= 2000*1000 && t  < 2400*1000){
		finalLattice6[pos*13 + adj - 2000*1000*13] = add;
	}
	if(t >= 2400*1000 && t  < 2800*1000){
		finalLattice7[pos*13 + adj - 2400*1000*13] = add;
	}
	if(t >= 2800*1000 && t  < 3200*1000){
		finalLattice8[pos*13 + adj -2800*1000*13] = add;
	}
	if(t >= 3200*1000 && t  < 3600*1000){
		finalLattice9[pos*13 + adj-3200*1000*13] = add;
	}
	if(t >= 3600*1000 && t  < 4000*1000){
		finalLattice10[pos*13 + adj - 3600*1000*13] = add;
	}
	if(t >= 4000*1000 && t  < 4400*1000){
		finalLattice11[pos*13 + adj - 4000*1000*13] = add;
	}
	if(t >= 4400*1000 && t  < 4800*1000){
		finalLattice12[pos*13 + adj -4400*1000*13] = add;
	}
	if(t >= 4800*1000 && t  < 5200*1000){
		finalLattice13[pos*13 + adj-4800*1000*13] = add;
	}
	if(t >= 5200*1000 && t  < 5600*1000){
		finalLattice14[pos*13 + adj -5200*1000*13] = add;
	}
	if(t >= 5600*1000 && t  < 6000*1000){
		finalLattice15[pos*13 + adj-5600*1000*13] = add;
	}
}
ivec4 get_lattice(int pos, int adj){
//pos = pos + 2000*1000;
	int t = pos;
	if(t  < 400*1000){
		return finalLattice[pos*13 + adj];
	}
	if(t >= 400*1000 && t  < 800*1000){
		return finalLattice2[pos*13 + adj-400*1000*13];
	}
	if(t >= 800*1000 && t  < 1200*1000){
		return finalLattice3[pos*13 + adj-800*1000*13];
	}
	if(t >= 1200*1000 && t  < 1600*1000){
		return finalLattice4[pos*13 + adj-1200*1000*13];
	}
	if(t >= 1600*1000 && t  < 2000*1000){
		return finalLattice5[pos*13 + adj-1600*1000*13];
	}
	if(t >= 2000*1000 && t  < 2400*1000){
		return finalLattice6[pos*13 + adj - 2000*1000*13];
	}
	if(t >= 2400*1000 && t  < 2800*1000){
		return finalLattice7[pos*13 + adj - 2400*1000*13];
	}
	if(t >= 2800*1000 && t  < 3200*1000){
		return finalLattice8[pos*13 + adj -2800*1000*13];
	}
	if(t >= 3200*1000 && t  < 3600*1000){
		return finalLattice9[pos*13 + adj-3200*1000*13];
	}
	if(t >= 3600*1000 && t  < 4000*1000){
		return finalLattice10[pos*13 + adj - 3600*1000*13];
	}
	if(t >= 4000*1000 && t  < 4400*1000){
		return finalLattice11[pos*13 + adj - 4000*1000*13];
	}
	if(t >= 4400*1000 && t  < 4800*1000){
		return finalLattice12[pos*13 + adj -4400*1000*13];
	}
	if(t >= 4800*1000 && t  < 5200*1000){
		return finalLattice13[pos*13 + adj-4800*1000*13];
	}
	if(t >= 5200*1000 && t  < 5600*1000){
		return finalLattice14[pos*13 + adj -5200*1000*13];
	}
	if(t >= 5600*1000 && t  < 6000*1000){
		return finalLattice15[pos*13 + adj-5600*1000*13];
	}
	return ivec4(-1,-1,-1,-1);
}

ivec3 getVecCoord(int place){
	int y = 0;
	int x = 0;
	int z = place%1000;
	if(place-z > 0){
		y = ((place-z)/(1000*highpos.z))%1000;
		if(place - y - z > 0){
			x = (place-y-z)/(1000*1000*highpos.y*highpos.z);
		}
	}
	return ivec3(x,y,z);
}
ivec3 getVectorCoord(ivec3 coord){
	ivec3 ret = ivec3(0,0,0);
	ret.x = coord.x - coord.x%10;
	ret.y = coord.y - coord.y%10;
	ret.z = coord.z - coord.z%10;
	return ret;
}
bool inBox(vec3 coord){
	float upper = 1080.0/1920.0;
	//if(finalLattice[gl_WorkGroupID.x*27].y > 800){

	//}
	if(coord.y >= 0 && coord.y < upper){

		if(coord.x >= 0 && coord.x < 1){
			return true;
		}
	}
	return false;	
}
void main(){
	vec3 coord = vec3(get_lattice(int(gl_WorkGroupID.x),0).xyz);
	float aspect = 1080.0/1920.0; 
	float a1 = 45*3.142/180.0;
	float a2 = aspect/tan(a1);
	float adjust = 1/tan(a1);
	//vec3 coord = vec3(getVecCoord(cube));
	coord.x = coord.x/(highpos.y*20*10) - 0.25*(highpos.x/highpos.y);
	coord.y = coord.y/(highpos.y*20*10) - 0.25;
	coord.z = coord.z/(highpos.y*20*10) - 0.25*highpos.z/highpos.y;
	//coord = coord + vec3(0.25,0.25,0);
	//coord = coord+transform;
	//coord = coord*rotate;
	vec3 Ecoord = coord+vec3(1,1,0)/(highpos.x*20*10);
	Ecoord.x = Ecoord.x/(tan(a1)*coord.z);
	Ecoord.y = Ecoord.y/(tan(a2)*coord.z);
	coord = vec4(projection*vec4(coord,0.0)).xyz;
	coord.z = coord.z + 0.25*highpos.z/highpos.y + adjust;
	coord.x = coord.x/(tan(a1)*coord.z) + 0.5*(highpos.x/highpos.y);
	coord.y = coord.y/(tan(a1)*coord.z) + 0.5*aspect;
	ivec2 pixloc;
	ivec2 Epixloc;
	//floatd[1].xyz = projection[0].xyz;
	//floatd[2].xyz = projection[1].xyz;
	//floatd[3].xyz = projection[2].xyz;

	if(inBox(coord)==true){
		pixloc.x = int(1920*coord.x);
		pixloc.y = int(1920*coord.y);
		Epixloc.x = int(1920*Ecoord.x);
		Epixloc.y = int(1920*Ecoord.y);
		int xd = 2*(Epixloc.x - pixloc.x);
		int yd =2*(Epixloc.y-pixloc.y);
		//if(finalLattice[gl_WorkGroupID.x*27].z > 500){
		int x = pixloc.x;
		int y = pixloc.y;
		//for(int x = Epixloc.x; x < Epixloc.x+xd; x++){
			float colour =  vec3(get_lattice(int(gl_WorkGroupID.x),0).xyz).z/(1000)*250;
			if(float(get_lattice(int(gl_WorkGroupID.x),0).z) >=0){
			atomicAdd(intd[1].x,1);
			floatd[0].x = 100;
			floatd[intd[1].x].x = 100;
			floatd[intd[1].x].z = float(get_lattice(int(gl_WorkGroupID.x),0).z);
			floatd[intd[1].x].y =  int(colour);
			}
			//for(int y = Epixloc.y; y < Epixloc.y + yd+10; y++){
			if(x < 1920 && y < 1080 && x >0 && y>0){
				if(imageLoad(uSampler,ivec2(x,y)).w  == 0 || imageLoad(uSampler,ivec2(x,y)).w > int((coord.z-adjust)*(highpos.y*20*10))){
					imageStore(uSampler,ivec2(x,y),ivec4(int(colour),0,0,int((coord.z-adjust)*(highpos.y*20*10))));
					ivec3 coord2 = ivec3(get_lattice(int(gl_WorkGroupID.x),0).xyz);
					if(coord2.x%100 == 0 || coord2.y%100 == 0){
						//imageStore(uSampler,ivec2(x,y),ivec4(250,0,0,int((coord.z-adjust)*(highpos.y*20*10))));
					}
					if(coord2.z%10 == 0 || coord2.y%10 == 0){
						//imageStore(uSampler,ivec2(x,y),ivec4(0,0,250,int((coord.z-adjust)*(highpos.y*20*10))));
					}
				}
			}
			//}
		//}
	}
}