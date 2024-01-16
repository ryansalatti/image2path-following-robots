// robotController.c

#include "robotController.h"
#include <math.h>
#define ANGULAR_VELOCITY_THRESHOLD 0.06
#include <linkbot.h>

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
