/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BezierCurve.cpp
 * Author: renato
 * 
 * Created on 30 de Outubro de 2016, 14:28
 */

#include "BezierCurve.h"

BezierCurve::BezierCurve(std::string name) : GeometricObject(name) {

}

BezierCurve::BezierCurve(std::string name, Point p) : GeometricObject(name) {
    addPointToPointsVector(p);
}

BezierCurve::BezierCurve(std::string name, std::vector<Point> points) : GeometricObject(name) {
    for (Point p : points) {
        addPointToPointsVector(p);
    }
}

BezierCurve::~BezierCurve() {
}

void BezierCurve::generateCurvePoints() {
    std::vector<Point> points = _points;
    _points.clear();

    int num_curves = ((points.size() - 4) / 3) + 1;

    for (int i = 0; i < num_curves; i++) {
        for (float j = 0; j < 1; j += step) {
            double j2 = j * j;
            double j3 = j2 * j;

            double x, y;
            x = (-j3 + 3 * j2 - 3 * j + 1) * points[i * 3 + 0].getX() + (3 * j3 - 6 * j2 + 3 * j) * points[i * 3 + 1].getX() +
                    (-3 * j3 + 3 * j2) * points[i * 3 + 2].getX() + (j3) * points[i * 3 + 3].getX();
            y = (-j3 + 3 * j2 - 3 * j + 1) * points[i * 3 + 0].getY() + (3 * j3 - 6 * j2 + 3 * j) * points[i * 3 + 1].getY() +
                    (-3 * j3 + 3 * j2) * points[i * 3 + 2].getY() + (j3) * points[i * 3 + 3].getY();

            _points.emplace_back(x, y);
        }
    }
}
