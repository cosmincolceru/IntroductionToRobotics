#ifndef OBJECTS_H
#define OBJECTS_H

#include <Arduino.h>
#include "matrix.h"

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define MAX_BULLETS 3

struct Player{
  byte x, y;
  byte direction;
};

struct Bullet {
  byte x, y;
  byte direction;
  bool bulletShot;
  unsigned long lastBulletBlink = 0;
  unsigned long lastBulletMove = 0;
};

Player generatePlayer();

int generateWalls(byte matrix[MATRIX_SIZE][MATRIX_SIZE], Player player);

void bilnkPlayer(Player player);

void shootBullet(Player& player, Bullet bullets[MAX_BULLETS]);

void moveBullet(Bullet bullets[MAX_BULLETS], int index, Player& player, int& numWalls);

#endif