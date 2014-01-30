
#ifndef _BLOB_DETECT
#define _BLOB_DETECT

#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <map>

///
///Documentation
///
enum FILTER_TO_USE
  {
    GRAY = 0,
    HSV = 1,
    HUE = 2,
    SAT = 3,
    VAL = 4,
    CANNY = 5,
    THRESHOLD = 6,
    LAPLACE = 7
  };
/**
* \brief
* Objectif de la classe :
* Appliquer differents filtres 
* sur une image puis trouver des 
* objets dans cette image.
**/

class BlobDetector
{
 public :
  CvSeq * getAllContoursInImage(IplImage*);
  BlobDetector(void);
  ~BlobDetector(void);
  /**
  * Fonction permettant de chercher des blobs
  * dans une image en precisant un certain filtre <br>
  * Param 1 : IplImage *, image dans laquelle nous voulons chercher nos objet (aka blobs)<br>
  *	Param 2 (Optionnel) : enum FILTER_TO_USE precise quel type de filtre nous souhaitons appliquer sur l'image<br>
  * Acces : public
  **/
  void findBlobs(IplImage *, enum FILTER_TO_USE = HUE);
  /**
  * Renvoi l'image demande sauf si elle n'existe pas renvoi 0.<br>
  * Param 1 (Optionnel) : precise l'image que nous souhaitons recuperer de base HUE <br>
  * Acces : public
  **/
  IplImage * getImage(enum FILTER_TO_USE f = HUE);
  /**
  * Renvoi le plus gros objet (ou blob) trouve dans l'image.<br>
  * Param : Aucun<br>
  * Acces : public
  **/
  CvSeq * getBiggestBlob()const;
  /**
  * Renvoi tous les blobs trouves dans l'image.<br>
  * Param : Aucun<br>
  *	Acces : public
  **/
  CvSeq * getBlobs();
  /**
  * Si un background a ete sauvegarde grace a la classe CamStreamer<br>
  * cette fonction permet de soustraire l'arriere plan a l'image courante.
  **/
  void removeBackground(IplImage *);
  /**
  * Lorsque la classe BlobDetector applique un
  * threshold sur l'image ceci permet de definir
  * les valeurs minimales et maximales que l'on souhaite recuperer.<br>
  * Thresholding : http://en.wikipedia.org/wiki/Thresholding_%28image_processing%29
  **/
  void setThresholds(int low, int high);
  /**
  * Recupere le plus gros blob dans une image passe en parametre.<br>
  * (Cf : l'image ne doit pas avoir plus de 1 seul canal de couleur
  * sinon la fonction ne fera rien).
  **/
  CvSeq * getBiggestContourInImage(IplImage *);
  /**
  * Fonction a appeler a chaque fin de boucle pour
  * liberer de la memoire ceci est par souci d'optimisation.
  **/
  void releaseImages();
  /**
  * Dessine les blobs sur l'image passe en parametre.
  **/
  void drawBlobs(IplImage * img, CvScalar color = CV_RGB(255, 0, 0));
  /**
  * Dessine le plus gros blob a l'ecran.
  **/
  void drawBiggestBlob(IplImage * img, CvScalar color = CV_RGB(255, 0, 0));
 private :
	 /**
	 * Fonction qui applique le thresholding de l'image,
	 * un filtre gaussien(voir wikipedia), un dilatation<br>
	 * et une erodation de l'image. Puis trouve les blobs.
	 **/
  void applyCannyEdgeDetector();
  /**
  * Methode appele par "applyCannyEdgeDetector()"
  * pour effectuer le thresholding.
  **/
  void applyThresholding(IplImage *, int, int, int);
  
  /**
  * Trouve le plus gros blob dans l'image.
  **/
  CvSeq * findBiggestBlob();

  /**
  * Trouve tous les blobs dans l'image.
  **/
  void findContours();

  /**
  * Verifie si les images ont ete alloue en memoire,
  * si non les alloue,<br> cette operation ne s'effectue qu'une seule fois
  * pour ne pas gaspier de memoire.
  **/
  bool imagesCreated();
  /**
  * Methode appele si imagesCreated renvoi false.
  **/
  void createImages(IplImage *);


  /**
  * Applique un filtre gris sur l'image.
  **/
  IplImage * treatGRAYImage(IplImage * = 0);
  /**
  * Applique un filtre HSV sur l'image.
  **/
  IplImage * treatHSVImage(IplImage * = 0);
  /**
  * Applique un filtre HSV sur l'image puis recupere le canal HUE
  * en separant les 3 canaux de l'image HSV.
  **/
  IplImage * treatHUEImage(IplImage * = 0);
  /**
  * Applique un filtre HSV sur l'image puis recupere le canal saturation
  * en separant les 3 canaux de l'image HSV.
  **/
  IplImage * treatSATImage(IplImage * = 0);
  /**
  * Applique un filtre HSV sur l'image puis recupere le canal valeurs
  * en separant les 3 canaux de l'image HSV.
  **/
  IplImage * treatVALImage(IplImage * = 0);

  /**
  * Assigne l'image passe en parametre a l'image gris.<br>
  * cf : la variable filteredBasic.
  **/
  void setGRAY(IplImage*);
  /**
  * Assigne l'image passe en parametre a l'image HSV.<br>
  * cf : la variable hsvFilter.
  **/
  void setHSV(IplImage*);
  /**
  * Assigne l'image passe en parametre a l'image HUE.<br>
  * cf : hueFilter.
  **/
  void setHUE(IplImage*);
  /**
  * Assigne l'image passe en parametre a l'image Saturation.<br>
  * cf : saturationFilter.
  **/
  void setSAT(IplImage*);
  /**
  * Assigne l'image passe en parametre a l'image Valeur.<br>
  * cf : valueFilter.
  **/
  void setVAL(IplImage*);

  /**
  * Recupere l'image filteredBasic (gris).
  **/
  IplImage * getGRAY()const;
  /**
  * Recupere l'image hsvFilter (hsv).
  **/
  IplImage * getHSV()const;
  /**
  * Recupere l'image hueFilter (HUE).
  **/
  IplImage * getHUE()const;
  /**
  * Recupere l'image saturationFilter (saturation).
  **/
  IplImage * getSAT()const;
  /**
  * Recupere l'image valueFilter (valeur).
  **/
  IplImage * getVAL()const;
  /**
  * Recupere l'image cannyImage (canny edge detector).
  **/
  IplImage * getCANNY()const;
  /**
  * Recupere l'image threshImage (thresholded Image).
  **/
  IplImage * getTHRESH()const;
  /**
  * Recupere l'image laplaceImage (image avec application d'un algorithme laplacien).
  **/
  IplImage * getLAPLACE()const;

  /**
  * Applique un filtre gaussien a l'image.<br>
  * Utilite : reduction du bruit.
  **/
  void gaussianSmooth();
  /**
  * Effectue une erodation de l'image.<br>
  * Utilite : reduction du bruit.
  **/
  void erode();
  /**
  * Effectue un dilatation de l'image.<br>
  * Utilite : reduction du bruit.
  **/
  void dilate();

  IplImage * camStream;
  IplImage * filteredBasic;
  IplImage * hsvFilter;
  IplImage * hueFilter;
  IplImage * saturationFilter;
  IplImage * valueFilter;
  IplImage * cannyImage;
  IplImage * threshImage;
  IplImage * laplaceImage;

  std::map<enum FILTER_TO_USE, IplImage * (BlobDetector::*)(IplImage*)> filterFunctions;
  std::map<enum FILTER_TO_USE, IplImage* (BlobDetector::*)()const> getImages;
  std::map<enum FILTER_TO_USE, void (BlobDetector::*)(IplImage*)> setImages;
  CvSeq * contours;
  CvMemStorage * storage;
  CvSeq * biggestBlob;

  int highThresh, lowThresh;

  enum FILTER_TO_USE filter;
};


#endif
