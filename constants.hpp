#include <SDL2/SDL.h>

#define ESCAPE SDL_SCANCODE_ESCAPE
#define W SDL_SCANCODE_W
#define S SDL_SCANCODE_S
#define UP SDL_SCANCODE_UP
#define DOWN SDL_SCANCODE_DOWN
#define PI 3.14159265358979323846

// Window size
const int WIDTH = 800;
const int HEIGHT = 600;

//Player
const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 100;
const int PLAYER_SPEED = 250;

// Ball
const int BALL_SIZE = 10;
const int BALL_SPEED = 400;
const double MAX_ANGLE = (5*PI)/12; // 75º

// Colors
const Uint32 white = 0xffffffff;
const Uint32 black = 0x00000000;


// How long a frame will be rendered as a function of FPS
const int FPS = 60;
const float FRAME_TIME = (float) 1000/FPS; // 1000 ms = 1 seg
