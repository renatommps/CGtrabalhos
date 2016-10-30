/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Matrix.h
 * Author: ltk
 *
 * Created on August 22, 2016, 12:34 PM
 */

#ifndef MATRIX_H
#define MATRIX_H

class Matrix {
public:
    Matrix();
    Matrix(const Matrix& orig);
    virtual ~Matrix();
    void setCoord(int x, int y, double valor);
    
private:
    double _matrix [3][3];
};

#endif /* MATRIX_H */

