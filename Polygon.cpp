/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Polygon.cpp
 * Author: renato
 * 
 * Created on 15 de Agosto de 2016, 23:37
 */

#include "Polygon.h"

Polygon::Polygon(std::string name) : GeometricObject(name) {
}

Polygon::Polygon(std::string name, Point p) : GeometricObject(name) {
    addPointToPointsVector(p);
}

Polygon::Polygon(std::string name, double xp, double yp) : GeometricObject(name) {
    addPointToPointsVector(Point(xp, yp));
}

Polygon::Polygon(std::string name, std::vector<Point> points) : GeometricObject(name) {
    for (Point p : points) {
        addPointToPointsVector(p);
    }
}

Polygon::Polygon() {
}

Polygon::~Polygon() {
}

