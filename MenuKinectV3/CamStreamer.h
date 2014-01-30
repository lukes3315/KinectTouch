#ifndef CAMSTREAMER_H
#define CAMSTREAMER_H

#include <cv.h>
#include <highgui.h>
#include <list>

/*
Classe : CamStreamer
- Contructeur:
	- Prototype : CamStreamer(char*,int,bool)
		- Param 1 : nom de la fenetre(de base "raw")
		- Param 2 : index de la camera (a n'utiliser que s'il y a plusieurs camera)(de base a 0)
		- Param 3 : bool signifiant si la camera stream ou non(de base a false)

- Fonctions : 
	getStream() : retourne la derniere frame recupere
		- Prototype : IplImage * getStream()const
		- Parametres : aucun
	startStream() : initialise le flux, la fenetre et commence a streamer
		- Prototype : void startStream()
		- Parametres : aucun
	fetchFrame() : recupere la frame courante de la webcam et la stock dans camStream
		- Prototype : void fetchFrame()
		- Parametres : aucun
	stopStream() : fonction appeler a la fin du programme liberant la memoire
		- Prototype : void stopStream()
		- Parametres : aucun
	displayImage() : fonction affichant un image dans un fenetre(si aucun parametre n'est passe l'affiche dans la fenetre "Raw")
		- Prototype : void displayImage(IplImage *, char *windowname)
		- Parametres : IplImage * : l'image a afficher(si rien n'est passe prend le stream de la webcam)
						char* : valable uniquement si le 1er parametre est passe, est le nom de la fenetre ou l'image est affiche, si la fentre n'existe pas elle est cree
	isRunning() : determine si le flux est toujours ouvert ou non
		- Prototype : bool isRunning()const
		- Parametres : aucun
	getKey() : renvoi la touche appuye(le parametre est optionnel, c'est l'interval en millisecondes de verification des touches appuye)
		- Prototype : int getKey(int timestamp)const
		- Parametres : timestamp automatiquement initialise a 2, verifie toutes les n millisecondes si une touche a ete appuye

- Variables :
	capture : permettant de streamer de la webcam
		- type : CvCapture *
	camStream : image contenant le flux courant de la webcam
		- type : IplImage *
	windName : nom de la fenetre (de base "raw")
		- type : char *
	camIndex : index de la cam ( de base 0)
		- type : int
	isStreaming : true/false si la webcam est en train de streamer ou non
		- type : bool
*/

class CamStreamer
{
public :
	CamStreamer(char * windowName = "Raw", int index = 0, bool isStream = false);
	~CamStreamer();
	IplImage * getStream()const;
	void startStream();
	void fetchFrame();
	void stopStream();
	void displayImage(IplImage * = NULL, char * windowName = "");
	int getCamIndex()const;
	char * getWindowName()const;
	void setWindowName(char *);
	bool isRunning()const;
	int getKey(int timestamp = 2)const;
private :
	CvCapture * capture;
	IplImage * camStream;
	char * windName;
	int camIndex;
	bool isStreaming;
    std::list<char*> windowList;
    bool windowExists(char*);
};

#endif
