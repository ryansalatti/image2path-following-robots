// imageProcessing.c
#include "imageProcessing.h"
#include <math.h>
#define CURVATURE_THRESHOLD 35

double distance(Point p1, Point p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// calculate direction vector from point a to point b
Vector calculateDirection(Point a, Point b) {
    Vector dir;
    dir.x = b.x - a.x;
    dir.y = b.y - a.y;
    return dir;
}

// normalize a vector to unit length
Vector normalize(Vector v) {
    double length = sqrt(v.x * v.x + v.y * v.y);
    // Avoid division by zero
    if (length != 0) {
        v.x /= length;
        v.y /= length;
    }
    return v;
}

double calculateSimilarity(Vector v1, Vector v2) {
    v1 = normalize(v1);
    v2 = normalize(v2);
    // dot product
    double dot = v1.x * v2.x + v1.y * v2.y;
    // ensure dot product is within the valid range [-1, 1]
    if (dot > 1.0) dot = 1.0;
    if (dot < -1.0) dot = -1.0;
    return dot;
}

void scalePathToFitArea(Point* pathPoints, int totalPoints, double maxWidth, double maxHeight) {
    double minX = pathPoints[0].x;
    double minY = pathPoints[0].y;
    double maxX = pathPoints[0].x;
    double maxY = pathPoints[0].y;

    // find bounding box of the path
    int i;
    for (i = 1; i < totalPoints; i++) {
        if (pathPoints[i].x < minX) {
            minX = pathPoints[i].x;
        }
        if (pathPoints[i].x > maxX) {
            maxX = pathPoints[i].x;
        }
        if (pathPoints[i].y < minY) {
            minY = pathPoints[i].y;
        }
        if (pathPoints[i].y > maxY) {
            maxY = pathPoints[i].y;
        }
    }

    double pathWidth = maxX - minX;
    double pathHeight = maxY - minY;

    // determine scaling factor
    double scaleX = maxWidth / pathWidth;
    double scaleY = maxHeight / pathHeight;
    double scale = (scaleX < scaleY) ? scaleX : scaleY;  // take smaller scale

    // scale points, round to nearest hundredths
    for (i = 0; i < totalPoints; i++) {
        pathPoints[i].x = round(((pathPoints[i].x - minX) * scale) * 100.0) / 100.0;  // -minX to translate to origin before scaling
        pathPoints[i].y = round(((pathPoints[i].y - minY) * scale) * 100.0) / 100.0;  // -minY to translate to origin before scaling and then translate in y direction
    }

}

void removeClosePoints(Point* pathPoints, int* totalPoints, double minDistance) {
    int i = 0;
    int j = 0;
    while (i < *totalPoints) {
        int hasClosePoint = 0;

        for (j = i + 1; j < *totalPoints; j++) {
            double dist = distance(pathPoints[i], pathPoints[j]);
            if (dist < minDistance) {
                // Remove point j by shifting the rest of the array left
                memmove(&pathPoints[j], &pathPoints[j + 1], (*totalPoints - j - 1) * sizeof(Point));
                (*totalPoints)--;
                hasClosePoint = 1;
                j--; // Decrement j to check the new point at position j
            }
        }

        // increment i only if no close point was found for the current point i
        if (!hasClosePoint) {
            i++;
        }
    }
}

int smoothPath(Point* pathPoints, int totalPoints, double minDistance) {
    int i, j = 0;
    Point smoothedPath[totalPoints];
    smoothedPath[j++] = pathPoints[0];
    for (i = 1; i < totalPoints - 1; i++) {
        double x1 = pathPoints[i - 1].x;
        double y1 = pathPoints[i - 1].y;
        double x2 = pathPoints[i].x;
        double y2 = pathPoints[i].y;
        double x3 = pathPoints[i + 1].x;
        double y3 = pathPoints[i + 1].y;
        
        double curvature = fabs((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1)) /
                           sqrt(((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) *
                                ((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1)) *
                                ((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2)));

        
        if (curvature > CURVATURE_THRESHOLD || distance(pathPoints[i], smoothedPath[j - 1]) >= minDistance) {
            smoothedPath[j++] = pathPoints[i];
        }
    }
    smoothedPath[j++] = pathPoints[totalPoints - 1]; // add last point
    
    for (i = 0; i < j; i++) {
        pathPoints[i] = smoothedPath[i];
    }
    // return the new total number of points
    return j;
}

IplImage* preprocessImage(char* filename) {
    IplImage* src = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
    if (!src) {
        printf("Failed to load image file: %s\n", filename);
        return NULL;
    }

    // noise reduction
    cvSmooth(src, src, CV_MEDIAN, 3, 3, 0, 0);

    double max_value = 255;
    int adaptive_method = CV_ADAPTIVE_THRESH_MEAN_C;
    int threshold_type = CV_THRESH_BINARY;
    int block_size = 45;  // Adjust this as needed
    double param1 = 6;    // Adjust this as needed

    IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    // adaptive thresholding
    cvAdaptiveThreshold(src, dst, max_value, adaptive_method, threshold_type, block_size, param1);

    // morphological dilation to enhance black lines
    IplConvKernel* element = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, NULL);
 
    cvDilate(dst, dst, element, 1);
    
    cvSaveImage("morphed.jpg", dst);

    cvReleaseStructuringElement(&element);
    
    return dst;
}

IplImage* detectEdges(IplImage* closed) {
    IplImage* edges = cvCreateImage(cvGetSize(closed), IPL_DEPTH_8U, 1);
    cvCanny(closed, edges, 50, 150, 3);
    return edges;
}

void nearestNeighborTSP(Point* points, int totalPoints) {
    if (totalPoints <= 1) return; // no need to sort if 1 or 0 points

    int* visited = (int*)calloc(totalPoints, sizeof(int));
    Point* sortedPoints = (Point*)malloc(totalPoints * sizeof(Point));
    Vector currentDirection = calculateDirection(points[0], points[1]); // Initial direction

    sortedPoints[0] = points[0];
    visited[0] = 1;
    int i, j;
    double similarityFactor = 0.3; // higher value means more weight on distance

    for (i = 1; i < totalPoints; i++) {
        double minScore = DBL_MAX;
        int nextIndex = -1;

        for (j = 0; j < totalPoints; j++) {
            if (!visited[j]) {
                double dist = distance(sortedPoints[i - 1], points[j]);
                Vector directionToPoint = calculateDirection(sortedPoints[i - 1], points[j]);
                double similarity = calculateSimilarity(currentDirection, directionToPoint);

                double score = dist - (similarityFactor * similarity);

                if (score < minScore) {
                    nextIndex = j;
                    minScore = score;
                }
            }
        }

        if (nextIndex != -1) {
            visited[nextIndex] = 1;
            sortedPoints[i] = points[nextIndex];
            // update direction for next iteration
            if (i < totalPoints - 1) {
                currentDirection = calculateDirection(points[nextIndex], points[nextIndex + 1]);
            }
        }
    }

    for (i = 0; i < totalPoints; i++) {
        points[i] = sortedPoints[i];
    }

    free(sortedPoints);
    free(visited);
}

Point* extractPathPoints(IplImage* edges, int* totalPoints) {
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contours = 0;
    cvFindContours(edges, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));

    double minAreaThreshold = 85.0; // allowable threshold for contours

    // dynamic array to store points
    Point* allPoints = NULL;
    int total = 0;
    int i;
    CvSeq* c;
    for (c = contours; c != NULL; c = c->h_next) {
        double area = fabs(cvContourArea(c, CV_WHOLE_SEQ));
        if (area > minAreaThreshold) {
            for (i = 0; i < c->total; i++) {
                CvPoint* pt = (CvPoint*)cvGetSeqElem(c, i);

                Point* tempPoints = realloc(allPoints, (total + 1) * sizeof(Point));
                if (tempPoints == NULL) {
                    // handle memory allocation error
                    free(allPoints);  // free previously allocated memory
                    *totalPoints = 0;
                    return NULL;
                }
                allPoints = tempPoints;

                allPoints[total].x = (double)pt->x;
                allPoints[total].y = (double)pt->y;
                total++;
            }
        }
    }

    *totalPoints = total;
    cvReleaseMemStorage(&storage);
    return allPoints;
}

Point* generatePathFromImage(char* filename, int* totalCorners) {
    IplImage* preprocessed = preprocessImage(filename);
    IplImage* edges = detectEdges(preprocessed);
    Point* pathPoints = extractPathPoints(edges, totalCorners);

    cvReleaseImage(&preprocessed);
    cvReleaseImage(&edges);

    return pathPoints;
}

