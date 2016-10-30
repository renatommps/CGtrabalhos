/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Clipping.h
 * Author: ltk
 *
 * Created on October 5, 2016, 3:26 PM
 */

#ifndef CLIPPING_H
#define CLIPPING_H

#include <vector>
#include "Point.h"
#include "Line.h"
#include "GeometricObject.h"
#include "Window.h"

class Clipping {
public:
    Clipping(std::vector<GeometricObject> graficObjects, Window * window);
    Clipping(const Clipping& orig);
    virtual ~Clipping();

    enum direction {
        north, northeast, east, southeast, south, southwest, west, northwest, center
    };
    //clipping(Window2D * janela);
    //Objeto * clip(Objeto * objeto);

    std::vector<GeometricObject> clip();
    GeometricObject * clipLine(Point p1, Point p2);

private:

    struct binCode {

        void setBit0(bool bit) {
            code[0] = bit;
        }

        void setBit1(bool bit) {
            code[1] = bit;
        }

        void setBit2(bool bit) {
            code[2] = bit;
        }

        void setBit3(bool bit) {
            code[3] = bit;
        }

        void reset() {
            setBit0(0);
            setBit1(0);
            setBit2(0);
            setBit3(0);
        }

        void set() {
            setBit0(1);
            setBit1(1);
            setBit2(1);
            setBit3(1);
        }
        binCode operator&(binCode other);
        bool operator==(binCode other);
        Clipping::direction getDirection();

    private:
        bool code[4] = {0, 0, 0, 0};
        int codeToInt();
    };

    std::vector<GeometricObject> myGraficObjects;
    Window * myWindowReference;

    binCode checkCode(Point point);
    double getAngularCoefficient(Line line);

    Point intersectionTop(Point point, double angularCoefficient);
    Point intersectionRight(Point point, double angularCoefficient);
    Point intersectionLeft(Point point, double angularCoefficient);
    Point intersectionBotton(Point point, double angularCoefficient);
    bool pointIsInsideWindow(Point point);
};

#endif /* CLIPPING_H */

