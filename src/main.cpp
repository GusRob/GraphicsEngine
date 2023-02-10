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

int frameCount = 0;
Scene scene = Scene(WIDTH, HEIGHT);

///////////////////////////
// MAIN DRAWING FUNCTION //
///////////////////////////

//main draw function for page refreshes
void draw(DrawingWindow &window) {
	window.clearPixels();
	scene.resetBuf();

/*
	float cos10 = cos(0.01);
	float sin10 = sin(0.01);

	float rotArrY[][3] = {{cos10, 0, sin10}, {0, 1, 0}, {-sin10, 0, cos10}};

	Matrix rotateY = Matrix(rotArrY);

	scene.camera = rotateY * scene.camera;
	*/
	scene.camera = Vector(0, 0, 10);
	scene.lookAt(Vector(0, 0, 0));

	if(frameCount%100 == 0){
		scene.raytraceScene(window);
	} else {
		scene.rasterScene(window);
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

////////////////////
// OBJ/MTL PARSER //
////////////////////

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
				} else {
					//std::cout << "err: " << currentWord << std::endl;
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

void objParser(std::vector<Triangle> &dest, std::string objFilepath, float scale, std::vector<Colour> materials, Vector translation){
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
				Triangle newTri = Triangle( Vector(vs[toAdd[0][0]])+translation, Vector(vs[toAdd[1][0]]) + translation, Vector(vs[toAdd[2][0]]) + translation, materials[currentMat]);
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

///////////////
// MAIN FUNC //
///////////////

//main function - entry point of program
//creates window
//loops event handling and rendering
int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	std::vector<Colour> cols = mtlParser("assets/materials.mtl");
	std::vector<Triangle> model;
	objParser(model, "assets/cornell-box.obj", 1, cols, Vector(0, 0, 0));
	scene.objects.push_back(SceneObject(model));

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
		frameCount+=1;
	}
}
