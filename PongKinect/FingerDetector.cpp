#include "FingerDetector.h"


FingerDetector::FingerDetector()
{}

FingerDetector::~FingerDetector()
{}


void FingerDetector::findFingers(ConvexHullManager *manager)
{
}

std::list<FingerDetector::Coords*> FingerDetector::getFingerList()const
{
  return fingerList;
}
