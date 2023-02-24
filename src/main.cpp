#include <DrawingWindow.h>
#include <Colour.h>
#include <Triangle.h>
#include <Vector.h>
#include <Material.h>
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
	scene.camera = Vector(0, 0, 19);
	Vector origin = Vector(0, 0, 0);
	scene.lookAt(origin);

	scene.raytraceScene(window, frameCount == 0);
	//scene.rasterScene(window);
	//scene.wireframeScene(window);
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

//NOTE: These need refactoring, they are very messy

std::vector<Material *> mtlParser(std::string mtlFilepath){
	std::vector<Material *> materials;
	std::ifstream ifs (mtlFilepath, std::ifstream::in);
	char c = ifs.get();
	std::string currentWord = "";
	int mode = 0; // mode 0, reading headers, mode 1 - name of colour, mode 2,3,4 - colour itself, mode 5,6,7,8 - texturemaps
	Colour currentCol = Colour();
	Texture currentTex = Texture();
	bool firstMat = false;
	while(ifs.good()){
		if(c == '\n' || c == ' '){
			if(mode == 0){
				if(currentWord == "newmtl"){
					mode = 1;
				} else if(currentWord == "diffuse"){
					mode = 2;
				} else if(currentWord == "diffuseMap"){
					mode = 5;
				} else if(currentWord == "heightMap"){
					mode = 6;
				} else if(currentWord == "normalMap"){
					mode = 7;
				} else if(currentWord == "roughnessMap"){
					mode = 8;
				} else if(currentWord == "amboccMap"){
					mode = 9;
				} else {
					//std::cout << "err: " << currentWord << std::endl;
				}
			} else if(mode == 1){
				if(firstMat){
					Material *m = (Material *)malloc(sizeof(Material));
					m = new Material(currentCol.name, currentCol, currentTex);
					materials.push_back(m);
				}
				firstMat = true;
				currentCol = Colour();
				currentTex = Texture();
				currentCol.name = currentWord;
				mode = 0;
			} else if(mode < 5) {
				if(mode == 2){
					currentCol.red = round(std::stof(currentWord)*255);
					mode++;
				} else if(mode == 3){
					currentCol.green = round(std::stof(currentWord)*255);
					mode++;
				} else if(mode == 4){
					currentCol.blue = round(std::stof(currentWord)*255);
					mode = 0;
				}
			} else {
				if(mode == 5){
					currentTex.addDiffuseMap(currentWord);
					mode = 0;
				} else if(mode == 6){
					currentTex.addHeightMap(currentWord);
					mode = 0;
				} else if(mode == 7){
					currentTex.addNormalMap(currentWord);
					mode = 0;
				} else if(mode == 8){
					currentTex.addRoughnessMap(currentWord);
					mode = 0;
				} if(mode == 9){
					currentTex.addAmboccMap(currentWord);
					mode = 0;
				}
			}
			currentWord = "";
		} else {
			currentWord.append(1, c);
		}
		c = ifs.get();
	}
	Material *m = (Material *)malloc(sizeof(Material));
	m = new Material(currentCol.name, currentCol, currentTex);
	materials.push_back(m);
	ifs.close();
	return materials;
}

std::vector<SceneObject *> objParser(std::string objFilepath, float scale, std::vector<Material *> materials, Vector translation){
	std::vector<SceneObject *> objects;
	std::vector<Vector> vertices;
	std::vector<Vector> textureVertices;
	std::ifstream ifs (objFilepath, std::ifstream::in);
	char c = ifs.get();
	std::string currentWord = "";
	int mode = 0; // mode 0, reading headers, mode 1 - name of object, mode 2 - name of material, mode 3,4,5 - vertex points, mode 6,7,8 - face points, mode 9,10 - texturevertex
	std::string currentName = "";
	Material *currentMaterial = materials[0];
	std::vector<Triangle *> currentFaces = std::vector<Triangle *>();
	int currIndex = 0;
	bool firstObj = false;
	while(ifs.good()){
		if(c == '\n' || c == ' '){
			if(mode == 0){
				if(currentWord == "o"){
					mode = 1;
				} else if(currentWord == "usemtl"){
					mode = 2;
				} else if(currentWord == "vertex"){
					mode = 3;
				} else if(currentWord == "face"){
					mode = 6;
				} else if(currentWord == "texture"){
					mode = 9;
				} else {
					//std::cout << "err: " << currentWord << std::endl;
				}
			} else if(mode == 1){
				if(firstObj){

					SceneObject *object= (SceneObject *)malloc(sizeof(SceneObject));
					object = new SceneObject(currentFaces);
					object->name = currentName;
					object->material = currentMaterial;
					objects.push_back(object);
				}
				firstObj = true;
				currentName = "";
				currentMaterial = materials[0];
				currentFaces = std::vector<Triangle *>();
				currentName = currentWord;
				mode = 0;
			} else if(mode == 2) {
				for(Material *mat : materials){
					if(currentWord == (mat->name)){
						currentMaterial = mat;
						break;
					}
				}
				mode = 0;
			} else if(mode < 6) {
				if(mode == 3){
					vertices.push_back(Vector());
					currIndex = vertices.size()-1;
					vertices[currIndex].x = (std::stof(currentWord))*scale + translation.x;
					mode++;
				} else if(mode == 4){
					vertices[currIndex].y = (std::stof(currentWord))*scale + translation.x;
					mode++;
				} else if(mode == 5){
					vertices[currIndex].z = (std::stof(currentWord))*scale + translation.x;
					mode = 0;
				}
			} else if(mode < 9){
				if(mode == 6){
					Triangle *tri= (Triangle *)malloc(sizeof(Triangle));
					tri = new Triangle();
					tri->mat = currentMaterial;

					currentFaces.push_back(tri);
					currIndex = currentFaces.size() - 1;

					int slashIndex = currentWord.find('/');
					currentFaces[currIndex]->p0 = vertices[std::stoi(currentWord.substr(0, slashIndex))-1];
					if(slashIndex < currentWord.size()-1){
						currentFaces[currIndex]->t0 = textureVertices[std::stoi(currentWord.substr(slashIndex+1))-1];
					}
					mode++;
				} else if(mode == 7){
					int slashIndex = currentWord.find('/');
					currentFaces[currIndex]->p1 = vertices[std::stoi(currentWord.substr(0, slashIndex))-1];
					if(slashIndex < currentWord.size()-1){
						currentFaces[currIndex]->t1 = textureVertices[std::stoi(currentWord.substr(slashIndex+1))-1];
					}
					mode++;
				} else if(mode == 8){
					int slashIndex = currentWord.find('/');
					currentFaces[currIndex]->p2 = vertices[std::stoi(currentWord.substr(0, slashIndex))-1];
					if(slashIndex < currentWord.size()-1){
						currentFaces[currIndex]->t2 = textureVertices[std::stoi(currentWord.substr(slashIndex+1))-1];
					}
					mode = 0;
				}
			} else {
				if(mode == 9){
					textureVertices.push_back(Vector());
					currIndex = textureVertices.size()-1;
					textureVertices[currIndex].x = (std::stof(currentWord));
					mode++;
				} else if(mode == 10){
					textureVertices[currIndex].y = (std::stof(currentWord));
					mode = 0;
				}
			}
			currentWord = "";
		} else {
			currentWord.append(1, c);
		}
		c = ifs.get();
	}
	SceneObject *object= (SceneObject *)malloc(sizeof(SceneObject));
	object = new SceneObject(currentFaces);
	object->name = currentName;
	object->material = currentMaterial;
	objects.push_back(object);
	ifs.close();
	return objects;
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

	std::vector<Material *> materials = mtlParser("assets/medievalDiningRoom/materials.mtl");
	std::vector<SceneObject *> model = objParser("assets/medievalDiningRoom/room.obj", 2, materials, Vector(0, 0, 0));
	std::vector<SceneObject *> obj1 = objParser("assets/medievalDiningRoom/shadowTest.obj", 2, materials, Vector(0, 0, 0));
	model.insert(model.end(), obj1.begin(), obj1.end());
	//std::vector<Material *> materials = mtlParser("assets/basicCornell/materials.mtl");
	//std::vector<SceneObject *> model = objParser("assets/basicCornell/cornell-box.obj", 5, materials, Vector(0, 0, 0));

	for(SceneObject *obj : model){
		obj->calcCollisionSphere();
		scene.objects.push_back(obj);
	}

	Light *light = (Light *)malloc(sizeof(Light));
	light = new Light(Vector(-5, 2, 8));
	scene.lights.push_back(light);
	Light *light2 = (Light *)malloc(sizeof(Light));
	light2 = new Light(Vector(5, 2, -8));
	scene.lights.push_back(light2);

	while (true) {
		handleMousePos();
		//while event exists in the queue, handle event and check for next item in queue
		int isAnotherEvent = window.pollForInputEvents(event);
		while (isAnotherEvent) {
			handleEvent(event, window);
			isAnotherEvent = window.pollForInputEvents(event);
		}

		if(frameCount == 0){
			draw(window);
			window.renderFrame();
		}
		frameCount+=1;
	}

	for(SceneObject *obj : scene.objects){
		for(Triangle *tri : obj->triangles){
			free(tri);
		}
		free(obj);
	}
	for(Light *light : scene.lights){
		free(light);
	}
	for(Material * mat : materials){
		free(mat);
	}
}
