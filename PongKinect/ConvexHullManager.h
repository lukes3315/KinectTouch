//
//  ConvexHullManager.h
//  TEST
//
//  Created by Lucas McKenna on 1/27/12.
//  Copyright 2012 Epitech. All rights reserved.
//

#ifndef TEST_ConvexHullManager_h
#define TEST_ConvexHullManager_h

#include <iostream>
#include <cv.h>
#include <highgui.h>

/**
*\brief
* Objectif de la classe: 
* Calculer l'enveloppe convexe 
* (plus simplement le polygone defini par la forme de la main).<br>
* Explication de l'algo : http://www.maa.org/joma/Volume8/Kalman/ConvexHull.html <br>
**/

/**
* Autres liens :<br> 
* http://www.ahristov.com/tutorial/geometry-games/convex-hull.html <br>
* http://www.cse.unsw.edu.au/~lambert/java/3d/hull.html
**/

class ConvexHullManager
{
 public :
  /**
   * Constructeur 1 : prend en parametre un contour.
   **/
  ConvexHullManager(CvContour *contour);
  /**
   * Constructeur 2 : ne prend pas de parametres.
   * <br> le contour est donc intialise a 0.
   **/
  ConvexHullManager();
  ~ConvexHullManager();
  /**
   * Si le contours n'est pas initialise il est automatiquement mis a zero,
   * <br>cette fonction permet de l'initialiser
   **/
  void setContours(CvContour *);
  /**
   * Permet de recuperer le contour qui lui a ete passe en parametre.
   **/
  CvContour * getContours()const;
  /**
   * Permet de calculer l'enveloppe<br>
   * convexe du contour qu'on lui a passe en parametre.<br>
   * La methode ne marchera pas si setContour() n'a pas ete utilise avant<br>
   * Explication de l'algo :<br>
   * -	http://www.ahristov.com/tutorial/geometry-games/convex-hull.html <br>
   * - http://www.cse.unsw.edu.au/~lambert/java/3d/hull.html <br>
   * -  http://www.maa.org/joma/Volume8/Kalman/ConvexHull.html
   **/
  void calcConvexity();
  void calcConvexityDefect(CvMat);
  /**
   * Methode recuperant l'enveloppe convexe<br>
   * sous forme d'un tableau de point
   **/
  CvPoint * getConvexityPoints()const;
  /**
   * Litteralement cette fonction recupere la taille<br>
   * du tableau de point recupere dans la fonction getConvexityPoints().
   **/
  int getHullSize()const;
  /**
   * Dessin les points convexe sur l'image passe en parametre.<br>
   * La methode ne marchera pas si calcConvexity() n'a pas ete appele avant.
   **/
  void drawConvexity(IplImage *, CvScalar color = CV_RGB(255, 0, 0), int lineSize = 1);
  void drawConvexityDefect(IplImage *);
  /**
   * Recupere l'enveloppe convexe sous forme de CvSeq (type opencv).
   **/
  CvSeq * getConvexityContour()const;
  void releaseArrays();
 private :
  int hullsize;
  int * hull;
  CvPoint * PointArray;
  CvContour * contours;
  CvPoint * convexityPoints;
  CvMemStorage * storage;
  CvMemStorage * defectStorage;
  CvSeq * defects;
  CvSeq * convexityContour;
  CvConvexityDefect * defectArray;
};

#endif
