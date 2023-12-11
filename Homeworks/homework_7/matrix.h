#ifndef MATRIX_H
#define MATRIX_H

#include <Arduino.h>
#include "LedControl.h"

#define MATRIX_SIZE 8

extern byte matrix[MATRIX_SIZE][MATRIX_SIZE];

void updateMatrix(LedControl lc, byte matrix[MATRIX_SIZE][MATRIX_SIZE]);
void clearMatrix(byte matrix[MATRIX_SIZE][MATRIX_SIZE]);
void matrixOn(byte matrix[MATRIX_SIZE][MATRIX_SIZE]);

#endif
