
#include "BlobDetector.h"

BlobDetector::BlobDetector(void)
{
	camStream = 0;
	filteredBasic = 0;
	hsvFilter = 0;
	hueFilter = 0;
	saturationFilter = 0;
	valueFilter = 0;
	cannyImage = 0;
    contours = 0;
	threshImage = 0;

	lowThresh = 100;
	highThresh = 255;


	filterFunctions[GRAY] = &BlobDetector::treatGRAYImage;
	filterFunctions[HSV] = &BlobDetector::treatHSVImage;
	filterFunctions[HUE] = &BlobDetector::treatHUEImage;
	filterFunctions[SAT] = &BlobDetector::treatSATImage;
	filterFunctions[VAL] = &BlobDetector::treatVALImage;

	getImages[GRAY] = &BlobDetector::getGRAY;
	getImages[HSV] = &BlobDetector::getHSV;
	getImages[HUE] = &BlobDetector::getHUE;
	getImages[SAT] = &BlobDetector::getSAT;
	getImages[VAL] = &BlobDetector::getVAL;
	getImages[CANNY] = &BlobDetector::getCANNY;
	getImages[THRESHOLD] = &BlobDetector::getTHRESH;
	
	setImages[GRAY] = &BlobDetector::setGRAY;
	setImages[HSV] = &BlobDetector::setHSV;
	setImages[HUE] = &BlobDetector::setHUE;
	setImages[SAT] = &BlobDetector::setSAT;
	setImages[VAL] = &BlobDetector::setVAL;
	storage = cvCreateMemStorage();
}

void BlobDetector::drawBlobs(IplImage * img, CvScalar color)
{
	if (img)
	{
		if (contours)
		{
			cvDrawContours(img, contours, color, color, 1, 3);
		}
	}
}

void BlobDetector::drawBiggestBlob(IplImage * img, CvScalar color)
{
	if (img)
	{
		if (biggestBlob)
		{
			cvDrawContours(img, biggestBlob, color, color, 1, 3);
		}
	}
}

void BlobDetector::setThresholds(int low, int high)
{
	lowThresh = low;
	highThresh = high;
}

void BlobDetector::setGRAY(IplImage *img)
{
	if (!filteredBasic)
		filteredBasic = cvCreateImage(cvGetSize(img), 8, 1);
	if (img->nChannels > 1)
	{
		cvCvtColor(img, filteredBasic, CV_RGB2GRAY);
	}
	else
	{
		cvCopy(img, filteredBasic);
	}
}

void BlobDetector::setHSV(IplImage *img)
{
	if (!hsvFilter)
		hsvFilter = cvCreateImage(cvGetSize(img), 8, 3);
//	hsvFilter = img;
	cvCopy(img, hsvFilter);
}

void BlobDetector::setHUE(IplImage *img)
{
//	hueFilter = img;
	cvCopy(img, hueFilter);
}

void BlobDetector::setSAT(IplImage *img)
{
	cvCopy(img, saturationFilter);
}

void BlobDetector::setVAL(IplImage *img)
{
	cvCopy(img, valueFilter);
}

IplImage * BlobDetector::getGRAY()const
{
	return filteredBasic;
}

IplImage * BlobDetector::getHSV()const
{
	return hsvFilter;
}

IplImage * BlobDetector::getHUE()const
{
	return hueFilter;
}

IplImage * BlobDetector::getSAT()const
{
	return saturationFilter;
}

IplImage * BlobDetector::getVAL()const
{
	return valueFilter;
}

IplImage * BlobDetector::getCANNY()const
{
	return cannyImage;
}

IplImage * BlobDetector::getTHRESH()const
{
	return threshImage;
}

IplImage * BlobDetector::getLAPLACE()const
{
    return laplaceImage;
}

BlobDetector::~BlobDetector(void)
{
}

void BlobDetector::releaseImages()
{
  cvClearMemStorage(storage);
  cvReleaseMemStorage(&storage);
}

void BlobDetector::findBlobs(IplImage * cam, enum FILTER_TO_USE f)
{
  filter = f;
  if (!cam)
    {
      std::cerr << "Invalid image end of execution" << std::endl;
      return;
    }
  if (!imagesCreated())
    createImages(cam);
  cvCopy(cam, camStream);
  storage = cvCreateMemStorage(0);
  if (filter >= 0 && filter <= 4)
    {
      (this->*filterFunctions[filter])(0);
      applyCannyEdgeDetector();
      findContours();
    }
  else
    {
      std::cerr << "Error : invalid filter, using gray scale filter" << std::endl;
      (this->*filterFunctions[GRAY])(0);
      filter = GRAY;
      applyCannyEdgeDetector();
      findContours();
    }
}

bool BlobDetector::imagesCreated()
{
  if (!camStream
      || !filteredBasic 
      || !hsvFilter 
      || !hueFilter
      || !saturationFilter
      || !valueFilter)
    return false;
  return true;
}

void BlobDetector::createImages(IplImage * original)
{
  camStream = cvCreateImage(cvGetSize(original), original->depth, original->nChannels);
  filteredBasic = cvCreateImage(cvGetSize(original), 8, 1);
  hsvFilter = cvCreateImage(cvGetSize(original), 8, original->nChannels);
  hueFilter = cvCreateImage(cvGetSize(camStream), 8, 1);
  saturationFilter = cvCreateImage(cvGetSize(camStream), 8, 1);
  valueFilter = cvCreateImage(cvGetSize(camStream), 8, 1);
}

IplImage * BlobDetector::treatGRAYImage(IplImage * img)
{
  if (!img)
    {
      cvCvtColor(camStream, filteredBasic, CV_RGB2GRAY);
      return 0;
    }
  IplImage * tmp = cvCreateImage(cvGetSize(img), 8, 1);
  cvCvtColor(img, tmp, CV_RGB2GRAY);
  cvReleaseImage(&tmp);
  return tmp;
}

IplImage * BlobDetector::treatHSVImage(IplImage * img)
{
  if (!img)
    {
      cvCvtColor(camStream, hsvFilter, CV_RGB2HSV);
      return 0;
    }
  IplImage * tmp = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
  cvCvtColor(img, tmp, CV_RGB2HSV);
  return tmp;
}

IplImage * BlobDetector::treatHUEImage(IplImage * img)
{
  IplImage * tmp = treatHSVImage(img);

  if (!tmp)
    {
      cvSplit(hsvFilter, hueFilter, 0, 0, 0);
      return 0;
    }
  IplImage * tmp2 = cvCreateImage(cvGetSize(img), 8, 1);
  cvSplit(tmp, tmp2, 0, 0, 0);
  cvReleaseImage(&tmp);
  return tmp2;

}

IplImage * BlobDetector::treatSATImage(IplImage * img)
{
  IplImage * tmp = treatHSVImage(img);
  if (!tmp)
    {
      cvSplit(hsvFilter, 0, saturationFilter, 0, 0);
      return 0;
    }
  IplImage * tmp2 = cvCreateImage(cvGetSize(img), 8, 1);
  cvSplit(tmp, 0, tmp2, 0, 0);
  cvReleaseImage(&tmp);
  return tmp2;
}

IplImage * BlobDetector::treatVALImage(IplImage * img)
{
  IplImage * tmp = treatHSVImage(img);
  if (!tmp)
    {
      cvSplit(hsvFilter, 0, 0, valueFilter, 0);
      return 0;
    }
  IplImage * tmp2 = cvCreateImage(cvGetSize(img), 8, 1);
  cvSplit(tmp, 0, 0, tmp2, 0);
  cvReleaseImage(&tmp);
  return tmp2;
}

void BlobDetector::applyThresholding(IplImage * img, int lowThresh, int highThresh, int mode)
{
  if (!threshImage)
    threshImage = cvCreateImage(cvGetSize(img), 8, 1);
  if (img->nChannels == 1)
    {
      cvThreshold(img, threshImage, lowThresh, highThresh, mode);
    }
  else
    {
      cvReleaseImage(&threshImage);
    }
}


IplImage * BlobDetector::getImage(enum FILTER_TO_USE f)
{
  return (this->*getImages[f])();
}

void BlobDetector::applyCannyEdgeDetector()
{
  if (!cannyImage)
    cannyImage = cvCreateImage(cvGetSize(camStream), 8, 1);
  dilate();
  erode();
  erode();
  dilate();
  applyThresholding((this->*getImages[filter])(), lowThresh, highThresh, CV_THRESH_BINARY);
  //cvCanny(threshImage, cannyImage, 1, 2);
}

void BlobDetector::dilate()
{
  IplImage * tmp;
  tmp = cvCreateImage(cvGetSize((this->*getImages[filter])()), (this->*getImages[filter])()->depth, (this->*getImages[filter])()->nChannels);
  cvDilate((this->*getImages[filter])(), tmp);
  (this->*setImages[filter])(tmp);
  cvReleaseImage(&tmp);
}

void BlobDetector::erode()
{
  IplImage * tmp;
  tmp = cvCreateImage(cvGetSize((this->*getImages[filter])()), (this->*getImages[filter])()->depth, (this->*getImages[filter])()->nChannels);
  cvErode((this->*getImages[filter])(), tmp);
  (this->*setImages[filter])(tmp);
  cvReleaseImage(&tmp);
}

void BlobDetector::gaussianSmooth()
{
  IplImage * tmp;
  tmp = cvCreateImage(cvGetSize((this->*getImages[filter])()), (this->*getImages[filter])()->depth, (this->*getImages[filter])()->nChannels);
  cvSmooth((this->*getImages[filter])(), tmp, 7, 7);
  (this->*setImages[filter])(tmp);
  cvReleaseImage(&tmp);
}

CvSeq * BlobDetector::getBiggestBlob()
{
  return findBiggestBlob();
}

CvSeq * BlobDetector::findBiggestBlob()
{
  if (contours)
    {
      CvSeq * biggest = 0;
      CvSeq * save = contours;
      CvRect max;
      //  int max = 0;
      max.width = 0;
      max.height = 0;
      while (save)
        {
	  CvRect area = cvBoundingRect(save, 1);
	  if (area.height * area.width > max.height * max.width)
            {
	      biggest = save;
	      max = area;
            }
	  save = save->h_next;
        }
      if (biggest)
        {
	  biggest->h_next = 0;
	  biggest->v_next = 0;
	  biggest->h_prev = 0;
	  biggest->v_prev = 0;
	  biggest = cvApproxPoly(biggest, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 15);
        }
      return biggest;
    }
  return 0;
}

void BlobDetector::removeBackground(IplImage * background)
{
	IplImage * filteredBg = (this->*filterFunctions[filter])(background);
	IplImage * tmp;
	tmp = cvCreateImage(cvGetSize((this->*getImages[filter])()), 8, 1);
	applyThresholding((this->*getImages[filter])(), lowThresh, highThresh, CV_THRESH_BINARY);
	cvThreshold(filteredBg, tmp, lowThresh, highThresh, CV_THRESH_BINARY);
	cvSub(tmp, (this->*getImages[filter])(), tmp, tmp);
	cvErode(tmp, tmp);
	cvDilate(tmp, tmp);
	cvDilate(tmp, tmp);
	cvErode(tmp, tmp);
	cvThreshold(tmp, tmp, 50, 150, CV_THRESH_BINARY);
	(this->*setImages[filter])(tmp);
	cvReleaseImage(&tmp);
}

void BlobDetector::findContours()
{
  if (cannyImage)
    {
      cvFindContours(threshImage, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    }
}

CvSeq * BlobDetector::getBiggestContourInImage(IplImage * img)
{
  CvSeq * _contours = 0;

  cvFindContours(img, storage, &_contours, 88, CV_RETR_EXTERNAL);
  CvSeq * biggest = 0;
  CvRect max;
  max.height = 0;
  max.width = 0;
  while (_contours)
    {
      CvRect area = cvBoundingRect(_contours);
      
      if (area.height * area.width > max.width * max.height)
	{
	  biggest = _contours;
	  max = area;
	}
      _contours = _contours->h_next;
    }
  if (biggest)
    {
      biggest->h_next = 0;
      biggest->v_next = 0;
      biggest->h_prev = 0;
      biggest->v_prev = 0;
    }
  return biggest;
}

CvSeq * BlobDetector::getBlobs()
{
  return contours;
}

CvSeq * BlobDetector::getAllContoursInImage(IplImage * img)
{
  if (img  && img->nChannels == 1)
    {
      CvSeq * c = 0;
      CvMemStorage * mem = cvCreateMemStorage(0);
      cvFindContours(img, mem, &c, 88, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
      return c;
    }
  return 0;
}
