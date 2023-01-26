#include <array>
#include "DrawingWindow.h"
// On some platforms you may need to include <cstring> (if you compiler can't find memset !)

DrawingWindow::DrawingWindow() {}

DrawingWindow::DrawingWindow(int w, int h, bool fullscreen) : width(w), height(h), pixelBuffer(w * h) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) printMessageAndQuit("Could not initialise SDL: ", SDL_GetError());
	IMG_Init(IMG_INIT_JPG);
	uint32_t flags = SDL_WINDOW_OPENGL;
	if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	int ANYWHERE = SDL_WINDOWPOS_UNDEFINED;
	window = SDL_CreateWindow("COMS30020", ANYWHERE, ANYWHERE, width, height, flags);
	if (!window) printMessageAndQuit("Could not set video mode: ", SDL_GetError());
	// Set rendering to software (hardware acceleration doesn't work on all platforms)
	flags = SDL_RENDERER_SOFTWARE;
	// You could try hardware acceleration if you like - by uncommenting the below line
	// flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer(window, -1, flags);
	if (!renderer) printMessageAndQuit("Could not create renderer: ", SDL_GetError());
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, width, height);
	int PIXELFORMAT = SDL_PIXELFORMAT_ARGB8888;
	texture = SDL_CreateTexture(renderer, PIXELFORMAT, SDL_TEXTUREACCESS_STATIC, width, height);
	if (!texture) printMessageAndQuit("Could not allocate texture: ", SDL_GetError());
}

void DrawingWindow::renderFrame() {
	SDL_UpdateTexture(texture, nullptr, pixelBuffer.data(), width * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void DrawingWindow::saveBMP(const std::string &filename) const {
	auto surface = SDL_CreateRGBSurfaceFrom((void *) pixelBuffer.data(), width, height, 32,
	                                        width * sizeof(uint32_t),
	                                        0xFF << 16, 0xFF << 8, 0xFF << 0, 0xFF << 24);
	SDL_SaveBMP(surface, filename.c_str());
}

void DrawingWindow::savePPM(const std::string &filename) const {
	std::ofstream outputStream(filename, std::ofstream::out);
	outputStream << "P6\n";
	outputStream << width << " " << height << "\n";
	outputStream << "255\n";

	for (size_t i = 0; i < width * height; i++) {
		std::array<char, 3> rgb {{
				static_cast<char> ((pixelBuffer[i] >> 16) & 0xFF),
				static_cast<char> ((pixelBuffer[i] >> 8) & 0xFF),
				static_cast<char> ((pixelBuffer[i] >> 0) & 0xFF)
		}};
		outputStream.write(rgb.data(), 3);
	}
	outputStream.close();
}

bool DrawingWindow::pollForInputEvents(SDL_Event &event) {
	if (SDL_PollEvent(&event)) {
		if ((event.type == SDL_QUIT) || ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_CLOSE)) || ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE))) {
			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			IMG_Quit();
			SDL_Quit();
			printMessageAndQuit("Exiting", nullptr);
		}
		SDL_Event dummy;
		// Clear the event queue by getting all available events
		// This seems like bad practice (because it will skip some events) however preventing backlog is paramount !
		while (SDL_PollEvent(&dummy));
		return true;
	}
	return false;
}

void DrawingWindow::setPixelColour(size_t x, size_t y, uint32_t colour) {
	if ((x >= width) || (y >= height)) {
		std::cout << x << "," << y << " not on visible screen area" << std::endl;
	} else pixelBuffer[(y * width) + x] = colour;
}

uint32_t DrawingWindow::getPixelColour(size_t x, size_t y) {
	if ((x >= width) || (y >= height)) {
		std::cout << x << "," << y << " not on visible screen area" << std::endl;
		return -1;
	} else return pixelBuffer[(y * width) + x];
}


void DrawingWindow::drawText(const std::string &text, int imX, int imY, int imW, int imH){
	TTF_Init();
	//this opens a font style and sets a size
	TTF_Font* Sans = TTF_OpenFont("./assets/font.ttf", 50);

	// this is the color in rgb format,
	// maxing out all would give you the color white,
	// and it will be your text's color
	SDL_Color White = {240, 100, 0};

	// as TTF_RenderText_Solid could only be used on
	// SDL_Surface then you have to create the surface first
	SDL_Surface* surfaceMessage =
	    TTF_RenderText_Solid(Sans, "A", White);

	// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = 200;  //controls the rect's x coordinate
	Message_rect.y = 200; // controls the rect's y coordinte
	Message_rect.w = 100; // controls the width of the rect
	Message_rect.h = 100; // controls the height of the rect
	// (0,0) is on the top left of the window/screen,
	// think a rect as the text's box,
	// that way it would be very simple to understand

	// Now since it's a texture, you have to put RenderCopy
	// in your game loop area, the area where the whole code executes

	// you put the renderer's name first, the Message,
	// the crop size (you can ignore this if you don't want
	// to dabble with cropping), and the rect which is the size
	// and coordinate of your texture
	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

	// Don't forget to free your surface and texture
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	TTF_CloseFont(Sans);
	TTF_Quit();
}



void DrawingWindow::drawIMG(const std::string &filename, int imX, int imY, int imW, int imH){
	SDL_Surface* image = IMG_Load(filename.c_str());
	if (image == nullptr) {
		std::cout << "IMG_Load: " << IMG_GetError() << "\n";
	} else {
		SDL_Surface* formattedImage = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
		Uint32* pixels = (Uint32 *) formattedImage->pixels;
		int actualImWidth = formattedImage->w;
		int actualImHeight = formattedImage->h;
		for(int y = 0; y < imH; y++){
			for(int x = 0; x < imW; x++){
				Uint32 pixelCol = pixels[actualImWidth * (int)( ((float)y/(float)imH)*actualImHeight ) + (int)( ((float)x/(float)imW)*actualImWidth )];
				if(pixelCol != 16777215 && pixelCol != 0){
					pixelBuffer[width * (imY+y) + (imX+x)] = pixelCol;
				}
			}
		}
		SDL_UnlockSurface(formattedImage);
		SDL_FreeSurface(formattedImage);
	}
	SDL_FreeSurface(image);
}

void DrawingWindow::clearPixels() {
	std::fill(pixelBuffer.begin(), pixelBuffer.end(), 0);
}

void printMessageAndQuit(const std::string &message, const char *error) {
	if (error == nullptr) {
		std::cout << message << std::endl;
		exit(0);
	} else {
		std::cout << message << " " << error << std::endl;
		exit(1);
	}
}
