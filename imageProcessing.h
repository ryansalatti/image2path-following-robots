#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <cv.h>
#include <highgui.h>
#include "point_def.h"

extern double distance(Point p1, Point p2);
extern void roundPointsToNearestWhole(Point* pathPoints, int totalPoints);
extern void mergeClosePoints(Point* pathPoints, int* totalCorners, double threshold);
extern void removeClosePoints(Point* pathPoints, int* totalPoints, double minDistance);
extern void scalePathToFitArea(Point* pathPoints, int totalPoints, double maxWidth, double maxHeight);
extern int smoothPath(Point* pathPoints, int totalPoints, double minDistance);
extern IplImage* preprocessImage(char* filename);
extern IplImage* detectEdges(IplImage* src);
extern void nearestNeighborTSP(Point* points, int totalPoints);
extern Point* extractPathPoints(IplImage* edges, int* totalPoints);
extern Point* generatePathFromImage(char* filename, int* totalCorners);

/* load code from file imageProcessing.c for Ch */
#ifdef _CH_
#pragma importf "imageProcessing.c"
#endif

#endif /* IMAGE_PROCESSING_H */
