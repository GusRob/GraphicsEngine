#include "Draw.h"
#include <math.h>
#include "Raster.h"

uint32_t packCol(Colour col){ return (255 << 24) + (int(col.red) << 16) + (int(col.green) << 8) + int(col.blue); }

//linear interpolation function used for rasterising
std::vector<float> linearInterpolation(float start, float end, int count){
	float step = (end-start)/count;
	std::vector<float> result;
	for(int i = 0; i < count; i++){
		result.push_back( (start + i*step) );
	}
	return result;
}

void drawOval(DrawingWindow &window, Vector p, int r, Colour colour){
    uint32_t col = packCol(colour);
    for(int y = 0; y < r; y++){
        int x = sqrt(r*r - y*y);
        for(int i = 0; i < x; i++){
		  window.setPixelColour(p.x + i, p.y + y, col);
		  window.setPixelColour(p.x + i, p.y - y, col);
		  window.setPixelColour(p.x - i, p.y + y, col);
		  window.setPixelColour(p.x - i, p.y - y, col);
        }
    }
}

//line drawing function
void drawLine(DrawingWindow &window, Vector p0, Vector p1, Colour colour){
    uint32_t col = packCol(colour);
	int noPixels = fmax( abs(p0.x-p1.x), abs(p0.y-p1.y) );
	std::vector<float> xs = linearInterpolation(p0.x, p1.x, noPixels);
	std::vector<float> ys = linearInterpolation(p0.y, p1.y, noPixels);
	for(int i = 0; i < noPixels; i++){
		window.setPixelColour(xs[i], ys[i], col);
	}
}


//triangle drawing function
void drawTriangle(DrawingWindow &window, Triangle tri){
	drawLine(window, tri.p0, tri.p1, tri.col);
	drawLine(window, tri.p1, tri.p2, tri.col);
	drawLine(window, tri.p2, tri.p0, tri.col);
}

//triangle filling function
void fillTriangle(DrawingWindow &window, Triangle tri){
    uint32_t col = packCol(tri.col);
	Vector top = tri.p0;
	Vector mid = tri.p1;
	Vector bot = tri.p2;
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
	Vector midAdj = Vector(xVal, mid.y); //cutoff of top-------bot
	if(midAdj.x < mid.x){
		std::swap(midAdj, mid);
	}
	float topH = top.y-mid.y;
	float botH = mid.y-bot.y;
	
    float leftTopStep = (mid.x - top.x) / topH;
    float rightTopStep = (midAdj.x - top.x) / topH;

    float leftBotStep = (bot.x - mid.x) / botH;
    float rightBotStep = (bot.x - midAdj.x) / botH;
	
    float left = top.x;
    float right = top.x;

	for(int i = 0; i < topH; i++){
        int startOfLine = left + (leftTopStep * i);
        int endOfLine = right + (rightTopStep * i);
		for(int j = startOfLine; j <= endOfLine; j++){
			window.setPixelColour(j, top.y - i, col);
		}
	}
    
    left = mid.x;
    right = midAdj.x;

    for(int i = 0; i < botH; i++){
        int startOfLine = left + (leftBotStep * i);
        int endOfLine = right + (rightBotStep * i);
        for(int j = startOfLine; j <= endOfLine; j++){
            window.setPixelColour(j, mid.y - i, col);
        }
    }
	window.setPixelColour(top.x, top.y, col);
	window.setPixelColour(mid.x, mid.y, col);
	window.setPixelColour(bot.x, bot.y, col);
}
