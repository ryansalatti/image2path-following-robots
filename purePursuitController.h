#ifndef PURE_PURSUIT_CONTROLLER_H
#define PURE_PURSUIT_CONTROLLER_H

#include <stdbool.h>
#include "robot_def.h"

extern void updateRobotState(RobotState* robotState, double timeStep, int command, double distance, double angle);
extern void controlRobot(RobotState* robotState, Point* pathPoints, int pathLength, bool *pathCompleted);

/* load code from file purePursuitController.c for Ch */
#ifdef _CH_
#pragma importf "purePursuitController.c"
#endif

#endif /* PURE_PURSUIT_CONTROLLER_H */
