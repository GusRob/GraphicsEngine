#include <DrawingWindow.h>
#include <Colour.h>
#include <Point2D.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

#define WIDTH 600
#define HEIGHT 600


/////////////////////////
// WINDOW/UI FUNCTIONS //
/////////////////////////

//packs Colour object into unsigned int colours
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

//line drawing function
void drawLine(DrawingWindow &window, Point2D p1, Point2D p2, uint32_t col){
	int noPixels = fmax( abs(p1.x-p2.x), abs(p1.y-p2.y) );
	std::vector<float> xs = linearInterpolation(p1.x, p2.x, noPixels);
	std::vector<float> ys = linearInterpolation(p1.y, p2.y, noPixels);
	for(int i = 0; i < noPixels; i++){
		window.setPixelColour(xs[i], ys[i], col);
	}
}

//triangle drawing function
void drawTriangle(DrawingWindow &window, Point2D p1, Point2D p2, Point2D p3, uint32_t col){
	drawLine(window, p1, p2, col);
	drawLine(window, p2, p3, col);
	drawLine(window, p3, p1, col);
}

//triangle filling function
void fillTriangle(DrawingWindow &window, Point2D p1, Point2D p2, Point2D p3, uint32_t col){
	Point2D top = p1;
	Point2D mid = p2;
	Point2D bot = p3;
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
	Point2D midAdj = Point2D(xVal, mid.y); //cutoff of top-------bot
	if(midAdj.x < mid.x){
		std::swap(midAdj, mid);
	}
	float topH = top.y-mid.y;
	float botH = mid.y-bot.y;
	std::vector<float> leftTopXs = linearInterpolation(top.x, mid.x, topH);
	std::vector<float> rightTopXs = linearInterpolation(top.x, midAdj.x, topH);
	std::vector<float> leftBotXs = linearInterpolation(mid.x, bot.x, botH);
	std::vector<float> rightBotXs = linearInterpolation(midAdj.x, bot.x, botH);
	std::vector<float> leftSideXs;
	std::vector<float> rightSideXs;
	leftSideXs.insert( leftSideXs.end(), leftTopXs.begin(), leftTopXs.end() );
	leftSideXs.insert( leftSideXs.end(), leftBotXs.begin(), leftBotXs.end() );
	rightSideXs.insert( rightSideXs.end(), rightTopXs.begin(), rightTopXs.end() );
	rightSideXs.insert( rightSideXs.end(), rightBotXs.begin(), rightBotXs.end() );
	for(int i = 0; i < (top.y - bot.y); i++){
		for(int j = leftSideXs[i]; j <= rightSideXs[i]; j++){
			window.setPixelColour(j, top.y - i, col);
		}
	}
	window.setPixelColour(top.x, top.y, col);
	window.setPixelColour(mid.x, mid.y, col);
	window.setPixelColour(bot.x, bot.y, col);
}

//main draw function for page refreshes
void draw(DrawingWindow &window) {
	window.clearPixels();
	fillTriangle(window, Point2D(50, 100), Point2D(220, 300), Point2D(100, 400), packCol(Colour(255, 255, 255)));
}

//event handler function - each frame execute one event from the queue
void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_MOUSEBUTTONDOWN) {
	} else if(event.type == SDL_MOUSEBUTTONUP){
	}
}

//handle mouse position function - called upon window update
void handleMousePos(){
	int x, y;
	SDL_GetMouseState(&x, &y);
}


///////////////
// MAIN FUNC //
///////////////

//main function - entry point of program
//creates window
//loops event handling and rendering
int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	while (true) {
		handleMousePos();
		//while event exists in the queue, handle event and check for next item in queue
		int isAnotherEvent = window.pollForInputEvents(event);
		while (isAnotherEvent) {
			handleEvent(event, window);
			isAnotherEvent = window.pollForInputEvents(event);
		}

		draw(window);
		window.renderFrame();
	}
}
