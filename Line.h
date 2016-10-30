/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Line.h
 * Author: renato
 *
 * Created on 14 de Agosto de 2016, 12:50
 */

#ifndef LINE_H
#define LINE_H

#include "GeometricObject.h"

class Line : public GeometricObject {
public:
    Line(std::string name, Point p1, Point p2);
    Line(Point p1, Point p2);
    Line(std::string name, double xp1, double yp1, double xp2, double yp2);
    Line();
    virtual ~Line();
private:
};

#endif /* LINE_H */

