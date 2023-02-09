#include <DrawingWindow.h>
#include <Colour.h>
#include <Triangle.h>
#include <Vector.h>
#include <Scene.h>

#include <Draw.h>
#include <Raster.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <math.h>
#include <iostream>

#define WIDTH 600
#define HEIGHT 600


///////////////
// CONSTANTS //
///////////////

Scene scene = Scene(WIDTH, HEIGHT);

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

Colour red = Colour("red", 255, 0, 0);
Colour green = Colour("green", 0, 255, 0);
Colour blue = Colour("blue", 0, 0, 255);
Colour cyan = Colour("cyan", 0, 255, 255);
Colour yellow = Colour("yellow", 255, 255, 0);
Colour white = Colour("white", 255, 255, 255);

std::vector<Triangle> cubeF = {
    Triangle(cubeV[4], cubeV[5], cubeV[7], yellow),//left
    Triangle(cubeV[4], cubeV[6], cubeV[7], yellow),//left
    Triangle(cubeV[0], cubeV[2], cubeV[4], white),//back
    Triangle(cubeV[6], cubeV[2], cubeV[4], white),//back
    Triangle(cubeV[1], cubeV[3], cubeV[5], green),//front
    Triangle(cubeV[7], cubeV[3], cubeV[5], green),//front
    Triangle(cubeV[0], cubeV[1], cubeV[4], blue),//base
    Triangle(cubeV[5], cubeV[1], cubeV[4], blue),//base
    Triangle(cubeV[2], cubeV[3], cubeV[6], cyan),//top
    Triangle(cubeV[7], cubeV[3], cubeV[6], cyan),//top
    Triangle(cubeV[0], cubeV[1], cubeV[2], red),//right
    Triangle(cubeV[3], cubeV[1], cubeV[2], red)//right
};

///////////////////////////
// MAIN DRAWING FUNCTION //
///////////////////////////

//main draw function for page refreshes
void draw(DrawingWindow &window) {
	window.clearPixels();
    scene.resetBuf();
    
    float cos10 = cos(0.01);
    float sin10 = sin(0.01);
    
    float rotArr[][3] = {{cos10, 0, sin10}, {0, 1, 0}, {-sin10, 0, cos10}};

    Matrix rotate = Matrix(rotArr);
        
    
    
    for(int i = 0; i < 12; i++){
        fill3DTriangle(window, scene, cubeF[i]);
        
        cubeF[i].p0 = rotate * cubeF[i].p0;
        cubeF[i].p1 = rotate * cubeF[i].p1;
        cubeF[i].p2 = rotate * cubeF[i].p2;
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
