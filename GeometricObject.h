/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GeometricObject.h
 * Author: renato
 *
 * Created on 14 de Agosto de 2016, 12:06
 */

#ifndef GEOMETRICOBJECT_H
#define GEOMETRICOBJECT_H

#define _USE_MATH_DEFINES
 
#include <cmath>    //M_PI
#include <string>   //std::string
#include <vector>   //std::vector
#include <cmath>
#include "Point.h"
#include "Matrix.h"
#include <iostream>     //std::cout
static const int MATRIX_SIZE = 3;

class GeometricObject {
public:
    GeometricObject(std::string name);
    GeometricObject(std::string name, Point p);
    GeometricObject(std::string name, std::vector<Point> points);
    GeometricObject();
    virtual ~GeometricObject();
    std::vector<Point> getPointsVector();
    void addPointToPointsVector(Point p);
    void addListOfPointsToPointsVector(std::vector<Point> points);
    std::string getName();
    int getNumPoints();
    Point getMassCenter();
    void prepareMatrix();
    void prepareTranslationMatrix(double dx, double dy);
    void prepareEscalonateMatrix(double sx, double sy);
    void prepareRotateMatrix(double angle);
    void translate(double dx, double dy);
    void scale(double sx, double sy);
    void rotate(double angle);
    void rotate(double angle, double coordX, double coordY);
    void calculateOperation(double m[MATRIX_SIZE][MATRIX_SIZE]);
    void setWindowsTransformationMatrix(double m[MATRIX_SIZE][MATRIX_SIZE]);
    void ApplyWindowsTransformation(double xMax, double xMin, double yMax, double yMin);
    void ApplyWindowsTransformation();
    void setAndApplyWindowsTransformation(double m[MATRIX_SIZE][MATRIX_SIZE], double xMax, double xMin, double yMax, double yMin);
private:
    std::string _name;
    std::vector<Point>_pointsVector;
    std::vector<Point>_windowsPointsVector;
    double _matrixWindowsTransformation[MATRIX_SIZE][MATRIX_SIZE];
    double _matrixTransaltion[MATRIX_SIZE][MATRIX_SIZE];
    double _matrixEscalonation[MATRIX_SIZE][MATRIX_SIZE];
    double _matrixRotation[MATRIX_SIZE][MATRIX_SIZE];
};

#endif /* GEOMETRICOBJECT_H */  

