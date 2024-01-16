#include <stdio.h>
#include "imageProcessing.h"
#include "point_def.h"

int main() {
    
    printf("Capturing image using Raspberry Pi camera...\n");

    // capture image using Raspberry Pi camera
    if (system("raspistill -o capture.jpg") != 0) {
        perror("Error capturing image");
        return 1;
    }
    
    printf("Generating points from image\n");
    const double AREA_WIDTH = 6.0;  // drawing bounds (ft)
    const double AREA_HEIGHT = 6.0;  // drawing bounds (ft)
    const double MIN_MOVE_DISTANCE = 1.0;  // min distance robot should move
    const double MIN_REMOVE_POINT_DISTANCE = 0.02;
    const double MAX_REMOVE_POINT_DISTANCE = 0.25;

    int totalCorners = 0;

    Point* pathPoints = generatePathFromImage("capture.jpg", &totalCorners);
    scalePathToFitArea(pathPoints, totalCorners, AREA_WIDTH, AREA_HEIGHT);
    totalCorners = smoothPath(pathPoints, totalCorners, MIN_MOVE_DISTANCE);
    removeClosePoints(pathPoints, &totalCorners, MIN_REMOVE_POINT_DISTANCE);
    nearestNeighborTSP(pathPoints, totalCorners);
    removeClosePoints(pathPoints, &totalCorners, MAX_REMOVE_POINT_DISTANCE);

    FILE* file = fopen("points.txt", "w");
    if (!file) {
        perror("Error opening file for writing");
        free(pathPoints);
        return 0;
    }
    fprintf(file, "%d\n", totalCorners);
    
    int i;
    for (i = 0; i < totalCorners; i++) {
        fprintf(file, "%f, %f\n", pathPoints[i].x, pathPoints[i].y);
    }

    fclose(file);
    printf("Attaching file \n");
    mc_AgentAttachFile(mc_current_agent, "data", "points.txt");
    
    printf("Freeing pathPoints\n");
    free(pathPoints);
    
    return 0;
}


