/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DisplayFile.cpp
 * Author: renato
 * 
 * Created on 15 de Agosto de 2016, 12:50
 */

#include "DisplayFile.h"

DisplayFile::DisplayFile(GeometricObject obj) {
    _objectsList.push_back(obj);
}

DisplayFile::DisplayFile() {
}

DisplayFile::~DisplayFile() {
}

void DisplayFile::addObject(GeometricObject obj) {
    _objectsList.push_back(obj);
}

std::vector<GeometricObject> * DisplayFile::getObjects() {
    return &_objectsList;
}

void DisplayFile::translateObject(double dx, double dy, std::string objName) {
    for (int i = 0; i < _objectsList.size(); i++) {
        std::string myObjName = _objectsList[i].getName();
        if (myObjName == objName) {
            _objectsList[i].translate(dx, dy);
            break;
        }
    }
}

void DisplayFile::scaleObject(double sx, double sy, std::string objName) {
    for (int i = 0; i < _objectsList.size(); i++) {
        std::string myObjName = _objectsList[i].getName();
        if (myObjName == objName) {
            _objectsList[i].scale(sx, sy);
            break;
        }
    }
}

void DisplayFile::rotateObjectToPointReference(double coordX, double coordY, double angle, std::string objName) {
    for (int i = 0; i < _objectsList.size(); i++) {
        std::string myObjName = _objectsList[i].getName();
        if (myObjName == objName) {
            _objectsList[i].rotate(angle, coordX, coordY);
            break;
        }
    }
}

void DisplayFile::rotateObject(double angle, std::string objName) {
    for (int i = 0; i < _objectsList.size(); i++) {
        std::string myObjName = _objectsList[i].getName();
        if (myObjName == objName) {
            _objectsList[i].rotate(angle);
            break;
        }
    }
}
