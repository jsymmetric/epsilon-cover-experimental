
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
vec3 corner[8];
int adj[12][6];
int triiter = 0;
uniform uint u_WorkGroupOffset;
ivec4 getGranular(int location){
	if(location < 3*1000*1000){
		return granular1[location];
	}
	if(location > 3*1000*1000 && location < 3*1000*1000*2){
		return granular2[location - 3*1000*1000];
	}
	return ivec4(0,0,0,0);
}
int getTopSubLevel(int location){
	if(location < 3*1000*1000*5){
		return toplevel[location];
	}
	if(location > 3*1000*1000*5 && location < 3*1000*1000*2*5){
		return toplevel1[location - 3*1000*1000*5];
	}
	return 0;
}
int get_line(int x, int y,int count,vec4 intersection[10]){
	int ret = -1;
	int counter = 0;
	for(int i = 0; i < count; i++){
		for(int e = 0; e <6; e++){
			if(intersection[i].w == adj[x][e] && intersection[i].w != y){
				ret = i;
				counter++;
			}
			if(counter ==1){
				i = 13;
			}
		}
	}
	return ret;
}

void set_lattice(int pos, int adj, ivec4 add){
//pos = pos + 2000*1000;
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
//pos = pos + 800*1000;
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
int get_trilist1(int x, int y, int z, int w){
	return tripos1[x*highpos.z*highpos.y*5000 + y*highpos.z*5000 + z*5000 + w];
};
int get_trilist2(int x, int y, int z, int w){
	return tripos2[x*100*5000 + y*10*5000 + z*5000 + w];
};
int get_trilist3(int x, int y, int z, int w){
	return tripos3[x*100*5000 + y*10*5000 + z*5000 + w];
};
void set_trilist1(int x, int y, int z, int add){
	tripos1[x*highpos.z*highpos.y*5000 + y*highpos.z*5000 + z*5000 + triiter] = add;
};
void set_trilist2(int x, int y, int z, int add){
	tripos2[x*10*10*5000 + y*10*5000+ z*5000 + triiter] = add;
};
void set_trilist3(int x, int y, int z, int add){
	tripos3[x*100*5000 + y*10*5000 + z*5000 + triiter] = add;
};
void set_cubelist3(int x,int y, int z){
	int pos = atomicAdd(tracker[2],1);
	cubepos3[pos] = ivec3(x,y,z);
};
void set_cubelist2(int x,int y, int z){
	int pos = atomicAdd(tracker[1],1);
	cubepos2[pos] = ivec3(x,y,z);
};
void set_cubelist1(int x,int y, int z){
	int pos = atomicAdd(tracker[0],1);
	cubepos1[pos] = ivec3(x,y,z);
};
ivec3 get_cubelist3(int x){
	return cubepos3[x];
};
ivec3 get_cubelist2(int x){
	return cubepos2[x];
};
ivec3 get_cubelist1(int x){
	return cubepos1[x];
};
ivec3[2] get_borders(ivec3 coords){
	ivec3 low = ivec3(0,0,0);
	ivec3 high = ivec3(0,0,0);
	if(coords.z%10 == 0){
		low.z = -1;
	}
	if(coords.y%100 ==0){
		low.y = -1;
	}
	if(coords.x%1000 == 0){
		low.x = -1;
	}
	if(coords.z%10000 == 0){
		high.z = -1;
	}
	if(coords.y%100000 == 0){
		high.y = -1;
	}
	if(coords.x%1000000 == 0){
		high.x = -1;
	}

	if((coords.z+1)%10 == 0){
		low.z = 1;
	}
	if((coords.y+1)%100 == 0){
		low.y = 1;
	}
	if((coords.x+1)%1000 == 0){
		low.x = 1;
	}
	if((coords.z+1)%1000*10 == 0){
		high.z = 1;
	}
	if((coords.y+1)%1000*10*10 == 0){
		high.y = 1;
	}
	if((coords.x+1)%1000*10*10*10 == 0){
		high.x = 1;
	}
	ivec3 boundaries[2];
	boundaries[0] = low;
	boundaries[1] = high;
	return boundaries;
}
int[5] getTopLevel(ivec3 coords){
	int[5] ret;
	coords = (coords-coords%10)/10;
	int place = coords.x*highpos.z*highpos.y*100*5 + coords.y*highpos.z*10*5 + coords.z*5;
	for(int i = 0; i < 5;i++){
		ret[i] = getTopSubLevel(place+i);
	}
	return ret;
}
ivec3[26] adjacent(ivec3 coord, ivec3 coord2){
	ivec3 adjacent[26];
	for(int i = 0; i < 26;i++){
		adjacent[i] = ivec3(-1,-1,-1);
	}
	int meta[] = getTopLevel(coord2);
	int rangeStart = meta[0];
	int rangeEnd = meta[1];
	/*if(coord.x == coord2.x && coord.y == coord2.y && coord.z == coord2.z){
		int trac = atomicAdd(tracker[6],1) + 11;
		intd[10+trac] = ivec4(rangeStart,rangeEnd,10,100);
	}*/
	int p = 0;
	for(int i = rangeStart; i <= rangeEnd; i++){ 
		ivec3 Ccoord = getGranular(i).xyz;
		ivec3 check = coord- Ccoord;
		if(check.x <=1 && check.y <= 1 && check.z <= 1){
			if(check.x >= -1 && check.y >= -1 && check.z >= -1){
				adjacent[p] = getGranular(i).xyz;
			}
		}
	}
	return adjacent;
}
int getAdjType(ivec3 coord1,ivec3 coord2){
	ivec3 dif = coord1 - coord2;
	return abs(dif.x)+abs(dif.y) +abs(dif.z);
}

bool adjacent2(ivec3 adjacent[26], ivec3 coord, int ref){
	int p = 0;
	bool rt = false;
	for(int i = 0; i < 26; i++){
		ivec3 candidate = adjacent[i];
		if(candidate.x != -1){
			ivec3 Ccoord = candidate;
			ivec3 check = coord- Ccoord;
			if(check.x <=1 && check.y <= 1 && check.z <= 1){
				if(check.x >= -1 && check.y >= -1 && check.z >= -1){
					int ty = getAdjType(Ccoord, coord);
					if(ty <= ref && ty > 0){
						return true;
					}
				}
			}
		}
	}
	return false;
}


ivec3 [7] getThree(ivec3 coords,ivec3 offset){
	int i = 0;
	ivec3 ret[7];
	ret[0] = ivec3(-1,-1,-1);
	ret[1] = ivec3(-1,-1,-1);
	ret[2] = ivec3(-1,-1,-1);
	ret[3] = ivec3(-1,-1,-1);
	ret[4] = ivec3(-1,-1,-1);
	ret[5] = ivec3(-1,-1,-1);
	ret[6] = ivec3(-1,-1,-1);
	ivec3 centre = coords + offset;
	ivec3 newcoord;
	newcoord = centre;
	if(newcoord.x != coords.x && newcoord.y != coords.y && newcoord.z != coords.z){
		ret[0] = ivec3(newcoord.x, newcoord.y, newcoord.z);
	}
	//check the numbers here later
	newcoord = centre + ivec3(-1,0,0);
	if(newcoord.x != coords.x && newcoord.y != coords.y && newcoord.z != coords.z){
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;
	}
	newcoord = centre + ivec3(-1,0,-1);
	if(newcoord.x != coords.x && newcoord.y != coords.y && newcoord.z != coords.z){
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;
	}
	newcoord = centre + ivec3(0,0,-1);
	if(newcoord.x != coords.x && newcoord.y != coords.y && newcoord.z != coords.z){
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;
	}
	newcoord = centre + ivec3(-1,-1,0);
	if(newcoord.x != coords.x && newcoord.y != coords.y && newcoord.z != coords.z){
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;	
	}
	newcoord = centre + ivec3(0,-1,-1);
	if(newcoord.x != coords.x && newcoord.y != coords.y && newcoord.z != coords.z){
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;
	}
	newcoord = centre + ivec3(0,-1,0);
	if(newcoord.x != coords.x && newcoord.y != coords.y && newcoord.z != coords.z){
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;
	}
	newcoord = centre + ivec3(-1,-1,-1);
	if(newcoord.x != coords.x && newcoord.y != coords.y && newcoord.z != coords.z){
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
	}
	return ret;
}
ivec3[3] getTwo(ivec3 coords,ivec3 offset){
	int i = 0;
	ivec3 ret[3];
	ret[0] = ivec3(-1,-1,-1);
	ret[1] = ivec3(-1,-1,-1);
	ret[2] = ivec3(-1,-1,-1);
	ivec3 centre = coords + offset;
	ivec3 newcoord;
	newcoord = centre;
	ret[0] = ivec3(newcoord.x, newcoord.y, newcoord.z);
	i++;
	if(offset.x != 0){
		newcoord.x = newcoord.x + offset.x;
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;
	}
	if(offset.y != 0){
		newcoord.y = newcoord.y + offset.y;
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;
	}
	if(offset.z != 0){
		newcoord.z = newcoord.z + offset.z;
		ret[i] = ivec3(newcoord.x, newcoord.y, newcoord.z);
		i++;
	}
	return ret;
}
ivec3 getOne(ivec3 coords, ivec3 offset){
	ivec3 ret = ivec3(-1,-1,-1);
	ivec3 centre = coords + offset;
	ivec3 newcoord;
	newcoord = centre;
	ret = ivec3((newcoord.x + offset.x),(newcoord.y+offset.y),(newcoord.z+offset.z));
	return ret;
}
int checkBorder(ivec3 toCheck){
	return abs(toCheck.x)+ abs(toCheck.y) +abs(toCheck.z);
}
void main(){
	uint TrueWorkGroupID_X = gl_WorkGroupID.x + u_WorkGroupOffset;

	ivec3 storeadj [8][26];
	ivec3 adjcnt[26];
	for(int uu = 0; uu < 26; uu++){
		for(int yy = 0; yy < 8; yy++){
			storeadj[yy][uu] = ivec3(-1,-1,-1);
		}
		adjcnt[uu] = ivec3(-1,-1,-1);
	}
	ivec3 coord = getGranular(int(TrueWorkGroupID_X)).xyz;
	ivec3 boundaries[] = get_borders(coord);
	int lowt = checkBorder(boundaries[0]);
	int hight = checkBorder(boundaries[1]);
	int ee = 0;
	
	if(lowt == 3){
		ivec3 hadjacent[7] = getThree(coord,boundaries[0]);
		ee = 0;
		for(int i = 0; i < 7; i++){
			storeadj[i] = adjacent(coord,hadjacent[i]);
			int e= 0;
			while(storeadj[i][e].x != -1 && e < 26){
				adjcnt[ee] = storeadj[i][e];
				ee++;
				e++;
			}
		}
	}
	if(lowt == 2){
		ivec3 hadjacent[3] = getTwo(coord,boundaries[0]);
		ee = 0;
		for(int i = 0; i < 3; i++){
			storeadj[i] = adjacent(coord,hadjacent[i]);
			int e= 0;
			while(storeadj[i][e].x != -1 && e < 26){
				adjcnt[ee] = storeadj[i][e];
				ee++;
				e++;
			}
		}
	}
	if(lowt == 1){
		ivec3 hadjacent = getOne(coord,boundaries[0]);
		ee = 0;
		storeadj[0] = adjacent(coord,hadjacent);
		int e= 0;
		while(storeadj[0][e].x != -1){
			adjcnt[ee] = storeadj[0][e];
			ee++;
			e++;
		}
	}		
	adjcnt[7] = adjacent(coord,coord)[0];
	if(adjcnt[0].z >= 0){
			intd[10] = ivec4(0,0,100,120);
			int trac = atomicAdd(tracker[6],1) + 11;
			intd[trac] = ivec4(adjcnt[7].xyz,10);
	}
	int e= 0;
	while(storeadj[7][e].x != -1){
		adjcnt[ee] = storeadj[7][e];
		ee++;
		e++;
	}	


	for(int i = 0; i < ee; i++){
		if(adjacent2(adjcnt,adjcnt[i],getAdjType(adjcnt[i],coord))){
			adjcnt[i].x = -1;
		}
	}
	int iter = 1;
	if(coord.z < 2000 && coord.x < 2000 && coord.y < 2000 && coord.x >= 0 && coord.z >= 0 && coord.y >= 0){
	int latticenum = atomicAdd(latticeInc,1);
	//if(latticenum > 5.6e6){
		int inc = atomicAdd(intd[0].x,1);
		//atomicAdd(intd[1].x,1);
		//floatd[0].x = float(intd[1].x); 
	set_lattice(latticenum,0,ivec4(getGranular(int(TrueWorkGroupID_X)).xyz,100));
	for(int i = 0; i < 12;i++){
		if(adjcnt[i].x != -1){
			set_lattice(latticenum,iter,ivec4(adjcnt[i],10));
			iter++;
		}
	}
	/*
		int tracloc = atomicAdd(tracker[7],3);
		if(finalLattice[1000*1000*20].z > 0){
			intd[10] = finalLattice[latti];
			atomicMax(intd[10].w,int(TrueWorkGroupID_X));
			intd[11].x = latticenum/27;
		}*/
//intd[20].xyz = ivec3(latticenum,1000,1000);
}}