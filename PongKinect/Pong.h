/* 

pong.h
par cynix (dark-s@evhr.net)

*/



#ifndef PONG_H
#define PONG_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "MyFreenectDevice.hpp"

#define SCREEN_W 800
#define SCREEN_H 600

#define BAR_W 30
#define BAR_H 120

#define LINE_W 2

#define BALL_X 100
#define BALL_Y 100
#define BALL_SIZE 10
#define BALL_SPEED 20

#define SCORE_D 30


void			init();
void			initPos();
bool			checkFinger(int Fx, int Fy, int Xmin, int Ymin, int Xmax, int Ymax);
void			menu(MyFreenectDevice * freenect);
void			gameLoop(MyFreenectDevice * freenect);
void			quitGame();
void			resetBall(SDL_Rect *position,int *x,int *y);


#endif
