// robotController.h
#ifndef ROBOT_CONTROLLER_H
#define ROBOT_CONTROLLER_H
#define TIME_STEP 1.0
#include <linkbot.h>

typedef struct {
    double x;      // x-coordinate of the robot's position
    double y;      // y-coordinate of the robot's position
    double theta;  // robot's orientation (heading direction)
    double v;      // linear velocity of the robot
    double omega;  // angular velocity of the robot
    double distance;
    double angle;
} RobotState;

typedef struct {
    double x;
    double y;
} Point;

// structure to hold robot parameters
typedef struct {
    CLinkbotI robot;
    double radius;
    double trackwidth;
} RobotParams;

extern void initializeRobot();
extern void rotateRobot(RobotState *robotState, double angle);
extern void moveStraight(RobotState *robotState, double distance);
extern double distance(Point p1, Point p2);

/* load code from file robotController.c for Ch */
#ifdef _CH_
#pragma importf "robotController.c"
#endif

#endif // ROBOT_CONTROLLER_H
