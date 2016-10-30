/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Line.cpp
 * Author: renato
 * 
 * Created on 14 de Agosto de 2016, 12:50
 */

#include "Line.h"

Line::Line(Point p1, Point p2) : GeometricObject("noNameForYou") {
    addPointToPointsVector(p1);
    addPointToPointsVector(p2);
}

Line::Line(std::string name, Point p1, Point p2) : GeometricObject(name) {
    addPointToPointsVector(p1);
    addPointToPointsVector(p2);
}

Line::Line(std::string name, double xp1, double yp1, double xp2, double yp2) : GeometricObject(name) {
    addPointToPointsVector(Point(xp1, yp1));
    addPointToPointsVector(Point(xp2, yp2));
}

Line::Line() : GeometricObject() {}

Line::~Line() {
}