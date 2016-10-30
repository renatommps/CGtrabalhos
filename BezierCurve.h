/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BezierCurve.h
 * Author: renato
 *
 * Created on 30 de Outubro de 2016, 14:28
 */

#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "GeometricObject.h"

class BezierCurve : public GeometricObject {
public:
    BezierCurve(std::string name);
    BezierCurve(std::string name, Point p);
    BezierCurve(std::string name, std::vector<Point> points);
    virtual ~BezierCurve();
private:
    double step = 0.001;
    std::vector<Point> _points;
    void generateCurvePoints();

};

#endif /* BEZIERCURVE_H */

