#include <DrawingWindow.h>
#include <Colour.h>
#include <Triangle.h>
#include <Vector.h>

#include <Draw.h>
#include <Raster.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <math.h>

#define WIDTH 600
#define HEIGHT 600


///////////////
// CONSTANTS //
///////////////

Vector camera = Vector(0, 0, -10);
Vector cameraAngle = Vector(0, 0, 1);

std::vector<Vector> cubeV = {
    Vector(1, 1, 1), 
    Vector(1, 1, -1), 
    Vector(1, -1, 1), 
    Vector(1, -1, -1), 
    Vector(-1, 1, 1), 
    Vector(-1, 1, -1), 
    Vector(-1, -1, 1), 
    Vector(-1, -1, -1)
};

Colour red = Colour(255, 0, 0);
Colour green = Colour(0, 255, 0);
Colour blue = Colour(0, 0, 255);
Colour yellow = Colour(0, 255, 255);
Colour pink = Colour(255, 255, 0);
Colour white = Colour(255, 255, 255);

std::vector<Triangle> cubeF = {
    Triangle(cubeV[0], cubeV[1], cubeV[2], red),
    Triangle(cubeV[3], cubeV[1], cubeV[2], red),
    Triangle(cubeV[4], cubeV[5], cubeV[7], pink),
    Triangle(cubeV[7], cubeV[5], cubeV[7], pink),
    Triangle(cubeV[0], cubeV[2], cubeV[4], white),
    Triangle(cubeV[6], cubeV[2], cubeV[4], white),
    Triangle(cubeV[1], cubeV[3], cubeV[5], green),
    Triangle(cubeV[7], cubeV[3], cubeV[5], green),
    Triangle(cubeV[0], cubeV[1], cubeV[4], blue),
    Triangle(cubeV[5], cubeV[1], cubeV[4], blue),
    Triangle(cubeV[2], cubeV[3], cubeV[6], yellow),
    Triangle(cubeV[7], cubeV[3], cubeV[6], yellow)
};


////////////////////////
// GRAPHICS FUNCTIONS //
////////////////////////

//packs Colour object into unsigned int colours
uint32_t packCol(Colour col){ return (255 << 24) + (int(col.red) << 16) + (int(col.green) << 8) + int(col.blue); }

///////////////////////////
// MAIN DRAWING FUNCTION //
///////////////////////////

//main draw function for page refreshes
void draw(DrawingWindow &window) {
	window.clearPixels();
    for(int i = 1; i < 5; i++){
        drawOval(window, Vector(100*i, 100*i), 10, packCol(Colour(255, 0, 0)));
    }
}

////////////////////
// EVENT HANDLERS //
////////////////////

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
