#ifndef POINT_DEF_H
#define POINT_DEF_H

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

typedef struct {
    double x, y;
} Vector;

#endif /* POINT_DEF_H */
