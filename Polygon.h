/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Polygon.h
 * Author: renato
 *
 * Created on 15 de Agosto de 2016, 23:37
 */

#ifndef POLYGON_H
#define POLYGON_H

#include "GeometricObject.h"

class Polygon : public GeometricObject {
public:
    Polygon(std::string name);
    Polygon(std::string name, Point p);
    Polygon(std::string name, double xp, double yp);
    Polygon(std::string name, std::vector<Point> points);
    Polygon();
    virtual ~Polygon();
private:

};

#endif /* POLYGON_H */

