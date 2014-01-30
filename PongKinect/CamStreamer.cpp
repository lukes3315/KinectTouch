#include <iostream>
#include "CamStreamer.h"

CamStreamer::CamStreamer(char * windowName, int index, bool isStream) : windName(windowName),
		camIndex(index), isStreaming(isStream)
{
    windowList.push_back(windName);
}

CamStreamer::~CamStreamer()
{}

IplImage * CamStreamer::getStream()const
{
	return camStream;
}

int CamStreamer::getCamIndex()const
{
	return camIndex;
}

char * CamStreamer::getWindowName()const
{
	return windName;
}

void CamStreamer::setWindowName(char *newWindName)
{
	if (newWindName)
    {
        std::list<char*>::iterator it = windowList.begin();
        while (it != windowList.end())
        {
            if (!strcmp(*it, windName))
            {
                *it = strdup(newWindName);
                break;
            }
            ++it;
        }
		windName = strdup(newWindName);
    }
}

int CamStreamer::getKey(int timestamp)const
{
	return cvWaitKey(timestamp);
}

void CamStreamer::startStream()
{
	cvNamedWindow(windName);
	capture = cvCaptureFromCAM(camIndex);
	isStreaming = true;
}

void CamStreamer::fetchFrame()
{
	camStream = cvQueryFrame(capture);
	if (!camStream)
		std::cerr << "Error no stream" << std::endl;
}

void CamStreamer::stopStream()
{
    cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	isStreaming = false;
}

void CamStreamer::displayImage(IplImage * toDisplay, char * windowName)
{
    if (strcmp(windowName, "") && !windowExists(windowName))
    {
        cvNamedWindow(windowName);
        windowList.push_back(windowName);
    }
	if (toDisplay != NULL && strcmp(windowName, ""))
	{
		cvShowImage(windowName, toDisplay);
	}
	else if (toDisplay != NULL)
	{
		cvShowImage(windName, toDisplay);
	}
	else
	{
		cvShowImage(windName, camStream);
	}
}

bool CamStreamer::isRunning()const
{
	return isStreaming;
}

bool CamStreamer::windowExists(char *w)
{
    std::list<char*>::iterator it = windowList.begin();
    
    while (it != windowList.end())
    {
        if (!strcmp(*it, w))
        {
            return true;
        }
        ++it;
    }
    return false;
}
