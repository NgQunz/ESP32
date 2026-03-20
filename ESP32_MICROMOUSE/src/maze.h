#ifndef MAZE_H
#define MAZE_H

#include <Arduino.h>
#include "BluetoothSerial.h"

extern int x;
extern int y;
extern int facing;
extern int moveCount;
extern bool inDeadEnd;
extern bool goalReached;

// wallMap[x][y][dir]: true = có tường
// dir: 0=N, 1=E, 2=S, 3=W
extern bool wallMap[16][16][4];

void initMazeScore();
void goCell();
void turnLeft90();
void turnRight90();
void turnBack180();
void moveRobot(char relativeDir);
void think();
void runMaze();

#endif