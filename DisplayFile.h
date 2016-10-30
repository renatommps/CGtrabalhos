/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DisplayFile.h
 * Author: renato
 *
 * Created on 15 de Agosto de 2016, 12:50
 */

#ifndef DISPLAYFILE_H
#define DISPLAYFILE_H

#include <vector>       //std::vector
#include "GeometricObject.h"

class DisplayFile {
public:
    DisplayFile(GeometricObject obj);
    DisplayFile();
    virtual ~DisplayFile();
    void addObject(GeometricObject obj);
    std::vector<GeometricObject> * getObjects();
    void translateObject(double dx, double dy, std::string objName);
    void scaleObject(double sx, double sy, std::string objName);
    void rotateObjectToPointReference(double coordX, double coordY, double angle, std::string objName);
    void rotateObject(double angle, std::string objName);
private:
    std::vector<GeometricObject> _objectsList;
};

#endif /* DISPLAYFILE_H */

