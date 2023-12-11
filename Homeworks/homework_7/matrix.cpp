#include "matrix.h"

byte matrix[MATRIX_SIZE][MATRIX_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

void updateMatrix(LedControl lc, byte matrix[MATRIX_SIZE][MATRIX_SIZE]) {
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void clearMatrix(byte matrix[MATRIX_SIZE][MATRIX_SIZE]) {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrix[i][j] = 0;
    }
  }
}

void matrixOn(byte matrix[MATRIX_SIZE][MATRIX_SIZE]) {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrix[i][j] = 1;
    }
  }
}

