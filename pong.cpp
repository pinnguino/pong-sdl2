#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <random>
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_video.h"
#include "constants.hpp"
#include <ctime>

SDL_Window *window = nullptr;
SDL_Surface *surface = nullptr;
Uint32 lastFrame = 0;

struct Vector { // 2D Vector
	int x;
	int y;
};

struct Ball {
	SDL_Rect ball;
	Vector direction;
};

bool initSDL(void);
SDL_Rect init_player(int);
void init_ball(Ball &);
void drawMidLine(void);
void drawInitialGame(SDL_Rect, SDL_Rect, Ball);
void handleEvents(bool&, Ball&);
void move_players(SDL_Rect*, SDL_Rect*, float);
void normalizeVector(Vector &n, int value);
Vector randomVector(int, int);
int randomDir(void);
void move_ball(Ball &, SDL_Rect*, SDL_Rect*, float);
void update(SDL_Rect*, SDL_Rect*, Ball &);
void exit(void);


int main(int argc, char* argv[]){ // Args in main are necessary for SDL
	bool running = initSDL();
	SDL_Rect pl1 = init_player(1);
	SDL_Rect pl2 = init_player(2);
	Ball ball;
	init_ball(ball);
	drawInitialGame(pl1, pl2, ball);
	while(running){
		handleEvents(running, ball);
		update(&pl1, &pl2, ball);
	}
	exit();
	return 0;
}

bool initSDL(void){ // Initialize SDL and the window
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
		std::cout<<"ERROR: SDL not initialized.\n";
		SDL_Quit();
		return false;
	}
	window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	if (!window) {
		std::cout<<"ERROR: Window not created.\n";
		SDL_Quit();
		return false;
	}
	surface = SDL_GetWindowSurface(window);
	if(!surface) {
		std::cout<<"ERROR: Could not get window surface.\n";
		SDL_Quit();
		return false;
	}
	return true;
}

SDL_Rect init_player(int pl){ // Initialize a player
	int playerX=0, playerY=HEIGHT/2 - PLAYER_HEIGHT/2;
	if(pl==1){
		playerX=30;
	}
	else {
		playerX=WIDTH - 50;
	}
	SDL_Rect player = {playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT};
	return player;
}

void normalizeVector(Vector &n, int value){ // Sets the vector to a length of value
	double length = sqrt(pow(n.x, 2) + pow(n.y, 2));
	n.x = (int) (n.x * value/length);
	n.y = (int) (n.y * value/length);
}

Vector randomVector(int maxX, int maxY){ // Returns a Vector where x -> [1,maxX] and y -> [1, maxY]
	// For 800x600: maxX = 400; maxY = 250
	srand((unsigned)time(0));
	Vector v;
	v.x = randomDir() * (1 + rand()%(maxX+1));
	v.y = randomDir() * (1 + rand()%(maxY+1));
	return v;
}

void init_ball(Ball &ball){ // Set up the ball in the middle, with a random direction
	Vector dir = randomVector(400, 400);
	// Vector dir = {-1, 0};
	normalizeVector(dir, BALL_SPEED);
	ball.ball = {WIDTH/2 - BALL_SIZE, HEIGHT/2 - BALL_SIZE/2, BALL_SIZE, BALL_SIZE};
	ball.direction = dir;
}

void drawMidLine(void){
	int dashHeight = 20;
	SDL_Rect dash = {(WIDTH/2)-(dashHeight/2), 10, 5, dashHeight};
	for(int i=0; i<15; i++){
		SDL_FillRect(surface, &dash, white);
		dash.y += 2*dashHeight;
	}
}

void drawInitialGame(SDL_Rect pl1, SDL_Rect pl2, Ball ball) {
	SDL_FillRect(surface, &pl1, white);
	SDL_FillRect(surface, &pl2, white);
	SDL_FillRect(surface, &ball.ball, white);
	drawMidLine();
	// drawPoints();
}

void handleEvents(bool &gameRunning, Ball &ball){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch (event.type){
			case SDL_QUIT:
				gameRunning = false;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE) gameRunning = false;
				if(event.key.keysym.sym == SDLK_r) {
					SDL_FillRect(surface, &(ball.ball), black);
					init_ball(ball);
				}
			break;
		}
	}
}

void move_players(SDL_Rect *pl1, SDL_Rect *pl2, float deltaTime){
	// bool keysPressed[2][2]; // W, S for player 1 - UP, DOWN for player 2
	const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
	if(keyboard[W] and pl1->y > 0) {
		SDL_FillRect(surface, pl1, black);
		pl1->y -= round(PLAYER_SPEED * deltaTime);
	}
	if(keyboard[S] and pl1->y + PLAYER_HEIGHT < HEIGHT) {
		SDL_FillRect(surface, pl1, black);
		pl1->y += round(PLAYER_SPEED * deltaTime);
	}
	if(keyboard[UP] and pl2->y > 0) {
		SDL_FillRect(surface, pl2, black);
		pl2->y -= round(PLAYER_SPEED * deltaTime);
	}
	if(keyboard[DOWN] and pl2->y + PLAYER_HEIGHT < HEIGHT) {
		SDL_FillRect(surface, pl2, black);
		pl2->y += round(PLAYER_SPEED * deltaTime);
	}
	SDL_FillRect(surface, pl1, white);
	SDL_FillRect(surface, pl2, white);
}

int randomDir(void){ // Return 1 or -1
	int values[2] = {-1, 1};
	std::random_device rd;
	std::mt19937 generator(rd());
	return values[generator()%2];
}


void move_ball(Ball &ball, SDL_Rect* pl1, SDL_Rect* pl2, float deltaTime){
	SDL_FillRect(surface, &(ball.ball), black);
	if(ball.ball.y <= 0){ // Top edge collision
		ball.direction.y *= -1;
	}
	if(ball.ball.y + BALL_SIZE >= HEIGHT){ // Bottom edge collision
		ball.direction.y *= -1;
	}
	
	Vector hitPoint = {-1, -1};
	int relativeHitPointY; // The y position relative to the center of the paddle
	double deflectAngle; // Deflect angle
	// Player 1 collision
	if(ball.ball.x <= (pl1->x + PLAYER_WIDTH) and ((ball.ball.y + (BALL_SIZE/2)) >= pl1->y) and (ball.ball.y + (BALL_SIZE/2)) <= (pl1->y + PLAYER_HEIGHT)){
		hitPoint.x = ball.ball.x;
		hitPoint.y = ball.ball.y + (BALL_SIZE/2);
		relativeHitPointY = (pl1->y + (PLAYER_HEIGHT/2)) - hitPoint.y;
		deflectAngle = (double) ((double)relativeHitPointY / (double)(PLAYER_HEIGHT/2.0)) * MAX_ANGLE;
		int newDirX, newDirY;
		newDirX = int (cos(deflectAngle) * (double)BALL_SPEED);
		newDirY = int (-sin(deflectAngle) * (double)BALL_SPEED);
		ball.direction = {newDirX, newDirY};
		// normalizeVector(ball.direction, BALL_SPEED);

	}
	// Player 2 collision
	if((ball.ball.x + BALL_SIZE) >= pl2->x and ((ball.ball.y + (BALL_SIZE/2)) >= pl2->y) and (ball.ball.y + (BALL_SIZE/2)) <= (pl2->y + PLAYER_HEIGHT)){
		hitPoint.x = ball.ball.x;
		hitPoint.y = ball.ball.y + (BALL_SIZE/2);
		relativeHitPointY = (pl2->y + (PLAYER_HEIGHT/2)) - hitPoint.y;
		deflectAngle = (double) ((double)relativeHitPointY / (double)(PLAYER_HEIGHT/2.0)) * MAX_ANGLE;
		int newDirX, newDirY;
		newDirX = int (-cos(deflectAngle) * (double)BALL_SPEED);
		newDirY = int (-sin(deflectAngle) * (double)BALL_SPEED);
		ball.direction = {newDirX, newDirY};
		// normalizeVector(ball.direction, BALL_SPEED);

	}

	ball.ball.x += round(ball.direction.x * deltaTime);
	ball.ball.y += round(ball.direction.y * deltaTime);
	SDL_FillRect(surface, &(ball.ball), white);
}

void update(SDL_Rect* pl1, SDL_Rect* pl2, Ball &ball){
	float deltaTime = (SDL_GetTicks() - lastFrame) / 1000.0f;

	lastFrame=SDL_GetTicks();
	move_players(pl1, pl2, deltaTime);
	move_ball(ball, pl1, pl2, deltaTime);

	drawMidLine();

	SDL_UpdateWindowSurface(window);

	// Delay until we reach the FRAME_TIME value
	Uint32 delayTime = (Uint32) FRAME_TIME - (SDL_GetTicks() - lastFrame);

	//But only if the frames are generating too fast
	if(delayTime >= 0 and delayTime <= FRAME_TIME) SDL_Delay(delayTime);
	
}

void exit(){ // Shut down SDL subsytems and destroy the window
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::cout<<"Game finished.\n";
}
