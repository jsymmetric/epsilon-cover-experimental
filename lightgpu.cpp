// lightgpu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// gputest1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <Windows.h>
#include <winbase.h>
#include <sys/stat.h>
#include <windows.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <random>
#include "storage.cpp"
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <iostream>
#include <array>
#include <cmath>
#include <numbers>
struct Vector3D {
	float x, y, z;

	Vector3D() : x(0), y(0), z(0) {}
	Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Triangle {
	//Vector3D normal;
	Vector3D vertices[3];

	Triangle() = default;
	Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3) {
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;
		// Calculate normal (optional - STL files typically include normals)
		//calculateNormal();
	}
};

class STLLoader {
public:
	// Load STL file (both ASCII and binary formats)
	static bool loadSTL(const std::string& filename, std::vector<Triangle>& triangles) {
		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "Error: Could not open file " << filename << std::endl;
			return false;
		}

		// Check if file is ASCII or binary
		if (isASCIISTL(file)) {
			return loadASCIISTL(filename, triangles);
		}
		else {
			return loadBinarySTL(file, triangles);
		}
	}

private:
	// Check if STL file is in ASCII format
	static bool isASCIISTL(std::ifstream& file) {
		file.seekg(0);
		std::string line;
		std::getline(file, line);

		// Check if first line contains "solid"
		if (line.find("solid") != std::string::npos) {
			// Additional check: look for "facet" in the next few lines
			std::streampos currentPos = file.tellg();
			for (int i = 0; i < 5; ++i) {
				std::getline(file, line);
				if (line.find("facet") != std::string::npos) {
					file.seekg(currentPos);
					return true;
				}
			}
		}
		file.seekg(0);
		return false;
	}

	// Load ASCII STL format
	static bool loadASCIISTL(const std::string& filename, std::vector<Triangle>& triangles) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			return false;
		}

		std::string line;
		Triangle currentTriangle;
		int vertexCount = 0;

		while (std::getline(file, line)) {
			// Trim whitespace
			line.erase(0, line.find_first_not_of(" \t\r\n"));
			line.erase(line.find_last_not_of(" \t\r\n") + 1);

			if (line.find("facet normal") == 0) {
				// Read normal
				sscanf_s(line.c_str(), "facet normal %f %f %f");
				vertexCount = 0;
			}
			else if (line.find("vertex") == 0) {
				// Read vertex
				Vector3D vertex;
				sscanf_s(line.c_str(), "vertex %f %f %f", &vertex.x, &vertex.y, &vertex.z);
				currentTriangle.vertices[vertexCount++] = vertex;

				if (vertexCount == 3) {
					triangles.push_back(currentTriangle);
				}
			}
			else if (line.find("endfacet") == 0) {
				// Triangle complete
				if (vertexCount == 3) {
					triangles.push_back(currentTriangle);
				}
			}
		}

		return !triangles.empty();
	}

	// Load binary STL format
	static bool loadBinarySTL(std::ifstream& file, std::vector<Triangle>& triangles) {
		file.seekg(0, std::ios::end);
		std::streamsize fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// Skip header (80 bytes)
		file.seekg(80, std::ios::cur);

		// Read number of triangles
		uint32_t numTriangles;
		file.read(reinterpret_cast<char*>(&numTriangles), sizeof(numTriangles));

		if (fileSize < 84 + numTriangles * 50) {
			std::cerr << "Error: File appears to be truncated" << std::endl;
			return false;
		}

		triangles.resize(numTriangles);

		for (uint32_t i = 0; i < numTriangles; ++i) {
			Triangle triangle;
			// Read normal
			char normalx = 'r';
			char normaly = 'r';
			char normalz = 'r';
			file.read(reinterpret_cast<char*>(&normalx), sizeof(float));
			file.read(reinterpret_cast<char*>(&normaly), sizeof(float));
			file.read(reinterpret_cast<char*>(&normalz), sizeof(float));
			// Read vertices

			for (int j = 0; j < 3; ++j) {
				file.read(reinterpret_cast<char*>(&triangle.vertices[j].x), sizeof(float));
				file.read(reinterpret_cast<char*>(&triangle.vertices[j].y), sizeof(float));
				file.read(reinterpret_cast<char*>(&triangle.vertices[j].z), sizeof(float));
			}

			// Skip attribute byte count (2 bytes)
			file.seekg(2, std::ios::cur);

			triangles[i] = triangle;
		}
		return true;






	}
};


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
float angleadjust = 0;
float fb = 0;
float xpos = 0;
float ypos = 20;
float zpos = -3;
// function to create text from glerror code
std::vector<float> genTranslations() {
	float angle_degrees = 45.0f;
	// Convert degrees to radians for std::tan
	float angle_radians = angle_degrees * (3.1415 / 180.0f);

	float factor = std::tan(angle_radians / 2.0f);
	float aspect = 1080.0f / 1960.0f;
	// float zn = 1960.0f / std::tan(angle_radians); // zn is not used in the return array

	// Use aggregate initialization for the vector
	std::vector<float> projection = {
		1.0f/(aspect*factor), 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f/(aspect * factor), 0.0f,0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};

	return projection; // The data is safely copied/moved
}
int* genTrilist() {
	int* list1 = new int[1000 * 5000];
	for (int i = 0; i < 1000*5000; i++) {
		list1[i] = 0;
	}
	return list1;
}
float* genFloat() {
	float* list10 = new float[1000 * 1000*4];
	for (int i = 0; i < 1000 * 1000*4; i++) {
		list10[i] = 0.0;
	}
	return list10;
}
int* genIntegers() {
	int* list11 = new int[1000 * 1000 * 4];
	for (int i = 0; i < 1000 * 1000 * 4; i++) {
		list11[i] = 0;
	}
	return list11;
}
int* genCubelist() {
	int* list2 = new int[1000 * 4];
	for (int i = 0; i < 1000*4; i++) {
		list2[i] = 0;
		if (i % 2 == 0) {
			list2[i] = 0;
		}
	}
	return list2;
}
float* genPixel() {
	float* list3 = new float[1920 * 1080+2];
	list3[0] = 1920.0;
	list3[1] = 1080.0;
	for (int i = 2; i < 1920 * 1080 +2; i++) {
		list3[i] = -1;
	}
	return list3;
}
int* genPixel2() {
	int* list3 = new int[1920 * 1080*4 ];
	for (int i = 0; i < 1920 * 1080*4; i++) {
		list3[i] = 0;
	}
	return list3;
}
int* genLinked(int x, int y, int z) {
	int* list4 = new int[7 + 1000 * 1000 * 5 + 10 * 1000 * 1000*4];
	list4[0] = x;
	list4[1] = y;
	list4[2] = z;
	cout << "highpos " << x << " " << y << " " << z << "\n";
	for (int i = 3; i < 7 + 1000 * 1000 * 5 + 10 * 1000 * 1000*4; i++) {
		list4[i] = 0;
	}
	return list4;
}

float* rotation(float scale) {
	float* list5 = new float[10 * 1000 * 9 + 3];
	list5[0] = scale;
	list5[1] = scale;
	list5[2] = scale;

	for (int i = 3; i < 10 * 1000 * 9 + 3; i++) {
		list5[i] = 0;
	}
	return list5;
}
int* genLattice() {
	int* list6 = new int[200 * 1000 * 27*4 + 1];
	for (int i = 0; i < 200 * 1000*27*4+1; i++) {
		list6[i] = 0;
	}
	return list6;
}
int* genLattice2() {
	int* list6 = new int[200 * 1000 * 27 * 4];
	for (int i = 0; i < 200 * 1000 * 27 * 4; i++) {
		list6[i] = 0;
	}
	return list6;
}
void glerrorstring(GLenum error) {
	switch (error) {
	case GL_NO_ERROR:
		cout << "no error"<< "\n";
		break;
	case GL_INVALID_ENUM:
		cout << "invalid enum"<< "\n";
		break;
	case GL_INVALID_VALUE:
		cout << "invalid value"<< "\n";
		break;
	case GL_INVALID_OPERATION:
		cout << "invalid operation"<< "\n";
		break;
	case GL_STACK_OVERFLOW:
		cout << "stack overflow"<< "\n";
		break;
	case GL_STACK_UNDERFLOW:
		cout << "stack underflow"<< "\n";
		break;
	case GL_OUT_OF_MEMORY:
		cout << "out of memory"  <<"\n";
		break;
	};
};
//function to take orientation and movement input from user
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		angleadjust += 0.1;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		angleadjust -= 0.1;
	}
	if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
		fb -= 0.3;
	}
	if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
		fb += 0.3;
	}
}

//function to retrieve data from opengl buffers mostly used for debug
store<int> readbuffer(void* point, int length) {
	GLuint* pnt = (GLuint*)point;
	store<int> buf = store<int>(1, length);
	for (int i = 0; i < length; i++) {
		buf.add(i, int(*(pnt + i)));

	}
	return buf;

}
store<float> readbufferF(void* point, int length) {
	GLfloat* pnt = (GLfloat*)point;
	store<GLfloat> buf = store<GLfloat>(1, length);
	for (int i = 0; i < length; i++) {
		buf.add(i, GLfloat(*(pnt + i)));

	}
	return buf;

}
void writebuffer(void* point, int length,int add) {
	GLuint* pnt = (GLuint*)point;
    *(pnt + length)  = add;

}
float* testTriangles() {
	float ret[]={
			0, 0, 0,
			0, 1, 0,
			0, 1, 1,
			0, 1, 1,
			1, 1, 1,
			1, 0, 1
		};
	return ret;
}
int main()
{
	//std::vector<Triangle> triangles;
	//STLLoader::loadSTL("E:\Sample_Hull_Mesh.stl", triangles);
	// standard opengl initiation code follows
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	GLFWwindow* window;
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHintString(GLFW_MAXIMIZED, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1920, 1080, "Render system prototype v1(voxel)", NULL, NULL);
	if (window == NULL) {
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	int version = gladLoadGL(glfwGetProcAddress);

	if (version == 0) {
		printf("Failed to initialize OpenGL context\n");
		return -1;
	}
	else {
		cout << version << "\n";
	}
	//glad is used not glew
	//glewExperimental = true;
	//if (glewInit() != GLEW_OK) {
		//fprintf(stderr, "Failed to initialize GLEW\n");
		//getchar();
		//glfwTerminate();
		//return -1;
	//}
	//set input mode for user control
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);


	// this code uses a pair of triangles with the texel array output to render to the screen the buffer generated below are used for the triangle vertices
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);



	GLuint vertexbuffer;

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// buff data;
	//store<GLint> texels = initLatticeState(size)-

	//initialize texel data and generate texture binding

	//glGenTextures(1, &texadd);

	//cout << texadd;
	glerrorstring(glGetError());
	float triangles[] = {
		0, 0, 0,0,
		0, 1, 0,0,
		0, 1, 1,0,
		0, 1, 1,0,
		1, 1, 1,0,
		1, 0, 1,0
	};
	/*float triangles[] = {
		0, 1.0, 2.0,0,
		3.0, 4.0, 5.0,0,
		6.0, 7.0, 8.0,0,
		9.0, 10.0, 11.0,0,
		12.0, 13.0, 14.0,0,
		15.0, 16.0, 17.0,0
	};*/
	float upperx = -10000;
	float uppery = -10000;
	float upperz = -10000;
	float lowerx = 10000;
	float lowery = 10000;
	float lowerz = 10000;
	//float triangles[] = testTriangles();
	/*
	for (int i = 0; i < triangles.size(); i++) {
		for (int e = 0; e < 3; e++) {
			if (triangles[i].vertices[e].x > upperx) {
				upperx = triangles[i].vertices[e].x;
			}
			if (triangles[i].vertices[e].y > uppery) {
				uppery = triangles[i].vertices[e].y;
			}
			if (triangles[i].vertices[e].z > upperz) {
				upperz = triangles[i].vertices[e].z;
			}


			if (triangles[i].vertices[e].x < lowerx) {
				lowerx = triangles[i].vertices[e].x;
			}
			if (triangles[i].vertices[e].y < lowery) {
				lowery = triangles[i].vertices[e].y;
			}
			if (triangles[i].vertices[e].z < lowerz) {
				lowerz = triangles[i].vertices[e].z;
			}
		}

	}*/
	lowerz = -0.1;
	lowery = -0.1;
	lowerx = -0.1;
	upperx = 1.1;
	uppery = 1.1;
	upperz = 1.1;

	float deltax = upperx - lowerx;
	float deltay = uppery - lowery;
	float deltaz = upperz - lowerz;
	float minimum = deltaz;
	cout << deltax;
	cout << " ";
	cout << deltay;
	cout << " ";
	cout << upperz;

	if (deltax < minimum) {
		minimum = deltax;
	}
	if (deltay <= minimum) {
		minimum = deltay;
	}

	int cubebase = (int)std::cbrt(1000 / (deltax * deltay * deltaz / (minimum * minimum * minimum)));
	double cubebase2 = std::cbrt(1000 / (deltax * deltay * deltaz / (minimum * minimum * minimum)));
	int x = cubebase * deltax/minimum;
	int y = cubebase * deltay/minimum;
	int z = cubebase * deltaz/minimum;
	cout << "\n" << x << " " << y << " " << z << "\n";
	float scale = minimum / cubebase;
	cout << scale << " scale \n";
	GLuint shaderdata[19];
	glGenBuffers(19, shaderdata);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[0]);
	//rotation
	glBufferData(GL_SHADER_STORAGE_BUFFER, (10*1000*9+3)*sizeof(float), rotation(scale), GL_STATIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, shaderdata[0], 0, (3+10*1000*9)*sizeof(GLfloat));
	//bind triangles;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 100*sizeof(GLfloat), &triangles[0], GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, shaderdata[1], 0, (100*1000*3) * sizeof(GLfloat));
	//bind trilist1
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[2]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * 5000 * sizeof(int), genTrilist(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, shaderdata[2], 0, 1000 * 5000 * sizeof(int));
	cout << "her3";
	//bind trilist2
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[3]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * 5000 * sizeof(GLint), genTrilist(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, shaderdata[3], 0, 1000 * 5000 * sizeof(GLint));
	//bind trilist3
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[4]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * 5000 * sizeof(GLint), genTrilist(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4, shaderdata[4], 0, 1000 * 5000 * sizeof(GLint));
	//bind cubelist
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[5]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * 3 * sizeof(GLint), genCubelist(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 5, shaderdata[5], 0, 1000 * 3 * sizeof(GLint));
	//bind cubelist1
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[6]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * 3 * sizeof(GLint), genCubelist(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 6, shaderdata[6], 0, 1000 * 3 * sizeof(GLint));
	//bind cubelist2
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[7]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * 3 * sizeof(GLint), genCubelist(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 7, shaderdata[7], 0, 1000 * 3 * sizeof(GLint));
	//bind tracking value
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[8]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 10 * sizeof(GLint), NULL, GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 8, shaderdata[8], 0, 10* sizeof(GLint));
	//bind final cubes
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[9]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (6+1000*1000*5 + 10*1000*1000)* sizeof(GLint), genLinked(x,y,z), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 9, shaderdata[9], 0, (6 + 1000 * 1000 * 5 + 10 * 1000 * 1000) * sizeof(GLint));
	//bind pixel buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[10]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (2 + 1920 * 1080) * sizeof(GLfloat), genPixel(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 10, shaderdata[10], 0, (2+1920*1080) * sizeof(GLfloat));


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[11]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (1000*1000*4) * sizeof(GLfloat), genFloat(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 11, shaderdata[11], 0, (1000*4*1000) * sizeof(GLfloat));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[13]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (1000 * 1000*4) * sizeof(GLint), genIntegers(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 13, shaderdata[13], 0, (1000 * 4 * 1000) * sizeof(GLint));

	//bind lattice
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[14]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (1 + 4*(200 * 1000 * 27)) * sizeof(GLint), genLattice(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 14, shaderdata[14], 0, (1 + 4*(200 * 1000 * 27)) * sizeof(GLint));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[15]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (4*(200 * 1000 * 27)) * sizeof(GLint), genLattice2(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 15, shaderdata[15], 0, ( 4*(200 * 1000 * 27)) * sizeof(GLint));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[16]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (4*(200 * 1000 * 27)) * sizeof(GLint), genLattice2(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 16, shaderdata[16], 0, (4*(200 * 1000 * 27)) * sizeof(GLint));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[17]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (4*(200 * 1000 * 27)) * sizeof(GLint), genLattice2(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 17, shaderdata[17], 0, (4*(200 * 1000 * 27)) * sizeof(GLint));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[18]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (4*(200 * 1000 * 27)) * sizeof(GLint), genLattice2(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 18, shaderdata[18], 0, (4*(200 * 1000 * 27)) * sizeof(GLint));
	// the two normal shaders can be compiled and linked together whilst the 3 compute shaders must be comiled and linked separately 
	GLuint programID = compile_shader();
	GLuint compstage1 = createcompshader(1);
	GLuint compstage2 = createcompshader(2);
	GLuint compstage3 = createcompshader(3);
	GLuint compstage4 = createcompshader(4);
	GLuint compstage5 = createcompshader(5);
	GLuint compstage6 = createcompshader(6);

	//two shaders for shadows the first generates a shadow map once and the second applies it
	glLinkProgram(programID);
	glLinkProgram(compstage1);
	glLinkProgram(compstage2);
	glLinkProgram(compstage3);
	glLinkProgram(compstage4);
	glLinkProgram(compstage5);
	glLinkProgram(compstage6);
	int facenum = 2;// triangles.size();
	void* bufmap3 = glMapNamedBuffer(shaderdata[1], GL_READ_ONLY);
	int bracket = 0 * 1;
	store<float> bu3f = readbufferF(bufmap3, 1000 * 5000);
	for (int u = bracket; u < bracket+4*6; u += 4) {
		//if (bu2.get(u) > 0) {
	//	if (u % 9 == 0) {
			//cout << "\n\n";
		//}
		cout << " check " << bu3f.get(u) << "," << bu3f.get(u + 1) << "," << bu3f.get(u + 2) << "   " << u << "\n";
		//}

	}
	cout << "facenum = " << facenum << "\n";
	glUnmapNamedBuffer(shaderdata[1]);
	glUseProgram(compstage1);
	glDispatchCompute(facenum, 1, 1);

	cout << "\n";
	//printMap(bu);
	int round1 = 0;
	void* bufmap6 = glMapNamedBuffer(shaderdata[1], GL_READ_ONLY);
	store<float> bu2f = readbufferF(bufmap6, 1000 * 5000);
	for (int u = bracket; u < bracket + 4*6; u +=4) {
		//if (u % 9 == 0){
			//cout << "\n\n";
		//}
		//if (bu2.get(u) > 0) {
		cout << " cube1 " << bu2f.get(u) << "," << bu2f.get(u + 1) << "," << bu2f.get(u + 2)  << "  " << u << "\n";
		//}
	}
	glUnmapNamedBuffer(shaderdata[1]);
	glUseProgram(compstage2);
	cout << x << " x \n";
	glDispatchCompute(x, y, z);
	void* bufmap8 = glMapNamedBuffer(shaderdata[8], GL_READ_ONLY);
	store<int> bu = readbuffer(bufmap8, 10);
	glUnmapNamedBuffer(shaderdata[8]);
	int length1 = bu.get(0);
	cout << "length1 " << length1 << "\n";
	cout << x << " "<< y << " " << z << "\n";
	glerrorstring(glGetError());
	/*
	void* bufmap5 = glMapNamedBuffer(shaderdata[2], GL_READ_ONLY);
	store<int> bu3 = readbuffer(bufmap5, 1000 * 5000);
	for (int r = 0; r < x; r++) {
		for (int t = 0; t < y; t++) {
			for (int g = 0; g < z; g++) {
				int pos = bu3.get(r * z * y * 5000 + t * z * 5000 + g * 5000+3484);
				if (bu3.get(r * z * y * 5000 + t * z * 5000 + g * 5000 +3480) >= 1){
					cout << r << " " << t << " " << g << " here " << pos << "\n";
				}
				else {
					cout << "no " << r << " " << t << " " << g << " " << pos << "\n";
				}
				for (int o = 0; o < 5000; o++) {
					if (bu3.get(r * z * y * 5000 + t * z * 5000 + g * 5000 + 3480 + o)>0) {
						cout << r << " " << t << " " << g << " here " << o << " value->";
						cout << bu3.get(r * z * y * 5000 + t* z * 5000 + g * 5000+3480 + o) << " \n" ;
					}
				}
			}

		}

	}
	
	glUnmapNamedBuffer(shaderdata[2]);
	*/
	int gplace = 0;
	
	void* bufmap15 = glMapNamedBuffer(shaderdata[5], GL_READ_ONLY);
	store<int> bu67 = readbuffer(bufmap15, 3000 * 2);
	for (int u = 0; u < 2000; u += 4) {
		if (bu67.get(u) > 0) {
			//cout << bu3.get(u) << " " << bu3.get(u + 1) << " " << bu3.get(u + 2) << "\n";
		
		}
		if (bu67.get(u) == 6 && bu67.get(u + 1) == 8 && bu67.get(u + 2) == 8) {
			gplace = u;
		}
	}
	
	glUnmapNamedBuffer(shaderdata[5]);
	GLint max_work_group_count[3];

	// Query X dimension (index 0)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_work_group_count[0]);

	// Query Y dimension (index 1)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_work_group_count[1]);

	// Query Z dimension (index 2)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_work_group_count[2]);

	// Output the results
	std::cout << "Max Work Group Count X: " << max_work_group_count[0] << std::endl;
	std::cout << "Max Work Group Count Y: " << max_work_group_count[1] << std::endl;
	std::cout << "Max Work Group Count Z: " << max_work_group_count[2] << std::endl;
	GLint64 maxSSBOSize;
	glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSSBOSize);

	// Output the result in bytes
	printf("Max Shader Storage Block Size: %lld bytes\n", (long long)maxSSBOSize);

	for (int i = 0; i < length1; i++) {
		//compute the second 1000 blocks
		//bind trilist2

		//cout << "\n";
	
		void* bufmap12 = glMapNamedBufferRange(shaderdata[8],0,9, GL_MAP_WRITE_BIT);
		//glerrorstring(glGetError());
		writebuffer(bufmap12,3,i);
		writebuffer(bufmap12, 1,0);
		
		//glFlushMappedNamedBufferRange(shaderdata[8],0, 9);
		glUnmapNamedBuffer(shaderdata[8]);
		void* bufmap5 = glMapNamedBuffer(shaderdata[3], GL_READ_ONLY);
		store<int> bu3 = readbuffer(bufmap5, 1000 * 5000);
		//for (int u = 0; u < 1000; u += 4) {
			//if (bu3.get(u)>= 0) {
				//cout << bu3.get(u) << " " << bu3.get(u + 1) << " " << bu3.get(u + 2) << "\n";
			//}
		//}
		glUnmapNamedBuffer(shaderdata[3]);
	
		//glerrorstring(glGetError());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[6]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * 3 * sizeof(GLint), genCubelist(), GL_DYNAMIC_READ);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 6, shaderdata[6], 0, 1000 * 3 * sizeof(GLint));
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[3]);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * 5000 * sizeof(GLint), genTrilist(), GL_DYNAMIC_READ);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, shaderdata[3], 0, 1000 * 5000 * sizeof(GLint));
		glFlush();
		glUseProgram(compstage3);
		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		//glerrorstring(glGetError());
		//cout << "cycle " << "\n";
		void* bufmap = glMapNamedBuffer(shaderdata[8], GL_READ_ONLY);
		store<int> bu2 = readbuffer(bufmap, 4);
		//cout << bu2.get(1) << "length2 \n";
		int length2 = bu2.get(1);
		glUnmapNamedBuffer(shaderdata[8]);
		/*
		if (i == int(gplace/4)) {
			cout << gplace/4 << " gplace\n";
			void* bufmap5 = glMapNamedBuffer(shaderdata[6], GL_READ_ONLY);
			store<int> bu3 = readbuffer(bufmap5, 1000 * 3);
			int gplace = 0;
			for (int u = 0; u < 500; u += 4) {
				//if (bu3.get(u) > 0) {
					cout << bu3.get(u) << " " << bu3.get(u + 1) << " " << bu3.get(u + 2) << "    " << u/4 << "\n";

				//}
			}

		}
		*/
		if (length2 > 0) {
			//cout << "comp 4 \n";
			//void* bufmap120 = glMapNamedBufferRange(shaderdata[8], 0, 9, GL_MAP_WRITE_BIT);
			//glerrorstring(glGetError());
			//writebuffer(bufmap120, 4, 0);

			//glFlushMappedNamedBufferRange(shaderdata[8],0, 9);
			///glUnmapNamedBuffer(shaderdata[8]);
			glUseProgram(compstage4);
			glDispatchCompute(length2, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			//void* bufmap = glMapNamedBuffer(shaderdata[8], GL_READ_ONLY);
			//store<int> bu2 = readbuffer(bufmap, 4);
			//cout << bu2.get(3) << "gh \n";
		//	glUnmapNamedBuffer(shaderdata[8]);

		}
		

		//cout << "length2 " << length2 << "\n";

	}
	//cout << "here1" << "\n";
	//void* bufmap10 = glMapNamedBuffer(shaderdata[8], GL_READ_ONLY);
	//store<int> bu9 = readbuffer(bufmap10,9);
	//for (int dd = 0; dd < 9; dd++) {
		//cout << "checker \n";
		//if (bu9.get(dd) >= 0) {
		//cout << bu9.get(7) << "end \n";
		//}
	//}
	//glUnmapNamedBuffer(shaderdata[8]);
	glerrorstring(glGetError());
	void* bufmap11 = glMapNamedBuffer(shaderdata[9], GL_READ_ONLY);
	bu = readbuffer(bufmap11, 1000 * 1000);
	//cout << bu.get(7) << " granend \n";
	glUnmapNamedBuffer(shaderdata[9]);
	float cubenum = bu.get(7);

	glUseProgram(compstage5);
	//glDispatchCompute(cubenum, 1, 1);
	GLint u_offset_loc = glGetUniformLocation(compstage5, "u_WorkGroupOffset");
	int Gx_Total = cubenum;
	int CHUNK_SIZE_X = 50000;
	for (GLuint offset_x = 0; offset_x < Gx_Total; offset_x += CHUNK_SIZE_X)
	{
		// Calculate how many work groups are left in the current chunk
		GLuint current_dispatch_x = min(CHUNK_SIZE_X, Gx_Total - offset_x);

		// Pass the starting index of the work group ID to the shader
		glUniform1ui(u_offset_loc, offset_x);

		// Dispatch the smaller chunk
		glDispatchCompute(current_dispatch_x, 1, 1);

		// Crucial: Force ALL prior writes to commit and clear the internal driver state
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
	glFinish();
	GLuint texadd;

	glGenTextures(1, &texadd);
	int count = 0;
	cout << "here1 " << cubenum << "\n";
	glerrorstring(glGetError());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderdata[12]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 * sizeof(GLfloat), genTranslations().data(), GL_DYNAMIC_READ);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 12, shaderdata[12], 0, 16 * sizeof(GLfloat));

	glBindTexture(GL_TEXTURE_2D, texadd);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, 1920, 1080, 0, GL_RGBA_INTEGER, GL_INT, genPixel2());
	GLuint* point = NULL;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindImageTexture(0, texadd, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32I);
	do {
		//cout << "\n";
		//printMap(but);
		//glUnmapNamedBuffer(shaderdata[0]);
		// the non static buffers need to be reinitialzed every frame so are setup here.

		
		/*
		// compute the first 1000 blocks
		void* bufmap12 = glMapNamedBufferRange(shaderdata[8], 0, 9, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
		//glerrorstring(glGetError());
		writebuffer(bufmap12, 7, 0);

		glFlushMappedNamedBufferRange(shaderdata[8], 0, 9);
		glUnmapNamedBuffer(shaderdata[8]);*/

		glUseProgram(compstage6);
		glDispatchCompute(cubenum, 1, 1);
		glFinish();
		//glerrorstring(glGetError());
		//cout << "frame\n";
		//
		/*
		void* bufmap10 = glMapNamedBuffer(shaderdata[12], GL_READ_ONLY);
		store<float> bu9 = readbufferF(bufmap10, 10 * 1000);
		for (int dd = 0; dd < 10 * 10000; dd+=4) {
			cout << "                            " << dd << " checker \n";
			//if (bu9.get(dd) > 0) {
				cout << bu9.get(dd) << " " << bu9.get(dd + 1) << " " << bu9.get(dd + 2) << " " << bu9.get(dd+3) << "end \n";
			//}
		}
		glUnmapNamedBuffer(shaderdata[12]);
		*/
		int checkered[200];
		for (int ch = 0; ch < 200; ch++) {
			checkered[ch] = ch;
		}

		if (count == 0) {
		void* bufmap10 = glMapNamedBuffer(shaderdata[13], GL_READ_ONLY);
		store<int> bu9 = readbuffer(bufmap10, 5000 * 4 + 4);
		int count2 = 0;
		if (count == 0) {
			for (int dd = 0; dd < 2000; dd += 4) {
				//cout << "checker \n";
				//if (bu9.get(dd) >= 0) {
				cout << bu9.get(dd) << " " << bu9.get(dd + 1) << " " << bu9.get(dd + 2) << " " << bu9.get(dd + 3) << "        " << dd / 4 << " end \n";
				if (bu9.get(dd) > 0) {
					count2++;
				}
				//if (dd % 3 == 0) {
					//int chu = bu9.get(dd + 3);
					
					//checkered[chu] = 0;
				//}
				//}
			}
			//for (int gu = 0; gu < 200; gu++) {
				//cout << checkered[gu] << " \n";
			//}
			GLint max_ssbo_size_bytes;
			glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_ssbo_size_bytes);

			// Your buffer size is 432,000,000 bytes (1000*1000*27*4*4)
			cout << "check size " << max_ssbo_size_bytes/1000000 << "\n";
			if (1000*1000*4*4*27 > max_ssbo_size_bytes) {
				cout << "fat //////////////////// \n";
			}
		}

		glUnmapNamedBuffer(shaderdata[13]);
	}
		//bufmap10 = glMapNamedBuffer(shaderdata[8], GL_READ_ONLY);
		//store<int> bu90 = readbuffer(bufmap10, 7);
			//cout << "checker \n";
				//if (bu9.get(dd*27+1) > 0) {;
			//cout << bu90.get(1) << " " << count2 << " /////////////////////////////////////end \n";
			//}
		//glUnmapNamedBuffer(shaderdata[8]);
		/**/
		//glerrorstring(glGetError());
		//cout << "here \n";
		GLint g_vertex_buffer_data[] = { -1,-1,0,   0, 0,   1,-1,0,   1, 0,  -1,1,0,  0, 1,  1,1,0,   1, 1,   1,-1,0,  1, 0,   -1,1,0,   0, 1 };
		
		glUseProgram(programID);

		glClear(GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);
		glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(GLint), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 5 * sizeof(GLint), (void*)0);
		int siz = 3 * sizeof(GLint);
		glVertexAttribPointer(1, 2, GL_INT, GL_FALSE, sizeof(GLint) * 5, (GLvoid*)siz);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		//glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
		glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glfwSwapBuffers(window);
		glfwPollEvents();
		glfwSetKeyCallback(window, key_callback);

		count++;
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &shaderdata[0]);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);
	glfwTerminate();
}