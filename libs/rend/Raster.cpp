#include "Raster.h"

#include "Draw.h"

Vector getPointOnCanvas(Scene &scene, Vector p){
    Vector w = scene.windowDim;
    Vector camToPoint = p - scene.camera;
    camToPoint = scene.cameraAngle * camToPoint;
    float i = ( (camToPoint.x/camToPoint.z) + 1) * w.x/2;
    float j = ( (camToPoint.y/camToPoint.z) + 1) * w.y/2;
    return Vector(i, j, camToPoint.z);
}

//triangle drawing function
void draw3DTriangle(DrawingWindow &window, Scene &scene, Triangle tri){
    Vector p0 = getPointOnCanvas(scene, tri.p0);
    Vector p1 = getPointOnCanvas(scene, tri.p1);
    Vector p2 = getPointOnCanvas(scene, tri.p2);
	drawLine(window, p0, p1, tri.col);
	drawLine(window, p1, p2, tri.col);
	drawLine(window, p2, p0, tri.col);
}

//triangle drawing function
void fill3DTriangle(DrawingWindow &window, Scene &scene, Triangle tri){
    uint32_t col = packCol(tri.col);
    Vector top = getPointOnCanvas(scene, tri.p0);
    Vector mid = getPointOnCanvas(scene, tri.p1);
    Vector bot = getPointOnCanvas(scene, tri.p2);
	if(bot.y > mid.y){
		std::swap(bot, mid);
	}
	if(mid.y > top.y){
		std::swap(mid, top);
		if(bot.y > mid.y){
			std::swap(bot, mid);
		}
	}
	float fracUpOfMidAdj = (mid.y-bot.y) / (top.y-bot.y);
	float xVal = (bot.x + fracUpOfMidAdj*(top.x-bot.x));
    float midAdjDepth = bot.z + fracUpOfMidAdj*(top.z-bot.z);
	Vector midAdj = Vector(xVal, mid.y, midAdjDepth); //cutoff of top-------bot
	if(midAdj.x < mid.x){
		std::swap(midAdj, mid);
	}

    
	float topH = top.y-mid.y;
	float botH = mid.y-bot.y;
	
    if(topH > 0){
        float leftTopStep = (mid.x - top.x) / topH;
        float rightTopStep = (midAdj.x - top.x) / topH;

        float leftTopStepDepth = (mid.z - top.z) / topH;
        float rightTopStepDepth = (midAdj.z - top.z) / topH;

        float left = top.x;
        float right = top.x;
        float leftDepth = top.z;
        float rightDepth = top.z;

        for(int i = 0; i < topH; i++){
            int startOfLine = left + (leftTopStep * i);
            int endOfLine = right + (rightTopStep * i);
            float startOfLineDepth = leftDepth + (leftTopStepDepth * i);
            float endOfLineDepth = rightDepth + (rightTopStepDepth * i);
            float lineLength = (endOfLine - startOfLine);
            float depthStep = (endOfLineDepth - startOfLineDepth) / lineLength;
            for(int j = 0; j <= lineLength; j++){
                int x = j + startOfLine;
                int y = top.y - i;
                float d = 1/(startOfLineDepth + (j*depthStep));
                if(scene.depthBuf[x][y] <= d){
                    window.setPixelColour(x, y, col);
                    scene.depthBuf[x][y] = d;
                }
            }
        }
    }
    if(botH > 0){
        float leftBotStep = (bot.x - mid.x) / botH;
        float rightBotStep = (bot.x - midAdj.x) / botH;

        float leftBotStepDepth = (bot.z - mid.z) / botH;
        float rightBotStepDepth = (bot.z - midAdj.z) / botH;


        float left = mid.x;
        float right = midAdj.x;
        float leftDepth = mid.z;
        float rightDepth = midAdj.z;

        for(int i = 0; i < botH; i++){
            int startOfLine = left + (leftBotStep * i);
            int endOfLine = right + (rightBotStep * i);
            float startOfLineDepth = leftDepth + (leftBotStepDepth * i);
            float endOfLineDepth = rightDepth + (rightBotStepDepth * i);
            float lineLength = (endOfLine - startOfLine);
            float depthStep = (endOfLineDepth - startOfLineDepth) / lineLength;
            for(int j = 0; j <= lineLength; j++){
                int x = j + startOfLine;
                int y = mid.y - i;
                float d = 1/(startOfLineDepth + (j*depthStep));
                if(scene.depthBuf[x][y] <= d){
                    window.setPixelColour(x, y, col);
                    scene.depthBuf[x][y] = d;
                }
            }
        }
    }
	window.setPixelColour(top.x, top.y, col);
	window.setPixelColour(mid.x, mid.y, col);
	window.setPixelColour(bot.x, bot.y, col);
}
/*
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

}*/
