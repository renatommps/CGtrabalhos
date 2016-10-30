/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Matrix.cpp
 * Author: ltk
 * 
 * Created on August 22, 2016, 12:34 PM
 */

#include "Matrix.h"

Matrix::Matrix() {
}

Matrix::Matrix(const Matrix& orig) {
}

Matrix::~Matrix() {
}

void Matrix::setCoord(int x, int y, double valor){
    _matrix[x][y] = valor;
}



