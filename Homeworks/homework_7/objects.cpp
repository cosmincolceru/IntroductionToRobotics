#include "objects.h"

Player generatePlayer() {
  int x = random(0, MATRIX_SIZE);
  int y = random(0, MATRIX_SIZE);

  return Player {x, y, 0};
}

const int minWallsPercentage = 30;
const int maxWallsPercentage = 55;

int generateWalls(byte matrix[MATRIX_SIZE][MATRIX_SIZE], Player player) {
  // Calculate the minimum or maximul number of walls that can be genrated
  int minWalls = (minWallsPercentage * MATRIX_SIZE * MATRIX_SIZE) / 100;
  int maxWalls = (maxWallsPercentage * MATRIX_SIZE * MATRIX_SIZE) / 100;

  int numWalls = random(minWalls, maxWalls + 1);

  for (int i = 0; i < numWalls; i++) {
    int row = random(0, MATRIX_SIZE);
    int col = random(0, MATRIX_SIZE);

    // Don't generate walls over the player
    while (row == player.x && col == player.y) {
      row = random(0, MATRIX_SIZE);
      col = random(0, MATRIX_SIZE);
    }
  
    matrix[row][col] = 1;
  }

  numWalls = 0;
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      numWalls += matrix[i][j];
    }
  }

  return numWalls;
}

unsigned long lastPlayerBlink;
unsigned long playerBlinkTime = 300;

void bilnkPlayer(Player player) {
  if (millis() - lastPlayerBlink > playerBlinkTime) {
    matrix[player.x][player.y] = !matrix[player.x][player.y];
    lastPlayerBlink = millis();
  }
}

void shootBullet(Player& player, Bullet bullets[MAX_BULLETS]) {
  for (int i = 0; i < MAX_BULLETS; i++) {
      if (bullets[i].bulletShot == false) {
        bullets[i].x = player.x;
        bullets[i].y = player.y;
        bullets[i].direction = player.direction;
        bullets[i].bulletShot = true;
        
        bullets[i].lastBulletBlink = millis();
        bullets[i].lastBulletMove = millis();
        break;
      } 
    }
}

unsigned long bulletBlinkTime = 100;
unsigned long bulletMoveTime = 200;

// Move the bullet and if the bullet hits a wall, distroy that wall. Return the number of walls that remain
void moveBullet(Bullet bullets[MAX_BULLETS], int index, Player& player, int& numWalls) {
  // Blink the bullet
  if ((millis() - bullets[index].lastBulletBlink > bulletBlinkTime) &&
     !(bullets[index].x == player.x && bullets[index].y == player.y)) {
    
    matrix[bullets[index].x][bullets[index].y] = !matrix[bullets[index].x][bullets[index].y];
    bullets[index].lastBulletBlink = millis();
  }

  // Move the bullet in the correct direction
  if (millis() - bullets[index].lastBulletMove > bulletMoveTime) {
    matrix[bullets[index].x][bullets[index].y] = 0;
    if (bullets[index].direction == UP) {
      bullets[index].x--;
    }
    else if (bullets[index].direction == DOWN) {
      bullets[index].x++;
    }
    else if (bullets[index].direction == LEFT) {
      bullets[index].y--;
    }
    else if (bullets[index].direction == RIGHT)  {
      bullets[index].y++;
    }

    // If the bullet hits a wall, destory the wall
    if (matrix[bullets[index].x][bullets[index].y] == 1) {
      matrix[bullets[index].x][bullets[index].y] = 0;
      bullets[index].bulletShot = false;
      numWalls--;
    }

    // If the bullet exits the matrix do nothing
    if (bullets[index].x < 0 || bullets[index].x >= MATRIX_SIZE ||
        bullets[index].y < 0 || bullets[index].y >= MATRIX_SIZE) {
      
      bullets[index].bulletShot = false;
    }

    bullets[index].lastBulletMove = millis();
  }
}
