#!/bin/ch
#include <math.h>
#include "point_def.h"
#include <linkbot.h>

#define ANGULAR_VELOCITY_THRESHOLD 0.06
#define TIME_STEP 1.0

CLinkbotI robot1 = CLinkbotI("F4KL");
const radius = 1.75;
const trackwidth = 3.69;

// initialize robot's position and speed
void initializeRobot() {
    robot1.initPosition(0, 0, 0);
    robot1.setSpeed(1.0, radius);
}

// turn the robot by a given angle (radians)
void rotateRobot(RobotState *robotState, double angle) {
    if (robotState == NULL) {
        perror("Robot state is null");
        exit(EXIT_FAILURE);
    }

    if(angle < 0) {
      printf("Turning %f degrees (right)\n",angle);
    } else {
      printf("Turning %f degrees (left)\n",angle);
    }
    
    robot1.turnLeft(angle/1.43, radius, trackwidth);

    // update robotState's theta
    robotState->theta += angle;
    while (robotState->theta < -180) {
        robotState->theta += 2 * 180;
        robotState->theta = round(robotState->theta * 100.0) / 100.0;
    }
    while (robotState->theta >= 180) {
        robotState->theta -= 2 * 180;
        robotState->theta = round(robotState->theta * 100.0) / 100.0;
    }
}

// move robot straight by a given distance
void moveStraight(RobotState *robotState, double distance) {
    if (robotState == NULL) {
        perror("Robot state is null");
        exit(EXIT_FAILURE);
    }
    printf("Moving straight %f units\n",distance);
    robot1.driveDistance(distance, radius);
    
    double turnAngle = robotState->theta * 3.141592 / 180; // turnAngle in radians

    // update robotState's x and y position
    robotState->x += round(distance * cos(turnAngle) * 100.0) / 100.0;
    robotState->y += round(distance * sin(turnAngle) * 100.0) / 100.0;
}

double distance(Point p1, Point p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

int main() {

FILE *file = fopen("points.txt", "r");
if (file == NULL) {
    perror("Error opening file");
    return 1;
}

int totalCorners;
if (fscanf(file, "%d", &totalCorners) != 1) {
    perror("Error reading totalCorners");
    return 1;
}

// initialize pathPoints (based on totalCorners)
Point* pathPoints = (Point*)malloc(totalCorners * sizeof(Point));
if (pathPoints == NULL) {
    perror("Error allocating memory");
    return 1;
}

size_t numPoints = 0;
char line[256]; // lines in the file shouldn't exceed this length

// read remaining lines to store pathPoints
while (fgets(line, sizeof(line), file)) {
    double x, y;
    if (sscanf(line, "%lf, %lf", &x, &y) == 2) {
        if (numPoints < totalCorners) {
            pathPoints[numPoints].x = x;
            pathPoints[numPoints].y = y;
            numPoints++;
        } else {
            // if more lines than totalCorners
            printf("Warning: More lines in the file than specified by totalCorners.\n");
            break;
        }
    }
}

fclose(file);

// check values
printf("totalCorners: %d\n", totalCorners);
int i;
for (i = 0; i < totalCorners; i++) {
    printf("Point %zu: x = %lf, y = %lf\n", i + 1, pathPoints[i].x, pathPoints[i].y);
}

initializeRobot();

RobotState robotState; // initial state
robotState.x = 0;
robotState.y = 0;
robotState.theta = 90; // degrees

double errorThreshold = 1.0;  // 1 degree
double turnFactor = 1.0;  // 100%
double positionTolerance = 0.001;

Point fakeRobotState;
for (i = 1; i < totalCorners; i++) { // start at 1, since robot is at 0 (initially)
    Point target = pathPoints[i];
    Point currentRobotPosition = {robotState.x, robotState.y};

    if (fabs(robotState.x - target.x) < positionTolerance) {
        fakeRobotState.x = target.x;
    } else {
        fakeRobotState.x = robotState.x;
    }
    if (fabs(robotState.y - target.y) < positionTolerance) {
        fakeRobotState.y = target.y;
    } else {
        fakeRobotState.y = robotState.y;
    }

    double targetAngle = atan2(target.y - fakeRobotState.y, target.x - fakeRobotState.x);
    targetAngle = targetAngle * 180 / 3.141592;
    targetAngle = round(targetAngle * 100.0) / 100.0;

    // calculate angle to next point
    //printf("targetAngle: %f , target.x: %f , target.y: %f \n",targetAngle,target.x,target.y);
    printf("robotState.theta: %f , robotState.x: %f , robotState.y: %f \n",robotState.theta,robotState.x,robotState.y);

    // calculate angle the robot needs to rotate to face the target
    double error = targetAngle - robotState.theta;

    // normalize error to be within -180 and 180
    while (error > 180) error -= 2 * 180;
    while (error < -180) error += 2 * 180;

    double incrementalTurn = turnFactor * fabs(error);

    // while error is outside the threshold, keep adjusting
    while (fabs(error) > errorThreshold) {
        if (error > 0) {
            rotateRobot(&robotState, incrementalTurn);
        } else {
            rotateRobot(&robotState, -incrementalTurn);
        }

        // recalc error
        error = targetAngle - robotState.theta;
        while (error > 180) error -= 2 * 180;
        while (error < -180) error += 2 * 180;
        //printf("error: %f\n",error);
            
        usleep(1000000 * TIME_STEP);  // sleep for TIME_STEP seconds
    }

    // move robot straight to the target
    double moveDistance = sqrt(pow(target.x - robotState.x, 2) + pow(target.y - robotState.y, 2));
    moveStraight(&robotState, moveDistance);

    usleep(1000000 * TIME_STEP);  // sleep for TIME_STEP seconds
    //printf("Robot State: %f %f %f\n",robotState.x,robotState.y,robotState.theta);
}

// free memory when done
free(pathPoints);
}
