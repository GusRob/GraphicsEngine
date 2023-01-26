#include <DrawingWindow.h>
#include <Colour.h>
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

//line drawing function, doesnt have perfect rasterization but does the job
void drawLine(DrawingWindow &window, int p1x, int p1y, int p2x, int p2y, Colour col){
}

//main draw function for page refreshes
void draw(DrawingWindow &window) {
	window.clearPixels();
}

//event handler function - each frame execute one event from the queue
//
//
void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_MOUSEBUTTONDOWN) {
	} else if(event.type == SDL_MOUSEBUTTONUP){
	}
}

//handle mouse position function - called upon window update
//
//
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
