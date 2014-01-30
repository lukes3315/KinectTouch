/* 

pong.c 
par cynix (dark-s@evhr.net)

*/

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <list>
#include "Pong.h"

bool P1=false;
bool P2 = false;
int alive = 1;
int enter = 1;
int GameMode = 1;
int xBall=0,yBall=0;
char scoreP1_s[10];
char scoreP2_s[10];
int scoreP1=0,scoreP2=0;
Uint8 *keystates = SDL_GetKeyState(NULL);
SDL_Surface *screen = NULL, *bar = NULL,*bar2 = NULL, *ball=NULL,*ligne=NULL, *selbar=NULL, *test=NULL;
SDL_Rect posBar1,posBar2,posBall,posLigne,posScore1,posScore2,posQuit,pos1p, pos2p, pospong, poselect, posplay, posselbar, posTest;
SDL_Event event;
TTF_Font *font, *font1, *font2;
SDL_Color couleurScore;
SDL_Surface *score1=NULL, *score2=NULL, *quit=NULL, *p1=NULL, *p2=NULL, *pong=NULL, *sel=NULL, *play=NULL;

void			init()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_WM_SetCaption("Pong", NULL);
  putenv("SDL_VIDEO_WINDOW_POS=0,1050");
  screen = SDL_SetVideoMode(SCREEN_W,SCREEN_H,32,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_NOFRAME);
  ligne = SDL_CreateRGBSurface(SDL_HWSURFACE,LINE_W,SCREEN_H,32,0,0,0,0);
  SDL_FillRect(ligne,NULL,SDL_MapRGB(ligne->format,100,100,100));
  bar = SDL_CreateRGBSurface(SDL_HWSURFACE,BAR_W,BAR_H,32,0,0,0,0);
  SDL_FillRect(bar,NULL,SDL_MapRGB(bar->format,255,255,255));

  bar2 = SDL_CreateRGBSurface(SDL_HWSURFACE,BAR_W,BAR_H,32,0,0,0,0);
  SDL_FillRect(bar2,NULL,SDL_MapRGB(bar->format,255,255,255));

  selbar = SDL_CreateRGBSurface(SDL_HWSURFACE,420,75,32,0,0,0,0);
  SDL_FillRect(selbar,NULL,SDL_MapRGB(bar->format,34,139,34));
  ball = SDL_CreateRGBSurface(SDL_HWSURFACE,BALL_SIZE,BALL_SIZE,32,0,0,0,0);
  SDL_FillRect(ball,NULL,SDL_MapRGB(ball->format,255,255,255));
  test = SDL_CreateRGBSurface(SDL_HWSURFACE,BALL_SIZE,BALL_SIZE,32,0,0,0,0);
  SDL_FillRect(test,NULL,SDL_MapRGB(test->format,255,0,0));

  TTF_Init();
  font = TTF_OpenFont("Alexis Expanded.ttf", 60);
  font1 = TTF_OpenFont("Alexis Expanded.ttf", 50);
  font2 = TTF_OpenFont("Alexis Expanded.ttf", 80);
  couleurScore.r = 255;
  couleurScore.g = 255;
  couleurScore.b = 255;
}

void			initPos()
{
  posScore1.x = SCREEN_W/2 - 100 ;

  posScore1.y = SCORE_D ;
  posScore2.x = SCREEN_W/2 + 100 ;
  posScore2.y = SCORE_D ;
  posQuit.x = SCREEN_W/2;
  posQuit.y = SCORE_D + 520;
  poselect.x = SCREEN_W/2 - 350;
  poselect.y = SCREEN_H/2 - 100;
  pospong.x = SCREEN_W/2 - 400;
  pospong.y = SCREEN_H/2 - 200;
  pos1p.x = SCREEN_W/2 - 200;
  pos1p.y = SCREEN_H/2 - 30;
  pos2p.x = SCREEN_W/2 - 200;
  pos2p.y = SCREEN_H/2 + 50;
  posplay.x = SCREEN_W/2 - 200;
  posplay.y = SCREEN_H/2 + 200;
  SDL_EnableKeyRepeat(10, 50);
  posBar1.x = 50;
  posBar1.y = SCREEN_H/2 - BAR_H/2;
  posBar2.x = SCREEN_W - 50 - BAR_W;
  posBar2.y = SCREEN_H/2 - BAR_H/2;
  posselbar.x = SCREEN_W/2 - 210;
  posselbar.y = SCREEN_H/2 - 55;

  posTest.x = posBar1.x + BAR_W;
  posTest.y =  posBar1.y + BAR_H;

  resetBall(&posBall,&xBall,&yBall);
  posLigne.x = SCREEN_W/2 - LINE_W/2;
  posLigne.y = 0;


  quit = TTF_RenderText_Solid(font, "QUIT", couleurScore);
  p1 = TTF_RenderText_Solid(font1, " 1  Player", couleurScore);

  p2 = TTF_RenderText_Solid(font1, "2  Players", couleurScore);
  pong = TTF_RenderText_Solid(font2, "  D I G I P O N G", couleurScore);

  play = TTF_RenderText_Solid(font2, "  PLAY", couleurScore);

}

bool			checkFinger(int Fx, int Fy, int Xmin, int Ymin, int Xmax, int Ymax)
{
  if (Fx >= Xmin && Fx <= Xmax && Fy >= Ymin && Fy <= Ymax)
    return true;
  return false;
}

void			menu(MyFreenectDevice * freenect)
{
  while (enter && alive)
    {
      SDL_PumpEvents();
      if (freenect->isDepthReady())
	{
	  freenect->calcFingerPositions();
	  std::list<Finger> flist = freenect->getFingerList();
	 
	  for (std::list<Finger>::iterator it = flist.begin() ; it != flist.end() ; ++it)
	    {
	      // BOUCLE DE PARCOURS DES POINTS DES DOIGTS
	      if(keystates[SDLK_RETURN]
		 || checkFinger(it->x, it->y, 
				SCREEN_W/2 - 150, 
				SCREEN_H/2 + 190,
				SCREEN_W /2 + 130,
				SCREEN_H/2 + 240))  // SELECTION DE PLAY
		{
		  /* XMIN : SCREEN_W /2 - 150;*/
		  /* YMIN : SCREEN_H/2 + 190*/
		  /* XMAX : SCREEN_W /2 + 130*/
		  /* YMAX :  SCREEN_H/2 + 240*/
		  enter=0;
		  break;
		}
	      if(keystates[SDLK_ESCAPE])
		alive=0;
		 if(keystates[SDLK_DOWN] || checkFinger(it->x, it->y, SCREEN_W /2 - 210,
							SCREEN_H/2 + 20,
							SCREEN_W /2 + 200,
							SCREEN_H/2 + 95)) // SELECTION DE 2 JOUEUR
		{
		  /* XMIN : SCREEN_W /2 - 210;*/
		  /* YMIN : SCREEN_H/2 + 20 */
		  /* XMAX : SCREEN_W /2 + 200 */
		  /* YMAX : SCREEN_H/2 + 95*/
		  posselbar.y = SCREEN_H/2 + 25;
		  GameMode = 2;
		  break;
		}
		 if(keystates[SDLK_UP] || checkFinger(it->x, it->y, SCREEN_W /2 - 210,
						      SCREEN_H/2 - BAR_H/2,
						      SCREEN_W /2 + 200,
						      SCREEN_H/2 + 10)) // SELECTION DE 1 JOUEUR
		{
		  /* XMIN : SCREEN_W /2 - 210*/
		  /* YMIN : SCREEN_H/2 - BAR_H/2 */
		  /* XMAX : SCREEN_W /2 + 200 */
		  /* YMAX : SCREEN_H/2 + 10 */
		  posselbar.y = SCREEN_H/2 - 55;
		  GameMode = 1;
		  break;
		}
	    }
	}
      SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
      SDL_BlitSurface(selbar,NULL,screen,&posselbar);
      SDL_BlitSurface(pong,NULL,screen,&pospong);
      SDL_BlitSurface(p1,NULL,screen,&pos1p);
      SDL_BlitSurface(p2,NULL,screen,&pos2p);
      SDL_BlitSurface(play,NULL,screen,&posplay);
      SDL_BlitSurface(sel,NULL,screen,&poselect);     
      SDL_Flip(screen);
      SDL_Delay(10);
      freenect->update();
    }
}

void			gameLoop(MyFreenectDevice * freenect)
{
  while(alive)
    {  
      /* Mise a jour du score */
      sprintf(scoreP1_s,"%d",scoreP1);
      sprintf(scoreP2_s,"%d",scoreP2); 
      score1 = TTF_RenderText_Solid(font, scoreP1_s, couleurScore);
      score2 = TTF_RenderText_Solid(font, scoreP2_s, couleurScore);
 
      /* Rebond sur les bords de l'ecran */
      if(posBall.y <=0)
	yBall = BALL_SPEED;
      if(posBall.y >=SCREEN_H)
	yBall = -BALL_SPEED;
           
      /* Si victoire d'un joueur */
      if(posBall.x <=0)
	{
	  resetBall(&posBall,&xBall,&yBall);
	  scoreP2++;
	}
      if(posBall.x + BALL_SIZE >SCREEN_W)
	{
	  resetBall(&posBall,&xBall,&yBall);
	  scoreP1++;
	}
      
      /* Collision avec la barre de gauche */
      if(posBall.x <= posBar1.x+BAR_W &&  posBall.x > posBar1.x)
	{
	  //SDL_FillRect(bar,NULL,SDL_MapRGB(bar->format,255,25,25));
	  if(posBall.y >= posBar1.y && posBall.y <= posBar1.y + BAR_H)
	    xBall = BALL_SPEED;
	  
	}
      /* Collision avec la barre de droite */
      else if(posBall.x >= posBar2.x &&  posBall.x < posBar2.x + BAR_W/2)
	{
	  // SDL_FillRect(bar,NULL,SDL_MapRGB(bar->format,255,25,25));
	  if(posBall.y >= posBar2.y && posBall.y <= posBar2.y + BAR_H)
	    xBall = -BALL_SPEED;
	  
	}
      else
	//SDL_FillRect(bar,NULL,SDL_MapRGB(bar->format,255,255,255));
      SDL_PollEvent(&event);
      switch(event.type)
	{
	case SDL_QUIT:
	  alive = 0;
	  break;
	}
      
      /* Gestion du clavier */
      SDL_PumpEvents();
      if(keystates[SDLK_ESCAPE])    // QUIT
	{
	  /* XMIN : SCREEN_W/2*/
          /* YMIN : SCREEN_H/2 + 240*/
          /* XMAX : SCREEN_W/2 + 160*/
          /* YMAX : SCREEN_H/2 + 280*/
	  alive=0;
	} 
      if(keystates[SDLK_q])
	if(posBar1.y>=5)
	  posBar1.y -=5;
      if(keystates[SDLK_a])
	if(posBar1.y<=SCREEN_H -BAR_H -5)
	  posBar1.y +=5;
      if(keystates[SDLK_p])
	if(posBar2.y>=5)
	  posBar2.y -=5;
      if(keystates[SDLK_l])
	if(posBar2.y<=SCREEN_H -BAR_H-5)
	  posBar2.y +=5;
      /*   IA   */
      if (GameMode == 1)
	{
	  // if(posBar2.y + 60 > posBall.y)
	  //   posBar2.y -=10;
	  // else if (posBar2.y + 60 < posBall.y)
	  //   if(posBar2.y<=SCREEN_H -BAR_H-5)
	  //     posBar2.y += 5;
	  posBar2.y = posBall.y - BAR_H;
	}
      
      posBall.x += xBall;
      posBall.y += yBall;
      
      posScore1.w = score1->w;
      posScore1.h = score1->h;
      
      posScore2.w = score2->w;
      posScore2.h = score2->h;
      
      if (freenect->isDepthReady())
      	{
      	  freenect->calcFingerPositions();
      	  std::list<Finger> flist = freenect->getFingerList();
      	 
      	  for (std::list<Finger>::iterator it = flist.begin() ; it != flist.end() ; ++it)
      	    {
      	      if (checkFinger(it->x, it->y, posBar1.x-30, posBar1.y-30, posBar1.x + BAR_W+30, posBar1.y + BAR_H+ 30))
      		{
      		  SDL_FillRect(bar,NULL,SDL_MapRGB(bar->format,255,0,0));
		  posBar1.y = it->y - BAR_H/2;
      		  break;
      		}
      	      else
      		SDL_FillRect(bar,NULL,SDL_MapRGB(bar->format,255,255,255));

      	      if (checkFinger(it->x, it->y, posBar2.x - 30, posBar2.y -30, posBar2.x + BAR_W + 30, posBar2.y + BAR_H+30))
      		{
      		  SDL_FillRect(bar2,NULL,SDL_MapRGB(bar->format,0,0,255));
		  posBar2.y = it->y - BAR_H/2;
      		  break;
      		}
      	      else
      		SDL_FillRect(bar2,NULL,SDL_MapRGB(bar->format,255,255,255));

	      if (checkFinger(it->x, it->y, SCREEN_W/2, SCREEN_H/2 + 240, SCREEN_W/2 + 160, SCREEN_H/2 + 280))
		{
		  /* XMIN : SCREEN_W/2*/
		  /* YMIN : SCREEN_H/2 + 240*/
		  /* XMAX : SCREEN_W/2 + 160*/
		  /* YMAX : SCREEN_H/2 + 280*/
		  alive=0;
		}
      	    }
      	}
      //Affichage des surfaces
      SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
      
      SDL_BlitSurface(ligne,NULL,screen,&posLigne);
      
      SDL_BlitSurface(ball,NULL,screen,&posBall);
      SDL_BlitSurface(bar,NULL,screen,&posBar1);
      SDL_BlitSurface(bar2,NULL,screen,&posBar2);
      
      SDL_BlitSurface(score1,NULL,screen,&posScore1);
      SDL_BlitSurface(score2,NULL,screen,&posScore2);
      SDL_BlitSurface(quit,NULL,screen,&posQuit); 
      SDL_Flip(screen);
      SDL_Delay(10);
      freenect->update();
    }
}

void			quitGame()
{
  SDL_FreeSurface(score1);
  SDL_FreeSurface(score2);
  SDL_FreeSurface(quit);
  SDL_FreeSurface(ligne);
  SDL_FreeSurface(ball);
  TTF_CloseFont(font);
  SDL_FreeSurface(screen);
  SDL_Quit();
}

void resetBall(SDL_Rect *position,int *x,int *y)
{
  position->x  = BALL_X;
  position->y = BALL_Y;
  *x=BALL_SPEED;
  *y=BALL_SPEED;
}
