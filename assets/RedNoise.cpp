#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <string>
#include <CanvasPoint.h>
#include <CanvasTriangle.h>
#include <ModelTriangle.h>
#include <TextureMap.h>
#include <RayTriangleIntersection.h>
#include <Colour.h>
#include <glm/glm.hpp>
#include <thread>

#define WIDTH 700
#define HEIGHT 700

#define DEPTH_MAX 5

//rm assets/outputPPM/*
//make speedy
//for i in assets/outputPPM/output*.ppm; do convert "$i" assets/outputPPM/converted${${i%%.*}##*/}.jpeg;done;
//convert -quality 95 assets/outputPPM/converted*.jpeg assets/outputPPM/aMovie.mp4




//for i in assets/outputPPM/*.ppm; do convert "$i" assets/outputPPM/converted${${i%%.*}##*/}.jpeg;done;
//convert -quality 95 assets/outputPPM/converted*.jpeg assets/outputPPM/movie.mp4

//^ converts ppm to jpg for all output images
//then convert to mp4

/*
	KEYBINDINGS:
	RETURN 		-> toggle camera rotating around y axis model with lookAt function
	L 			-> toggle light translating vertically up and down
	1			-> RENDERING MODE 1: Wireframe
	2			-> RENDERING MODE 2: Rasterized
	3			-> RENDERING MODE 3: RayTraced
	4			-> RENDERING MODE 4: Gouraud shader
	5			-> RENDERING MODE 5: Phong shader
	qwertyu -> change rendSettings
*/

bool generating = false; // set to 2 to generate images for the ident in assets/outputPPM

//renderingSettings for Ident example
bool rendSet_Lighting = true; //
bool rendSet_Shadows = true; //
bool rendSet_SoftShadows = true;
bool rendSet_Reflection = true; //
bool rendSet_Refraction = true; //
bool rendSet_Textures = true; //
bool rendSet_BumpMap = true; //

glm::vec3 camPos = glm::vec3(0, 0, 3.0);
glm::mat3 camAng = glm::mat3(1.0);
std::vector<ModelTriangle> model;
std::vector<TextureMap> modelTextures;
std::vector<TextureMap> modelBump;
glm::vec3 lightPos = glm::vec3(0, 0.3, 0);
std::vector<glm::vec3> lightPoints;
float lightSize = 0.05;
float lightDensity = 1;
float focal = 4;

int frames = 0;

int rendering = 1;
int fps = 60;

uint32_t packCol(Colour col){ return (255 << 24) + (int(col.red) << 16) + (int(col.green) << 8) + int(col.blue); }

Colour unpackCol(uint32_t col){
	Colour result;
	result.red = (uint8_t) ((col >> 16) & 0xFF);
	result.green = (uint8_t) ((col >> 8) & 0xFF);
	result.blue = (uint8_t) ((col) & 0xFF);
	return result;
}

Colour dimCol(Colour col, float dimPerc){
	if(dimPerc >= 1) { dimPerc = 1; }
	if(dimPerc <= 0) { dimPerc = 0; }
	return Colour(col.name, (int)(col.red*dimPerc)%256, (int)(col.green*dimPerc)%256, (int)(col.blue*dimPerc)%256);
}

Colour brightCol(Colour col, float brightPerc){
	if(brightPerc >= 1) { brightPerc = 1; }
	if(brightPerc <= 0) { brightPerc = 0; }
	return Colour(col.name, (int)((255-col.red)*brightPerc + col.red)%256, (int)((255-col.green)*brightPerc + col.green)%256, (int)((255-col.blue)*brightPerc + col.blue)%256);
}

glm::vec3 getNormal(ModelTriangle tri){
	glm::vec3 e0 = tri.vertices[1] - tri.vertices[0];
	glm::vec3 e1 = tri.vertices[2] - tri.vertices[0];
	return glm::normalize(glm::cross(e0, e1));
}

void drawCircle(DrawingWindow &window, int cx, int cy, int r, uint32_t colour){
	for(int y = cy-r; y < cy+r; y++){
		for(int x = cx-r; x < cx+r; x++){
			if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
				float dist = sqrt((cy-y)*(cy-y) + (cx-x)*(cx-x));
				if(dist < r){
					window.setPixelColour(x, y, colour);
				}
			}
		}
	}
}

std::vector<glm::vec3> getPointsInSphere(glm::vec3 source, float density, float r){
	//generate cube of points and add if dist from center is > 0
	float gap = 1 / density;
	std::vector<glm::vec3> points;
	for(float i = -r; i <= r; i+=(r*gap)){
		for(float j = -r; j <= r; j+=(r*gap)){
			for(float k = -r; k <= r; k+=(r*gap)){
				if(sqrt(i*i + j*j + k*k) <= r){
					points.push_back(glm::vec3(i+source.x, j+source.y, k+source.z));
				}
			}
		}
	}
	return points;
}

std::vector<float> interpolateSingleFloats(float a, float b, int noOfVals){
	std::vector<float> result;
	float step = (b-a)/(noOfVals-1);
	result.push_back(a);
	for(int i = 1; i < noOfVals-1; i++){
		result.push_back(a+i*step);
	}
	result.push_back(b);
	return result;
}

std::vector<float> interpolateSingleFloatsNonlinear(float a, float b, int noOfVals, float depthA, float depthB){
	std::vector<float> result;
	float step = (b-a)/(noOfVals-1);
	result.push_back(a);
	for(int i = 1; i < noOfVals-1; i++){
		result.push_back(a+i*step);
	}
	result.push_back(b);
	return result;
}

std::vector<glm::vec3> interpolateTripleFloats(glm::vec3 a, glm::vec3 b, int noOfVals){
	std::vector<glm::vec3> result;
	glm::vec3 step((b[0]-a[0])/(noOfVals-1), (b[1]-a[1])/(noOfVals-1), (b[2]-a[2])/(noOfVals-1));

	result.push_back(a);
	for(int i = 1; i < noOfVals-1; i++){
		glm::vec3 temp(a[0]+i*step[0], a[1]+i*step[1], a[2]+i*step[2]);
		result.push_back(temp);
	}
	result.push_back(b);
	return result;
}

void drawLine(DrawingWindow &window, CanvasPoint a, CanvasPoint b, Colour col){
	int n = std::max( std::fabs(a.x-b.x), std::fabs(a.y-b.y) )+2;
	std::vector<float> xs = interpolateSingleFloats(a.x, b.x, n);
	std::vector<float> ys = interpolateSingleFloats(a.y, b.y, n);
	for(int i = 0; i < n; i++){
		if(xs[i] >= 0 && xs[i] < WIDTH && ys[i] >= 0 && ys[i] < HEIGHT){
			window.setPixelColour(xs[i], ys[i], packCol(col));
		}
	}
}

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle t, Colour col){
	drawLine(window, t.vertices[0], t.vertices[1], col);
	drawLine(window, t.vertices[1], t.vertices[2], col);
	drawLine(window, t.vertices[2], t.vertices[0], col);
}

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle t, Colour col, float depthBuf[][HEIGHT]){
	CanvasPoint v0 = t.vertices[0];
	CanvasPoint v1 = t.vertices[1];
	CanvasPoint v2 = t.vertices[2];
	CanvasPoint v3;
	CanvasPoint temp;

	//sort vertices
	if(v0.y > v1.y){ std::swap(v0, v1); }
	if(v1.y > v2.y){ std::swap(v1, v2); }
	if(v0.y > v1.y){ std::swap(v0, v1); }

	//find v3
	float m = (v0.y - v2.y) / (v0.x - v2.x);

	float c = v0.y - m*v0.x;
	float x = (v1.y-c)/m;
	v3 = CanvasPoint(x, v1.y);
	if(m == 0){ v3 = v1; } //horizontal line -> shouldnt happen?
	if(v0.x == v2.x){ v3.x = v2.x; } //vertical line -> possible

	//calc depth of v3
	float frac = (v3.y-v0.y)/(v2.y-v0.y);
	v3.depth = v0.depth + frac*(v2.depth-v0.depth);

	//std::cout<<col.name <<": " <<"v0:" << v0.depth<<" v1:" << v1.depth<<" v2:" << v2.depth<<std::endl;

	if(v3.x < v1.x){ std::swap(v3, v1); }


	//fill top triangle
	//    v0
	// v1    v3
	int height = std::fabs(std::ceil(v1.y) - std::floor(v0.y))+1;
	std::vector<float> leftside = interpolateSingleFloats(std::floor(v0.x), std::floor(v1.x), height);
	std::vector<float> leftsideDepths = interpolateSingleFloats(v0.depth, v1.depth, height);
	std::vector<float> rightside = interpolateSingleFloats(std::ceil(v0.x), std::ceil(v3.x), height);
	std::vector<float> rightsideDepths = interpolateSingleFloats(v0.depth, v3.depth, height);
	for(int currY = 0; currY < height; currY++){
		int width = std::fabs(rightside[currY]-leftside[currY])+1;
		std::vector<float> rowDepths = interpolateSingleFloats(leftsideDepths[currY], rightsideDepths[currY], width);
		for(int currX = 0; currX < width; currX++){
			int x = leftside[currY]+currX;
			int y = currY+std::floor(v0.y);
			float d = 1 / rowDepths[currX];
			if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
				if(d >= depthBuf[x][y]){
					window.setPixelColour(x, y, packCol(col));
					depthBuf[x][y] = d;
				}
			}
		}
	}

	//fill bottom triangle
	// v1    v3
	//    v2
	height = std::fabs(std::floor(v1.y)-std::ceil(v2.y))+1;
	leftside = interpolateSingleFloats(std::floor(v2.x), std::floor(v1.x), height);
	leftsideDepths = interpolateSingleFloats(v2.depth, v1.depth, height);
	rightside = interpolateSingleFloats(std::ceil(v2.x), std::ceil(v3.x), height);
	rightsideDepths = interpolateSingleFloats(v2.depth, v3.depth, height);
	for(int currY = 0; currY < height; currY++){
		int width = std::fabs(rightside[currY]-leftside[currY])+1;
		std::vector<float> rowDepths = interpolateSingleFloats(leftsideDepths[currY], rightsideDepths[currY], width);
		for(int currX = 0; currX < width; currX++){
			int x = leftside[currY]+currX;
			int y = std::ceil(v2.y)-currY;
			float d = 1 / rowDepths[currX];
			if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
				if(d >= depthBuf[x][y]){
					window.setPixelColour(x, y, packCol(col));
					depthBuf[x][y] = d;
				}
			}
		}
	}

}

void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle t, Colour col, float depthBuf[][HEIGHT]){
	CanvasPoint v0 = t.vertices[0];
	CanvasPoint v1 = t.vertices[1];
	CanvasPoint v2 = t.vertices[2];
	CanvasPoint v3;
	CanvasPoint temp;
	TextureMap texture = modelTextures[col.textureIndex];

	//sort vertices
	if(v0.y > v1.y){ std::swap(v0, v1); }
	if(v1.y > v2.y){ std::swap(v1, v2); }
	if(v0.y > v1.y){ std::swap(v0, v1); }

	//find v3
	float m = (v0.y - v2.y) / (v0.x - v2.x);
	float c = v0.y - m*v0.x;
	float x = (v1.y-c)/m;
	v3 = CanvasPoint(x, v1.y);
	float prop = (v2.x-v3.x)/(v2.x-v0.x);
	float v3textx = v2.texturePoint.x + prop*(v0.texturePoint.x-v2.texturePoint.x);
	float v3texty = v2.texturePoint.y + prop*(v0.texturePoint.y-v2.texturePoint.y);
	v3.texturePoint = TexturePoint(v3textx, v3texty);
	//calc depth of v3
	float frac = (v3.y-v0.y)/(v2.y-v0.y);
	v3.depth = v0.depth + frac*(v2.depth-v0.depth);


	if(v3.x < v1.x){ std::swap(v3, v1); }

	//fill top triangle
	//    v0
	// v1    v3
	/*float z0, z1, c0, c1;
	if(v0.depth > v1.depth && v0.depth > v3.depth){
		z0 = v0.depth;
		c0 = v0.texturePoint.y;
	} else if(v1.depth > v0.depth && v1.depth > v3.depth){
		z0 = v1.depth;
		c0 = v1.texturePoint.y;
	} else{
		z0 = v3.depth;
		c0 = v3.texturePoint.y;
	}
	if(v0.depth < v1.depth && v0.depth < v3.depth){
		z1 = v0.depth;
		c1 = v0.texturePoint.y;
	} else if(v1.depth < v0.depth && v1.depth < v3.depth){
		z1 = v1.depth;
		c1 = v1.texturePoint.y;
	} else{
		z1 = v3.depth;
		c1 = v3.texturePoint.y;
	}
	float cOverZ0 = c0/z0;
	float cOverZ1 = c1/z1;
	float oneOverZ0 = 1/z0;
	float oneOverZ1 = 1/z0;*/
	int height = std::fabs(std::ceil(v1.y) - std::floor(v0.y))+1;
	std::vector<float> leftside = interpolateSingleFloats(std::floor(v0.x), std::floor(v1.x), height);
	std::vector<float> leftsideDepths = interpolateSingleFloats(v0.depth, v1.depth, height);
	std::vector<float> leftsideTexXs = interpolateSingleFloatsNonlinear(v0.texturePoint.x, v1.texturePoint.x, height, v0.depth, v1.depth);
	std::vector<float> leftsideTexYs = interpolateSingleFloatsNonlinear(v0.texturePoint.y, v1.texturePoint.y, height, v0.depth, v1.depth);
	std::vector<float> rightside = interpolateSingleFloats(std::ceil(v0.x), std::ceil(v3.x), height);
	std::vector<float> rightsideDepths = interpolateSingleFloats(v0.depth, v3.depth, height);
	std::vector<float> rightsideTexXs = interpolateSingleFloatsNonlinear(v0.texturePoint.x, v3.texturePoint.x, height, v0.depth, v3.depth);
	std::vector<float> rightsideTexYs = interpolateSingleFloatsNonlinear(v0.texturePoint.y, v3.texturePoint.y, height, v0.depth, v3.depth);
	for(int currY = 0; currY < height; currY++){
		int width = std::fabs(rightside[currY]-leftside[currY])+1;
		std::vector<float> rowDepths = interpolateSingleFloats(leftsideDepths[currY], rightsideDepths[currY], width);
		int rowLen = rightside[currY]-leftside[currY]+2;
		std::vector<float> txPoints = interpolateSingleFloatsNonlinear(leftsideTexXs[currY], rightsideTexXs[currY], rowLen, leftsideDepths[currY], rightsideDepths[currY]);
		std::vector<float> tyPoints = interpolateSingleFloatsNonlinear(leftsideTexYs[currY], rightsideTexYs[currY], rowLen, leftsideDepths[currY], rightsideDepths[currY]);
		for(int currX = 0; currX < width; currX++){
			int x = leftside[currY]+currX;
			int y = currY+std::floor(v0.y);
			float d = 1 / rowDepths[currX];
			if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
				if(d >= depthBuf[x][y]){
					//float vertProp = currY/height;
					window.setPixelColour(x, y, texture.pixels[round(tyPoints[currX])*texture.width + round(txPoints[currX])]);
					depthBuf[x][y] = d;
				}
			}
		}
	}

	//fill bottom triangle
	// v1    v3
	//    v2
	height = std::fabs(std::floor(v1.y)-std::ceil(v2.y))+1;
	leftside = interpolateSingleFloats(std::floor(v2.x), std::floor(v1.x), height);
	leftsideDepths = interpolateSingleFloats(v2.depth, v1.depth, height);
	leftsideTexXs = interpolateSingleFloatsNonlinear(v2.texturePoint.x, v1.texturePoint.x, height, v2.depth, v1.depth);
	leftsideTexYs = interpolateSingleFloatsNonlinear(v2.texturePoint.y, v1.texturePoint.y, height, v2.depth, v1.depth);
	rightside = interpolateSingleFloats(std::ceil(v2.x), std::ceil(v3.x), height);
	rightsideDepths = interpolateSingleFloats(v2.depth, v3.depth, height);
	rightsideTexXs = interpolateSingleFloatsNonlinear(v2.texturePoint.x, v3.texturePoint.x, height, v2.depth, v3.depth);
	rightsideTexYs = interpolateSingleFloatsNonlinear(v2.texturePoint.y, v3.texturePoint.y, height, v2.depth, v3.depth);
	for(int currY = 0; currY < height; currY++){
		int width = std::fabs(rightside[currY]-leftside[currY])+1;
		std::vector<float> rowDepths = interpolateSingleFloats(leftsideDepths[currY], rightsideDepths[currY], width);
		int rowLen = rightside[currY]-leftside[currY]+2;
		std::vector<float> txPoints = interpolateSingleFloatsNonlinear(leftsideTexXs[currY], rightsideTexXs[currY], rowLen, leftsideDepths[currY], rightsideDepths[currY]);
		std::vector<float> tyPoints = interpolateSingleFloatsNonlinear(leftsideTexYs[currY], rightsideTexYs[currY], rowLen, leftsideDepths[currY], rightsideDepths[currY]);
		for(int currX = 0; currX < width; currX++){
			int x = leftside[currY]+currX;
			int y = std::ceil(v2.y)-currY;
			float d = 1 / rowDepths[currX];
			if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
				if(d >= depthBuf[x][y]){
					window.setPixelColour(x, y, texture.pixels[round(tyPoints[currX])*texture.width + round(txPoints[currX])]);
					depthBuf[x][y] = d;
				}
			}
		}
	}
}

CanvasPoint getCanvasIntersectionPoint(glm::vec3 inPoint){
	glm::vec3 relVec = glm::vec3(-(inPoint[0]-camPos[0]), -(inPoint[1]-camPos[1]), inPoint[2]-camPos[2]);
	relVec = camAng * relVec;
	relVec.y *= -1;
	float u = (focal * (relVec[0]/relVec[2]))*(WIDTH/2) + WIDTH/2;
	float v = (focal * (relVec[1]/relVec[2]))*(HEIGHT/2) + HEIGHT/2;
	return CanvasPoint(u, v, -relVec[2]);
}

Colour getTextureCol(ModelTriangle tri, glm::vec3 point){
	TextureMap texture = modelTextures[tri.colour.textureIndex];
	glm::vec3 v0 = tri.vertices[0];
	glm::vec3 v1 = tri.vertices[1];
	glm::vec3 v2 = tri.vertices[2];

	float AOV0 = glm::length(glm::cross(point-v1, v2-v1))/2; //calculate areas of subtriangles
	float AOV1 = glm::length(glm::cross(point-v2, v0-v2))/2;
	float AOV2 = glm::length(glm::cross(point-v0, v1-v0))/2;
	float totalA = AOV0 + AOV1 + AOV2;	//proportional areas instead of absolute areas
	AOV0 = AOV0 / totalA;
	AOV1 = AOV1 / totalA;
	AOV2 = AOV2 / totalA;

	TexturePoint vt0 = tri.texturePoints[0];
	TexturePoint vt1 = tri.texturePoints[1];
	TexturePoint vt2 = tri.texturePoints[2];

	float textureX = (AOV0*vt0.x + AOV1*vt1.x + AOV2*vt2.x);
	float textureY = (AOV0*vt0.y + AOV1*vt1.y + AOV2*vt2.y);

	float floorX = (float)floor(textureX);
	float floorY = (float)floor(textureY);
	float ceilX = (float)ceil(textureX);
	float ceilY = (float)ceil(textureY);
	Colour q11 = unpackCol(texture.pixels[floorY*texture.width + floorX]);
	Colour q12 = unpackCol(texture.pixels[ceilY*texture.width + floorX]);
	Colour q21 = unpackCol(texture.pixels[floorY*texture.width + ceilX]);
	Colour q22 = unpackCol(texture.pixels[ceilY*texture.width + ceilX]);
	float x2x1 = ceilX - floorX;
	float y2y1 = ceilY - floorY;
	float x2x = ceilX - textureX;
	float y2y = ceilY - textureY;
	float yy1 = textureY - floorY;
	float xx1 = textureX - floorX;
	Colour col;
	col.red = 1.0 / (x2x1 * y2y1) * ( (q11.red * x2x * y2y) + (q21.red * xx1 * y2y) + (q12.red * x2x * yy1) + (q22.red * xx1 * yy1) );
	col.green = 1.0 / (x2x1 * y2y1) * ( (q11.green * x2x * y2y) + (q21.green * xx1 * y2y) + (q12.green * x2x * yy1) + (q22.green * xx1 * yy1) );
	col.blue = 1.0 / (x2x1 * y2y1) * ( (q11.blue * x2x * y2y) + (q21.blue * xx1 * y2y) + (q12.blue * x2x * yy1) + (q22.blue * xx1 * yy1) );

	//Colour col = unpackCol(texture.pixels[round(textureY)*texture.width + round(textureX)]);
	return col;
}

Colour getTriCol(ModelTriangle tri, glm::vec3 point){
	Colour col;
	if(rendSet_Textures && tri.colour.textureIndex != -1){
		col = getTextureCol(tri, point);
	} else {
		col = tri.colour;
	}
	return col;
}

RayTriangleIntersection getClosestIntersection(glm::vec3 rayPos, glm::vec3 rayDirection, float disp){
	RayTriangleIntersection result;
	result.distanceFromCamera = INT_MAX;
	for(int n = 0; n < model.size(); n++){
		ModelTriangle tri = model[n];
		glm::vec3 e0 = tri.vertices[1] - tri.vertices[0];
		glm::vec3 e1 = tri.vertices[2] - tri.vertices[0];
		glm::vec3 SPVector = rayPos - tri.vertices[0];
		glm::mat3 DEMatrix(-rayDirection, e0, e1);
		glm::vec3 tempSol = glm::inverse(DEMatrix) * SPVector;
		float t = tempSol[0];
		float u = tempSol[1];
		float v = tempSol[2];
		if(result.distanceFromCamera > t && t > disp && (u >= 0.0) && (u <= 1.0) && (v >= 0.0) && (v <= 1.0) && (u + v) <= 1.0){
			result.distanceFromCamera = t;
			result.intersectionPoint = tri.vertices[0] + u*e0 + v*e1;
			result.intersectedTriangle = tri;
			result.triangleIndex = n;
		}
	}
	return result;
}

glm::vec3 closestPointOnLine(glm::vec3 point, glm::vec3 l1, glm::vec3 l2){
	float t = glm::dot((l2-l1),(l1-point))/glm::dot((l2-l1), (l2-l1));
	glm::vec3 G = l1 - t*(l2-l1);
	return G;
}

float getShadow(glm::vec3 point){
	float shadePerc = 1;
	if(rendSet_SoftShadows){
		int sum = 0;
		for(int i = 0; i < lightPoints.size(); i++){
			glm::vec3 pToLSphereVec = lightPoints[i] - point;
			RayTriangleIntersection pToLSphereRay = getClosestIntersection(point, pToLSphereVec, 0.01);
			float pToLSphereLength = glm::length(pToLSphereVec);
			if(pToLSphereLength <= pToLSphereRay.distanceFromCamera){
				sum++;
			}
		}
		double lightNoPoints = lightPoints.size();
		shadePerc = sum/lightNoPoints;
	} else {
		glm::vec3 PtoL = lightPos - point;
		RayTriangleIntersection pToLRay = getClosestIntersection(point, PtoL, 0.01);
		if(pToLRay.distanceFromCamera <= glm::length(PtoL)){ //light blocked
			shadePerc = 0.2;
		}
	}
	return shadePerc;
}

float getDimness(glm::vec3 point, glm::vec3 normal){
	glm::vec3 PtoL = lightPos - point;
	float angleInc = glm::dot( PtoL, normal );
	float dimPerc;
	if(angleInc > 0){
		float pointToLightDist = glm::length(PtoL);
		float proxDim = 8 * ( 1 / (4 * M_PI * (pointToLightDist*pointToLightDist)) );
		dimPerc = (proxDim * angleInc);
	} else {
		dimPerc = 0.05;
	}
	return dimPerc;
}

float getSpecBrightness(glm::vec3 point, glm::vec3 normal, glm::vec3 source){
	glm::vec3 PtoL = lightPos - point;
	float angleInc = glm::dot( PtoL, normal );
	float specBright;
	if(angleInc > 0){
		glm::vec3 R = (-PtoL) - (2.0f * normal * glm::dot((-PtoL), normal));
		glm::vec3 rayVec = point-source;
		specBright = pow( glm::dot(glm::normalize(R), glm::normalize(rayVec)), 126 );
	} else {
		specBright = 0;
	}
	return specBright;
}

Colour getColFromRay(glm::vec3 source, glm::vec3 rayVec, float displacement, int depth);

glm::vec3 getNormal(ModelTriangle tri, glm::vec3 point, glm::vec3 triNormal){
	glm::vec3 result = triNormal;
	if(rendSet_BumpMap && tri.colour.bumpIndex != -1){
		TextureMap texture = modelTextures[tri.colour.bumpIndex];
		glm::vec3 v0 = tri.vertices[0];
		glm::vec3 v1 = tri.vertices[1];
		glm::vec3 v2 = tri.vertices[2];

		float AOV0 = glm::length(glm::cross(point-v1, v2-v1))/2; //calculate areas of subtriangles
		float AOV1 = glm::length(glm::cross(point-v2, v0-v2))/2;
		float AOV2 = glm::length(glm::cross(point-v0, v1-v0))/2;
		float totalA = AOV0 + AOV1 + AOV2;	//proportional areas instead of absolute areas
		AOV0 = AOV0 / totalA;
		AOV1 = AOV1 / totalA;
		AOV2 = AOV2 / totalA;

		glm::vec2 vt0 = glm::vec2(tri.bumpPoints[0].x, tri.bumpPoints[0].y);
		glm::vec2 vt1 = glm::vec2(tri.bumpPoints[1].x, tri.bumpPoints[1].y);
		glm::vec2 vt2 = glm::vec2(tri.bumpPoints[2].x, tri.bumpPoints[2].y);

		float textureX = (AOV0*vt0.x + AOV1*vt1.x + AOV2*vt2.x);
		float textureY = (AOV0*vt0.y + AOV1*vt1.y + AOV2*vt2.y);
		textureX = textureX >= texture.width ? texture.width-1 : textureX;
		textureY = textureY >= texture.height ? texture.height-1 : textureY;
		float floorX = (float)floor(textureX);
		float floorY = (float)floor(textureY);
		float ceilX = (float)ceil(textureX);
		float ceilY = (float)ceil(textureY);
		Colour q11 = unpackCol(texture.pixels[floorY*texture.width + floorX]);
		Colour q12 = unpackCol(texture.pixels[ceilY*texture.width + floorX]);
		Colour q21 = unpackCol(texture.pixels[floorY*texture.width + ceilX]);
		Colour q22 = unpackCol(texture.pixels[ceilY*texture.width + ceilX]);
		float x2x1 = ceilX - floorX;
		float y2y1 = ceilY - floorY;
		float x2x = ceilX - textureX;
		float y2y = ceilY - textureY;
		float yy1 = textureY - floorY;
		float xx1 = textureX - floorX;
		Colour col;
		col.red = 1.0 / (x2x1 * y2y1) * ( (q11.red * x2x * y2y) + (q21.red * xx1 * y2y) + (q12.red * x2x * yy1) + (q22.red * xx1 * yy1) );
		col.green = 1.0 / (x2x1 * y2y1) * ( (q11.green * x2x * y2y) + (q21.green * xx1 * y2y) + (q12.green * x2x * yy1) + (q22.green * xx1 * yy1) );
		col.blue = 1.0 / (x2x1 * y2y1) * ( (q11.blue * x2x * y2y) + (q21.blue * xx1 * y2y) + (q12.blue * x2x * yy1) + (q22.blue * xx1 * yy1) );
		glm::vec3 bumpMapVec = glm::vec3(col.red, col.green, col.blue)-125.0f;

		glm::vec3 deltaPos1 = v1-v0;
		glm::vec3 deltaPos2 = v2-v0;
		glm::vec2 deltaUV1 = vt1-vt0;
		glm::vec2 deltaUV2 = vt2-vt0;
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r);
		glm::vec3 bitangent = glm::normalize((deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r);
		glm::vec3 bumpedNormal = (bumpMapVec.x * tangent) + (bumpMapVec.y * bitangent) + (bumpMapVec.z * triNormal);

		//glm::vec3 bumpNormal = glm::vec3(0, 0, 1);
		//glm::vec3 rotVec = glm::normalize(glm::cross(bumpNormal, triNormal));
		//float angle = acos(glm::dot(bumpNormal, triNormal)/(glm::length(bumpNormal)*glm::length(triNormal)));
		//glm::vec3 newNormal = bumpMapVec*cos(angle) + glm::cross(rotVec, bumpMapVec)*sin(angle) + rotVec*glm::dot(rotVec, bumpMapVec)*(1-cos(angle));
		result = bumpedNormal;
	}
	return glm::normalize(result);
}

Colour getColRT(glm::vec3 point, ModelTriangle tri, glm::vec3 source, int depth, glm::vec3 normal){
	Colour col = getTriCol(tri, point);
	if(rendSet_Lighting){
		float dimness = getDimness(point, normal);
		col = dimCol(col, dimness);
		float brightness = getSpecBrightness(point, normal, source);
		col = brightCol(col, brightness);
	}
	if(rendSet_Shadows){
		float shade = getShadow(point);
		col = dimCol(col, shade);
	}
	if(rendSet_Refraction && tri.colour.refractionIndex > 0 && depth < DEPTH_MAX){
		glm::vec3 incidentVector = glm::normalize(point - source);
		normal = glm::normalize(normal);
		float eta = 1/tri.colour.refractionIndex;
		float angle = acos(glm::dot(-incidentVector, normal)/(glm::length(incidentVector)*glm::length(normal)));
		if(angle > M_PI/2){
			normal = -normal;
			eta = 1/eta;
			angle = acos(glm::dot(-incidentVector, normal)/(glm::length(incidentVector)*glm::length(normal)));
		}
		glm::vec3 refractedVec;
		float N_dot_I = glm::dot(normal, incidentVector);
		float k = 1.f - eta * eta * (1.f - N_dot_I * N_dot_I);
		if(k < 0.f) {//reflect
			refractedVec = incidentVector - (2.0f)*(normal)*glm::dot(incidentVector, normal);
		} else {//refract
			refractedVec = eta * incidentVector - (eta * N_dot_I + sqrt(k)) * normal;
		}
		col = getColFromRay(point, refractedVec, 0.01, ++depth);
	}
	if(rendSet_Reflection && tri.colour.reflectionIndex > 0 && depth < DEPTH_MAX){
		glm::vec3 incidentVector = glm::normalize(point - source);
		glm::vec3 reflectedVec = incidentVector - (2.0f)*(normal)*glm::dot(incidentVector, normal);
		Colour reflectedCol = getColFromRay(point, reflectedVec, 0.01, ++depth);
		Colour actCol = col;
		float refColTotal = (reflectedCol.red + reflectedCol.green + reflectedCol.blue)/(255*3) - 0.0000001;
		float weight = pow( (tri.colour.reflectionIndex) , 1-(refColTotal) );
		float angle = acos(glm::dot(-incidentVector, normal)/(glm::length(incidentVector)*glm::length(normal)));
		if(angle > M_PI/2){
			normal = -normal;
			angle = acos(glm::dot(-incidentVector, normal)/(glm::length(incidentVector)*glm::length(normal)));
		}
		weight *= pow( (angle/(M_PI/2)), tri.colour.fresnel );
		col.red = (reflectedCol.red-actCol.red)*weight + actCol.red;
		col.green = (reflectedCol.green-actCol.green)*weight + actCol.green;
		col.blue = (reflectedCol.blue-actCol.blue)*weight + actCol.blue;
	}
	return col;
}

Colour getColGouraud(glm::vec3 point, ModelTriangle tri, glm::vec3 source, int depth){
	glm::vec3 v0 = tri.vertices[0]; glm::vec3 vn0 = tri.vertexNormals[0]; //get tri vec and normals
	glm::vec3 v1 = tri.vertices[1]; glm::vec3 vn1 = tri.vertexNormals[1];
	glm::vec3 v2 = tri.vertices[2]; glm::vec3 vn2 = tri.vertexNormals[2];
	Colour dimPercV0 = getColRT(v0, tri, source, depth, vn0); //calculate vertex dimPercs
	Colour dimPercV1 = getColRT(v1, tri, source, depth, vn1);
	Colour dimPercV2 = getColRT(v2, tri, source, depth, vn2);
	float AOV0 = glm::length(glm::cross(point-v1, v2-v1))/2; //calculate areas of subtriangles
	float AOV1 = glm::length(glm::cross(point-v2, v0-v2))/2;
	float AOV2 = glm::length(glm::cross(point-v0, v1-v0))/2;
	float totalA = AOV0 + AOV1 + AOV2;	//proportional areas instead of absolute areas
	AOV0 = AOV0 / totalA;
	AOV1 = AOV1 / totalA;
	AOV2 = AOV2 / totalA;
	Colour col;
	col.red = (AOV0 * dimPercV0.red) + (AOV1 * dimPercV1.red) + (AOV2 * dimPercV2.red);
	col.green = (AOV0 * dimPercV0.green) + (AOV1 * dimPercV1.green) + (AOV2 * dimPercV2.green);
	col.blue = (AOV0 * dimPercV0.blue) + (AOV1 * dimPercV1.blue) + (AOV2 * dimPercV2.blue);
	return col;
}

Colour getColPhong(glm::vec3 point, ModelTriangle tri, glm::vec3 source, int depth){
	Colour col;
	glm::vec3 v0 = tri.vertices[0]; glm::vec3 vn0 = tri.vertexNormals[0]; //get tri vec and normals
	glm::vec3 v1 = tri.vertices[1]; glm::vec3 vn1 = tri.vertexNormals[1];
	glm::vec3 v2 = tri.vertices[2]; glm::vec3 vn2 = tri.vertexNormals[2];
	float AOV0 = glm::length(glm::cross(point-v1, v2-v1))/2; //calculate areas of subtriangles
	float AOV1 = glm::length(glm::cross(point-v2, v0-v2))/2;
	float AOV2 = glm::length(glm::cross(point-v0, v1-v0))/2;
	float totalA = AOV0 + AOV1 + AOV2;	//proportional areas instead of absolute areas
	AOV0 = AOV0 / totalA;
	AOV1 = AOV1 / totalA;
	AOV2 = AOV2 / totalA;
	glm::vec3 norm;
	norm = (AOV0 * vn0) + (AOV1 * vn1) + (AOV2 * vn2);
	col = getColRT(point, tri, source, depth, getNormal(tri, point, norm)); //calculate vertex dimPercs
	return col;
}

Colour getColOfPoint(glm::vec3 point, ModelTriangle tri, glm::vec3 source, int depth){
	Colour col;
	if(rendering == 3){
		col = getColRT(point, tri, source, depth, getNormal(tri, point, tri.normal));
	} else if(rendering == 4){
		col = getColGouraud(point, tri, source, depth);
	} else if(rendering == 5){
		col = getColPhong(point, tri, source, depth);
	}
	return col;
}

Colour getColFromRay(glm::vec3 source, glm::vec3 rayVec, float displacement, int depth){
	Colour col;
	RayTriangleIntersection ray = getClosestIntersection(source, rayVec, displacement);
	if(ray.distanceFromCamera == INT_MAX){ //ray did not intersect any triangles
		if(rayVec.y > 0){
			col = Colour(157, 190, 245);
		} else {
			col = Colour(61, 56, 50);
		}
	} else { //ray intersected at least one triangle
		glm::vec3 destPoint = ray.intersectionPoint;
		col = getColOfPoint(destPoint, ray.intersectedTriangle, source, depth);
	}
	return col;
}

void drawRayTrace(DrawingWindow &window, float i, float j){
	float right = ((2*i)/WIDTH) - 1;
	float down = ((2*j)/HEIGHT) - 1;
	glm::vec3 rayVec = camAng * glm::vec3(right, -down, -1*focal);
	Colour col = getColFromRay(camPos, rayVec, 0.1, 0);
	window.setPixelColour(i, j, packCol(col));
}

bool checkInFront(ModelTriangle input){
	bool result = true;
	for(int i = 0; i < 3; i++){
		glm::vec3 camToVertex = input.vertices[i] - camPos;
		glm::vec3 camForward = camAng[2];
		float angleBetween = acos(glm::dot(camToVertex, camForward)/(glm::length(camToVertex)*glm::length(camForward)));
		result = result & (angleBetween > M_PI/2 || angleBetween < M_PI/2);
	}
	return result;
}

void draw(DrawingWindow &window, bool progress) {
	window.clearPixels();
	Colour sky = Colour(157, 190, 245);
	Colour ground = Colour(61, 56, 50);
	for(int j = 0; j < HEIGHT; j++){
		if(j < HEIGHT/2){
			for(int i = 0; i < WIDTH; i++){
				window.setPixelColour(i, j, packCol(sky));
			}
		} else {
			for(int i = 0; i < WIDTH; i++){
				window.setPixelColour(i, j, packCol(ground));
			}
		}
	}

	if( rendering == 1 ){
		for(int i=0; i < model.size(); i++){
			if(checkInFront(model[i])){
				CanvasPoint v0 = getCanvasIntersectionPoint(model[i].vertices[0]);
				CanvasPoint v1 = getCanvasIntersectionPoint(model[i].vertices[1]);
				CanvasPoint v2 = getCanvasIntersectionPoint(model[i].vertices[2]);
				CanvasTriangle tri = CanvasTriangle(v0, v1, v2);
				drawStrokedTriangle(window, tri, model[i].colour);
			}
		}
	} else if( rendering == 2 ){
		float depthBuf[WIDTH][HEIGHT];
		for(int i = 0; i < WIDTH; i++){
			for(int j = 0; j < HEIGHT; j++){
				depthBuf[i][j] = -1;
			}
		}
		for(int i=0; i < model.size(); i++){
			if(checkInFront(model[i])){
				CanvasPoint v0 = getCanvasIntersectionPoint(model[i].vertices[0]);
				v0.texturePoint = model[i].texturePoints[0];
				CanvasPoint v1 = getCanvasIntersectionPoint(model[i].vertices[1]);
				v1.texturePoint = model[i].texturePoints[1];
				CanvasPoint v2 = getCanvasIntersectionPoint(model[i].vertices[2]);
				v2.texturePoint = model[i].texturePoints[2];
				CanvasTriangle tri = CanvasTriangle(v0, v1, v2);
				if(model[i].colour.textureIndex == -1 || !rendSet_Textures){
					drawFilledTriangle(window, tri, model[i].colour, depthBuf);
				} else {
					drawTexturedTriangle(window, tri, model[i].colour, depthBuf);
				}
			}
		}
	} else if( rendering == 3 || rendering == 4 || rendering == 5){
		std::chrono::system_clock::time_point timeA = std::chrono::system_clock::now();
		int sum = 0;
		std::vector<TexturePoint> pixels;
		for(float i = 0; i < WIDTH; i++){
			for(float j = 0; j < HEIGHT; j++){
				pixels.push_back(TexturePoint(i, j));
			}
		}
		std::random_shuffle( pixels.begin(), pixels.end() );
		for(TexturePoint pixel : pixels){
			drawRayTrace(window, pixel.x, pixel.y);
			sum += 1;
			std::chrono::system_clock::time_point timeB = std::chrono::system_clock::now();
			std::chrono::duration<double, std::ratio<60>> timeDiff = timeB - timeA;
			float ratio = 100*sum/(WIDTH*HEIGHT);
			float timeRemaining = ((float)((int)(100 * ((timeDiff.count() / ratio) * (100-ratio)))))/100;
			if(progress){
				std::cout << "\r" << (int)ratio << "% complete; time elapsed: " << (float)((int)(100 * timeDiff.count()))/100 << " mins; est. remaining: " << timeRemaining << " mins    " << std::flush;
			}
			if(sum%1000 == 0 && frames == 0){
				window.renderFrame();
			}
		}
		if(progress){
			std::cout << std::endl;
		}
	}
	CanvasPoint light = getCanvasIntersectionPoint(lightPos);
	drawCircle(window, light.x, light.y, 5, packCol(Colour(0, 0, 0)));
	drawCircle(window, light.x, light.y, 4, packCol(Colour(255, 255, 255)));
}

std::vector<Colour> mtlParser(std::string mtlFilepath){
	std::vector<Colour> cols;
	std::ifstream ifs (mtlFilepath, std::ifstream::in);
	char c = ifs.get();
	std::string currentWord = "";
	int mode = 0; // mode 0, reading headers, mode 1 - name of colour, mode 2,3,4 - colour itself
	Colour currentCol = Colour(-1, -1, -1);
	while(ifs.good()){
		if(c == '\n' || c == ' '){
			if(mode == 0){
				if(currentWord == "newmtl"){
					mode = 1;
				} else if(currentWord == "Kd"){
					mode = 2;
				} else if(currentWord == "Rfli") {
					mode =  5;
				} else if(currentWord == "Rfri") {
					mode =  6;
				} else if(currentWord == "map_Kd") {
					mode =  7;
				} else if(currentWord == "map_Bn") {
					mode =  8;
				} else if(currentWord == "Frsn") {
					mode =  9;
				} else {
					std::cout << "err: " << currentWord << std::endl;
				}
			} else if(mode == 1){
				if(currentCol.red != -1){
					cols.push_back(currentCol);
				}
				currentCol = Colour(-1, -1, -1);
				currentCol.name = currentWord;
				mode = 0;
			} else {
				if(mode == 2){
					currentCol.red = round(std::stof(currentWord)*255);
					mode++;
				} else if(mode == 3){
					currentCol.green = round(std::stof(currentWord)*255);
					mode++;
				} else if(mode == 4){
					currentCol.blue = round(std::stof(currentWord)*255);
					mode = 0;
					c = ifs.get();
				} else if(mode == 5){
					currentCol.reflectionIndex = std::stof(currentWord);
					mode = 0;
				} else if(mode == 6){
					currentCol.refractionIndex = std::stof(currentWord);
					mode = 0;
				} else if(mode == 7){
					TextureMap texture = TextureMap(currentWord);
					modelTextures.push_back(texture);
					currentCol.textureIndex = modelTextures.size()-1;
					mode = 0;
				} else if(mode == 8){
					TextureMap bumpMap = TextureMap(currentWord);
					modelTextures.push_back(bumpMap);
					currentCol.bumpIndex = modelTextures.size()-1;
					mode = 0;
				} else if(mode == 9){
					currentCol.fresnel = std::stof(currentWord);
					if(currentCol.fresnel == 1){
						currentCol.fresnel = 0.99;
					}
					mode = 0;
				}
			}
			currentWord = "";
		} else {
			currentWord.append(1, c);
		}
		c = ifs.get();
	}
	cols.push_back(currentCol);
	ifs.close();
	return cols;
}

glm::vec3 vtov3(std::vector<float> in){
	glm::vec3 out = glm::vec3(in[0], in[1], in[2]);
	return out;
}

void moveTriangle(ModelTriangle &input, glm::vec3 translation){
	for(int i = 0; i < 3; i++){
		input.vertices[i] = input.vertices[i] + translation;
	}
}

void objParser(std::vector<ModelTriangle> &dest, std::string objFilepath, float scale, std::vector<Colour> materials, glm::vec3 translation){
	std::vector<std::vector<float>> vs;
	std::vector<std::vector<float>> vts;
	std::vector<std::vector<float>> vns;
	std::vector<std::vector<float>> vbs;
	std::ifstream ifs (objFilepath, std::ifstream::in);
	char c = '\n';
	char firstLetterOfLine = c;
	char target = ' ';
	int currentMat = 0;

	while(ifs.good()){
		if(c == '\n'){
			c = ifs.get();
			firstLetterOfLine = c;
			while(firstLetterOfLine == '#'){
				while(c != '\n'){
					c = ifs.get();
				}
				firstLetterOfLine = c;
			}
			if(firstLetterOfLine == 'v' || firstLetterOfLine == 'f' || firstLetterOfLine == 'u'){
				target = firstLetterOfLine;
			} else { target = ' '; }
			c = ifs.get();
		} else {
			if(target == 'v'){
				if(c == 'n'){
					std::vector<float> toAdd;
					c = ifs.get();
					for(int i = 0; i < 3; i++){
						c = ifs.get();
						std::string currentNum = "";
						while(c != ' ' && c != '\n'){
							currentNum = currentNum.append(1, c);
							c = ifs.get();
						}
						toAdd.push_back(std::stof(currentNum));
					}
					vns.push_back(toAdd);
				} else if(c == 't'){
					std::vector<float> toAdd;
					c = ifs.get();
					for(int i = 0; i < 2; i++){
						c = ifs.get();
						std::string currentNum = "";
						while(c != ' ' && c != '\n'){
							currentNum = currentNum.append(1, c);
							c = ifs.get();
						}
						toAdd.push_back(std::stof(currentNum));
					}
					vts.push_back(toAdd);
				} else if(c == 'b'){
					std::vector<float> toAdd;
					c = ifs.get();
					for(int i = 0; i < 2; i++){
						c = ifs.get();
						std::string currentNum = "";
						while(c != ' ' && c != '\n'){
							currentNum = currentNum.append(1, c);
							c = ifs.get();
						}
						toAdd.push_back(std::stof(currentNum));
					}
					vts.push_back(toAdd);
				} else {
					std::vector<float> toAdd;
					for(int i = 0; i < 3; i++){
						c = ifs.get();
						std::string currentNum = "";
						while(c != ' ' && c != '\n'){
							currentNum = currentNum.append(1, c);
							c = ifs.get();
						}
						toAdd.push_back(std::stof(currentNum)*scale);
					}
					vs.push_back(toAdd);
				}
			} else if(target == 'f'){
				std::vector<std::vector<int>> toAdd;
				for(int i = 0; i < 3; i++){
					std::vector<int> addition;
					c = ifs.get();
					std::string currentNum = "";
					while(c != ' ' && c != '\n' && ifs.good()){
						if(c == '/'){
							if(currentNum == ""){currentNum = "0";}
							addition.push_back(std::stoi(currentNum)-1);
							currentNum = "";
						} else {
							currentNum = currentNum.append(1, c);
						}
						c = ifs.get();
					}
					while(addition.size() < 4){
						if(currentNum == ""){currentNum = "0";}
						addition.push_back(std::stoi(currentNum)-1);
						currentNum = "";
					}
					toAdd.push_back(addition);
				}
				ModelTriangle newTri = ModelTriangle( vtov3(vs[toAdd[0][0]]), vtov3(vs[toAdd[1][0]]), vtov3(vs[toAdd[2][0]]), materials[currentMat]);
				moveTriangle(newTri, translation);
				newTri.normal = getNormal(newTri);
				if(toAdd[0][1] != -1){
					float texWidth = modelTextures[newTri.colour.textureIndex].width;
					float texHeight = modelTextures[newTri.colour.textureIndex].height;
					TexturePoint newVt;
					newVt = TexturePoint(vts[toAdd[0][1]][0]*texWidth, vts[toAdd[0][1]][1]*texHeight);
					newTri.texturePoints[0] = newVt;
					newVt = TexturePoint(vts[toAdd[1][1]][0]*texWidth, vts[toAdd[1][1]][1]*texHeight);
					newTri.texturePoints[1] = newVt;
					newVt = TexturePoint(vts[toAdd[2][1]][0]*texWidth, vts[toAdd[2][1]][1]*texHeight);
					newTri.texturePoints[2] = newVt;
				}
				if(toAdd[0][2] != -1){
					newTri.vertexNormals[0] = glm::normalize( vtov3( vns[ toAdd[0][2] ] ) );
					newTri.vertexNormals[1] = glm::normalize( vtov3( vns[ toAdd[1][2] ] ) );
					newTri.vertexNormals[2] = glm::normalize( vtov3( vns[ toAdd[2][2] ] ) );
				} else {
					newTri.vertexNormals[0] = newTri.normal;
					newTri.vertexNormals[1] = newTri.normal;
					newTri.vertexNormals[2] = newTri.normal;
				}
				if(toAdd[0][3] != -1){
					float texWidth = modelTextures[newTri.colour.bumpIndex].width;
					float texHeight = modelTextures[newTri.colour.bumpIndex].height;
					TexturePoint newVt;
					newVt = TexturePoint(vts[toAdd[0][3]][0]*texWidth, vts[toAdd[0][3]][1]*texHeight);
					newTri.bumpPoints[0] = newVt;
					newVt = TexturePoint(vts[toAdd[1][3]][0]*texWidth, vts[toAdd[1][3]][1]*texHeight);
					newTri.bumpPoints[1] = newVt;
					newVt = TexturePoint(vts[toAdd[2][3]][0]*texWidth, vts[toAdd[2][3]][1]*texHeight);
					newTri.bumpPoints[2] = newVt;
				}
				dest.push_back(newTri);
			} else if(target == 'u'){
				c = ifs.get();c = ifs.get();c = ifs.get();c = ifs.get();c = ifs.get();c = ifs.get();
				std::string currentNum = "";
				while(c != '\n'){
					currentNum = currentNum.append(1, c);
					c = ifs.get();
				}
				for(int a = 0; a < materials.size(); a++){
					if(materials[a].name == currentNum){
						currentMat = a;
					}
				}
			} else { c = ifs.get(); }
		}
	}
	ifs.close();
}

void handleEvent(SDL_Event event, DrawingWindow &window, std::vector<bool> &keys) {
	if (event.type == SDL_KEYDOWN) {
		frames = 0;
		float speed = 0.1;
		if (event.key.keysym.sym == SDLK_RETURN){ keys[0] = true; }
		else if (event.key.keysym.sym == SDLK_l){ keys[1] = true; }
		else if (event.key.keysym.sym == SDLK_1){ rendering = 1; }
		else if (event.key.keysym.sym == SDLK_2){ rendering = 2; }
		else if (event.key.keysym.sym == SDLK_3){ rendering = 3; }
		else if (event.key.keysym.sym == SDLK_4){ rendering = 4; }
		else if (event.key.keysym.sym == SDLK_5){ rendering = 5; }
		else if (event.key.keysym.sym == SDLK_UP){ camPos.z-=speed; }
		else if (event.key.keysym.sym == SDLK_DOWN){ camPos.z+=speed; }
		else if (event.key.keysym.sym == SDLK_LEFT){ camPos.x-=speed; }
		else if (event.key.keysym.sym == SDLK_RIGHT){ camPos.x+=speed; }
		else if (event.key.keysym.sym == SDLK_q){
			rendSet_Lighting = !rendSet_Lighting; //
		} else if (event.key.keysym.sym == SDLK_w){
			rendSet_SoftShadows = !rendSet_SoftShadows;
		} else if (event.key.keysym.sym == SDLK_e){
			rendSet_Reflection = !rendSet_Reflection; //
		} else if (event.key.keysym.sym == SDLK_r){
			rendSet_Refraction = !rendSet_Refraction; //
		} else if (event.key.keysym.sym == SDLK_t){
			rendSet_Textures = !rendSet_Textures; //
		} else if (event.key.keysym.sym == SDLK_y){
			rendSet_Shadows = !rendSet_Shadows; //
		} else if (event.key.keysym.sym == SDLK_u){
			rendSet_BumpMap = !rendSet_BumpMap; //
		}
	} else if (event.type == SDL_KEYUP) {
		for(int i = 0; i < keys.size(); i++){
			keys[i] = false;
		}
	}else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("assets/output.ppm");
		window.saveBMP("assets/output.bmp");
	}
}

void lookAt(glm::vec3 target){
	glm::vec3 forward = glm::normalize(camPos - target);
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
	glm::vec3 up = glm::normalize(glm::cross(forward, right));

	camAng = glm::mat3(right, up, forward);
}

void fpsDelay(std::chrono::system_clock::time_point &timeA, std::chrono::system_clock::time_point &timeB){
	// Maintain designated frequency of 5 Hz (200 ms per frame)
	timeA = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> work_time = timeA - timeB;

	if (work_time.count() < fps)
	{
		std::chrono::duration<double, std::milli> delta_ms(fps - work_time.count());
		auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
		std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
	}

	timeB = std::chrono::system_clock::now();
	//std::chrono::duration<double, std::milli> sleep_time = timeB - timeA;
	//printf("Time: %f \n", (work_time + sleep_time).count());
}

void moveObject(std::vector<ModelTriangle> &input, glm::vec3 translation){
	for(int i = 0; i < input.size(); i++){
		ModelTriangle tri = input[i];
		tri.vertices[0] = tri.vertices[0] + translation;
		tri.vertices[1] = tri.vertices[1] + translation;
		tri.vertices[2] = tri.vertices[2] + translation;
		input[i] = tri;
	}
}

int generatePath(DrawingWindow &window, glm::vec3 target, int pathToTake, int initialFileNo, int animLength){
	std::chrono::system_clock::time_point timeA = std::chrono::system_clock::now();
	int padding = 10;
	int noOfImages = animLength + 2*padding;
	for(int i = 0; i < noOfImages; i++){
		std::chrono::system_clock::time_point timeB = std::chrono::system_clock::now();
		std::chrono::duration<double, std::ratio<60>> timeDiff = timeB - timeA;
		float ratio = 100*i/(noOfImages);
		float timeRemaining = ((float)((int)(100 * ((timeDiff.count() / ratio) * (100-ratio)))))/100;
		std::cout << "\r" << (int)ratio << "% complete; time elapsed: " << (float)((int)(100 * timeDiff.count()))/100 << " mins; est. remaining: " << timeRemaining << " mins    " << std::flush;
		if(i > padding && i <= noOfImages-padding){
			if(pathToTake == 0){ // camera circles around object
				float yRotTheta = (2.0f*M_PI)/(noOfImages-padding*2);
				glm::mat3 camRot = glm::mat3(cos(yRotTheta), 0, -sin(yRotTheta), 0, 1, 0, sin(yRotTheta), 0, cos(yRotTheta));
				camPos = camRot*camPos;
			} else if(pathToTake == 1){ // camera moves in front of object
				float t = ((float)(i-padding)/(float)(noOfImages-padding*2))*2*M_PI;
				camPos = glm::vec3(sin(t), sin(t)*cos(t)/3, 3.0);
			} else if(pathToTake == 2){ // camera moves in front of object
				float t = ((float)(i-padding)/(float)(noOfImages-padding*2))*2*M_PI;
				lightPos = glm::vec3(sin(t)/4, sin(t)*cos(t)/6 + 0.3, 0);
				lightPoints = getPointsInSphere(lightPos, lightDensity, lightSize);
			}
			lookAt(target);
			draw(window, false);
			window.renderFrame();
		} else if(i == 0){
			lookAt(target);
			draw(window, false);
			window.renderFrame();
		}
		std::string filename = std::to_string(i+initialFileNo);
		while(filename.size() <= 4){
			filename.insert(0, "0");
		}
		filename.insert(0, "output");
		window.savePPM("assets/outputPPM/" + filename + ".ppm");
	}
	return noOfImages;
}

int main(int argc, char *argv[]) {
	lightPoints = getPointsInSphere(lightPos, lightDensity, lightSize);
	model.reserve(500);
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	//std::vector<Colour> cols = mtlParser("assets/cornell-box.mtl");
	//model = objParser("assets/cornell-box.obj", 0.17, cols);
	std::vector<Colour> cols = mtlParser("assets/materials.mtl");
	objParser(model, "assets/boxes/cornell-box-empty.obj", 0.17, cols, glm::vec3(0, 0, 0));
	objParser(model, "assets/logo/logo.obj", 0.001, cols, glm::vec3(-0.3, -0.2, -0.42));
	objParser(model, "assets/spheres/sphere-mirror.obj", 0.17, cols, glm::vec3(0.2, -0.55, -0.45));
	objParser(model, "assets/spheres/sphere-gloss.obj", 0.17, cols, glm::vec3(-0.2, -0.55, -0.45));
	objParser(model, "assets/spheres/sphere-glass.obj", 0.17, cols, glm::vec3(-0.25, 0.0, 0));
	objParser(model, "assets/spheres/bubble.obj", 0.17, cols, glm::vec3(0.2, -0.15, 0));
	//std::cout << sizeof(std::vector<ModelTriangle>) + (sizeof(ModelTriangle) * model.size()) << std::endl;
	//std::cout << sizeof(std::vector<ModelTriangle>) + (sizeof(ModelTriangle) * 500) << std::endl;

	//objParser(model, "assets/logo/logo.obj", 0.002, cols, glm::vec3(0, -0.4, -0.3));

	if(generating){
		rendSet_Lighting = false; //
		rendSet_Shadows = false; //
		rendSet_SoftShadows = false;
		rendSet_Reflection = false; //
		rendSet_Refraction = false; //
		rendSet_Textures = false; //
		rendSet_BumpMap = false; //

		int noOfPaths = 13;
		int sumOfImages = 0;
		glm::vec3 target = glm::vec3(0, 0, 0);
		rendering = 1;
		sumOfImages += generatePath(window, target, 0, 0, 100);
		std::cout << "\nCompleted 1/" << noOfPaths << std::endl;
		rendering = 2;
		sumOfImages += generatePath(window, target, 0, sumOfImages, 100);
		std::cout << "\nCompleted 2/" << noOfPaths << std::endl;
		rendSet_Textures = true;
		sumOfImages += generatePath(window, target, 0, sumOfImages, 100);
		std::cout << "\nCompleted 3/" << noOfPaths << std::endl;
		rendSet_Textures = false;
		rendering = 3;
		sumOfImages += generatePath(window, target, 1, sumOfImages, 50);
		std::cout << "\nCompleted 4/" << noOfPaths << std::endl;
		rendSet_Lighting = true;
		sumOfImages += generatePath(window, target, 1, sumOfImages, 50);
		std::cout << "\nCompleted 5/" << noOfPaths << std::endl;
		rendering = 4;
		sumOfImages += generatePath(window, target, 1, sumOfImages, 50);
		std::cout << "\nCompleted 6/" << noOfPaths << std::endl;
		rendering = 3;
		rendSet_Textures = true;
		sumOfImages += generatePath(window, target, 1, sumOfImages, 50);
		std::cout << "\nCompleted 7/" << noOfPaths << std::endl;
		rendSet_Shadows = true;
		sumOfImages += generatePath(window, target, 1, sumOfImages, 50);
		std::cout << "\nCompleted 8/" << noOfPaths << std::endl;
		rendering = 5;
		sumOfImages += generatePath(window, target, 1, sumOfImages, 50);
		std::cout << "\nCompleted 9/" << noOfPaths << std::endl;
		rendSet_Reflection = true;
		sumOfImages += generatePath(window, target, 1, sumOfImages, 50);	//move lookat smoothly?
		std::cout << "\nCompleted 10/" << noOfPaths << std::endl;
		rendSet_Refraction = true;
		sumOfImages += generatePath(window, target, 1, sumOfImages, 50);	//move lookat smoothly?
		std::cout << "\nCompleted 11/" << noOfPaths << std::endl;
		rendSet_BumpMap = true;
		sumOfImages += generatePath(window, target, 2, sumOfImages, 50);
		std::cout << "\nCompleted 12/" << noOfPaths << std::endl;
		rendSet_SoftShadows = true;
		sumOfImages += generatePath(window, target, 2, sumOfImages, 50);
		std::cout << "\nCompleted 13/" << noOfPaths << std::endl;
	} else {
		std::vector<bool> keys{ false, false };
		//enter l
		//1 for wireframe
		//2 for rasterized
		//3 for raytraced
		bool anim = false;
		bool lock = false;
		float lightY = 0.4;
		bool lightAnimTog = false;


		std::chrono::system_clock::time_point timeA = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point timeB = std::chrono::system_clock::now();
		while (true) {
			fpsDelay(timeA, timeB);
			if(keys[0] && !lock){
				anim = !anim;
				lock = true;
				camPos = glm::vec3(0, 0, 3.5);
				camAng = glm::mat3(1.0);
			}
			if(!keys[0]){ lock = false; }

			if(keys[1]){
				if(lightAnimTog){
					if(lightY < 0.5){
						lightY += 0.01;
					} else {
						lightAnimTog = false;
					}
				} else {
					if(lightY > 0.3){
						lightY -= 0.01;
					} else {
						lightAnimTog = true;
					}
				}
				lightPos.y = lightY;
			}

			if(anim){
				float yRotTheta = rendering == 3 ? 0.05 * M_PI : 0.01 * M_PI;
				float xRotTheta = 0;
				glm::mat3 camRot = glm::mat3(1, 0, 0, 0, cos(xRotTheta), sin(xRotTheta), 0, -sin(xRotTheta), cos(xRotTheta));
				camRot *= glm::mat3(cos(yRotTheta), 0, -sin(yRotTheta), 0, 1, 0, sin(yRotTheta), 0, cos(yRotTheta));
				camPos = camRot*camPos;
			}
			// We MUST poll for events - otherwise the window will freeze !
			if (window.pollForInputEvents(event)) handleEvent(event, window, keys);

			lookAt(glm::vec3(0, 0.0, 0));
			draw(window, true);
			// Need to render the frame at the end, or nothing actually gets shown on the screen !
			window.renderFrame();
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			frames++;
		}
	}
}



//alternate path - teardrop inside box
/*std::vector<std::tuple<glm::vec3, glm::vec3>> customPath;
float step = 0.04;
for(float i = 0; i < 6; i+=0.04){
	float xPos = - sin(i) * pow(sin(i/2), 3) /2;
	float yPos = sin(3*i-M_PI/2)/6;
	float zPos = 1+cos(i);
	glm::vec3 currentPos = glm::vec3(xPos, yPos, zPos);
	glm::vec3 currentAim = glm::vec3(0, 0.0, -1);//(x, y, z);
	customPath.push_back(std::make_tuple(currentPos, currentAim));
}*/
