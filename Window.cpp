/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Window.cpp
 * Author: renato
 * 
 * Created on 14 de Agosto de 2016, 22:31
 */

#include <algorithm>    // std::min, std::max, std::max_element
#include <cmath>        // std::abs
#include <list>         // std::list
#include "Window.h"
#include "GeometricObject.h"

Window::Window(double xMin, double yMin, double xMax, double yMax, DisplayFile * display) {
    if (xMax > xMin) {
        _xMin = xMin;
        _xMax = xMax;
    } else {
        _xMin = X_MIN_DEFAULT;
        _xMax = X_MAX_DEFAULT;
    }

    if (yMax > yMin) {
        _yMin = yMin;
        _yMax = yMax;
    } else {
        _yMin = Y_MIN_DEFAULT;
        _yMax = Y_MAX_DEFAULT;
    }
    _displayFile = display;
    _angle = START_ANGLE;

    /* inicia a matriz _SCNdescriptionMatrix */
    for (int i = 0; i < SCN_MATRIX_SIZE; i++) {
        for (int j = 0; j < SCN_MATRIX_SIZE; j++) {
            if (i == j) {
                _SCNdescriptionMatrix[i][j] = 1;
            } else {
                _SCNdescriptionMatrix[i][j] = 0;
            }
        }
    }
}

Window::Window() {
}

Window::~Window() {
}

double Window::distance(double a, double b) {
    return std::max(a, b) - std::min(a, b);
}

void Window::moveUp(double value) {
    if (_yMax + value <= WINDOW_MAX_VALUE) {
        _yMin += value;
        _yMax += value;
    } else {
        double maxValidValue = distance(WINDOW_MAX_VALUE, _yMax);
        _yMin += maxValidValue;
        _yMax += maxValidValue;
    }
    generateDescriptionSCN();
}

void Window::moveDown(double value) {
    if (_yMin - value >= WINDOW_MIN_VALUE) {
        _yMin -= value;
        _yMax -= value;
    } else {
        double maxValidValue = distance(WINDOW_MIN_VALUE, _yMin);
        _yMin -= maxValidValue;
        _yMax -= maxValidValue;
    }
    generateDescriptionSCN();
}

void Window::moveLeft(double value) {
    if (_xMin - value >= WINDOW_MIN_VALUE) {
        _xMin -= value;
        _xMax -= value;
    } else {
        double maxValidValue = distance(WINDOW_MIN_VALUE, _xMin);
        _xMin -= maxValidValue;
        _xMax -= maxValidValue;
    }
    generateDescriptionSCN();
}

void Window::moveRight(double value) {
    if (_xMin + value <= WINDOW_MAX_VALUE) {
        _xMin += value;
        _xMax += value;
    } else {
        double maxValidValue = distance(WINDOW_MAX_VALUE, _xMax);
        _xMin += maxValidValue;
        _xMax += maxValidValue;
    }
    generateDescriptionSCN();
}

void Window::zoomIn(double value) {
    double zoomvalue = value / 2;
    if (((_xMin + zoomvalue) < (_xMax - zoomvalue)) && ((_yMin + zoomvalue) < (_yMax - zoomvalue))) {
        _xMin += value;
        _xMax -= value;
        _yMin += value;
        _yMax -= value;
    } else {
        //        double xDistance =  (( distance(_xMax, _xMin)/2) - 1);
        //        double yDistance =  (( distance(_yMax, _yMin)/2) - 1);
        //        double maxValideValue = std::min(xDistance, yDistance);
        //        _xMin += maxValideValue;
        //        _xMax -= maxValideValue;
        //        _yMin += maxValideValue;
        //        _yMax -= maxValideValue;
    }
    generateDescriptionSCN();
}

void Window::zoomOut(double value) {
    double zoomvalue = value / 2;
    double xMinZoomedValue = _xMin - zoomvalue;
    double xMaxZoomedValue = _xMax + zoomvalue;
    double yMinZoomedValue = _yMin - zoomvalue;
    double yMaxZoomedValue = _yMax + zoomvalue;

    if ((xMinZoomedValue >= WINDOW_MIN_VALUE) && (xMaxZoomedValue <= WINDOW_MAX_VALUE)
            && (yMinZoomedValue >= WINDOW_MIN_VALUE) && (yMaxZoomedValue <= WINDOW_MAX_VALUE)) {
        _xMin = xMinZoomedValue;
        _xMax = xMaxZoomedValue;
        _yMin = yMinZoomedValue;
        _yMax = yMaxZoomedValue;
    } else {
        //        std::list<double> listofValidValues;
        //        listofValidValues.push_back( std::abs( (_xMin - WINDOW_MIN_VALUE) / 2 ) );
        //        listofValidValues.push_back( std::abs( (WINDOW_MAX_VALUE - _xMax) / 2 ) );
        //        listofValidValues.push_back( std::abs( (_yMin - WINDOW_MIN_VALUE) / 2 ) );
        //        listofValidValues.push_back( std::abs( (WINDOW_MAX_VALUE - _yMax) / 2 ) );
        //
        //        listofValidValues.sort();
        //        double maxValideValue = listofValidValues.back();
        //
        //        _xMin -= maxValideValue;
        //        _xMax += maxValideValue;
        //        _yMin -= maxValideValue;
        //        _yMax += maxValideValue;
    }
    generateDescriptionSCN();
}

void Window::rotate(double value) {
    if (_angle + value > 360) {
        _angle = _angle + value - 360;
    } else {
        _angle += value;
    }
    std::cout << "Window rotate chamado, vai aplicar descrição SCN." << std::endl;
    generateDescriptionSCN();
}

double Window::getXmin() {
    return _xMin;
}

double Window::getYmin() {
    return _yMin;
}

double Window::getXmax() {
    return _xMax;
}

double Window::getYmax() {
    return _yMax;
}

double Window::getHeight() {
    return _yMax - _yMin;
}

double Window::getWidth() {
    return _xMax - _xMin;
}

double Window::getAngle() {
    return _angle;
}

Point Window::getCenter() {
    double xCenter = _xMin + ((_xMax - _xMin) / 2);
    double yCenter = _yMin + ((_yMax - _yMin) / 2);
    Point pCenter(xCenter, yCenter);

    return pCenter;
}

void Window::generateDescriptionSCN() {

    /* inicia a matriz _SCNdescriptionMatrix */
    for (int i = 0; i < SCN_MATRIX_SIZE; i++) {
        for (int j = 0; j < SCN_MATRIX_SIZE; j++) {
            if (i == j) {
                _SCNdescriptionMatrix[i][j] = 1;
            } else {
                _SCNdescriptionMatrix[i][j] = 0;
            }
        }
    }

    Point centerPoint = getCenter();
    applyTranslation(-centerPoint.getX(), -centerPoint.getY());
    applyRotation(-_angle);
    applyTranslation(centerPoint.getX(), centerPoint.getY());
    
    std::vector<GeometricObject> * objects = _displayFile->getObjects();
    int numObjs = objects->size();
    for (int i = 0; i < numObjs; i++) {
        (objects->at(i)).setAndApplyWindowsTransformation(_SCNdescriptionMatrix, (getWidth() / 2), -(getWidth() / 2), (getHeight() / 2), -(getHeight() / 2));
    }
    //    for (GeometricObject * obj : objects ) {
    //        obj->applyWindowsTransformation(_SCNdescriptionMatrix, (getWidth() / 2), -(getWidth() / 2), (getHeight() / 2), -(getHeight() / 2));
    //    }
}

double Window::setAngle(double value) {
    _angle = value;
}

void Window::applyTranslation(double dx, double dy) {
    double partialMatrix[SCN_MATRIX_SIZE][SCN_MATRIX_SIZE];

    /* inicia a matriz parcial */
    for (int i = 0; i < SCN_MATRIX_SIZE; i++) {
        for (int j = 0; j < SCN_MATRIX_SIZE; j++) {
            if (i == j) {
                partialMatrix[i][j] = 1;
            } else {
                partialMatrix[i][j] = 0;
            }
        }
    }

    partialMatrix[2][0] = dx;
    partialMatrix[2][1] = dy;

    multiplyMatrixSCN(partialMatrix);
}

void Window::applyRotation(double angle) {
    double partialMatrix[SCN_MATRIX_SIZE][SCN_MATRIX_SIZE];
    double radAngle = angle * (M_PI / 180.0);

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (i == j) {
                partialMatrix[i][j] = 1;
            } else {
                partialMatrix[i][j] = 0;
            }
        }
    }

    partialMatrix[0][0] = std::cos(radAngle);
    partialMatrix[0][1] = std::sin(radAngle);
    partialMatrix[1][0] = -std::sin(radAngle);
    partialMatrix[1][1] = std::cos(radAngle);

    multiplyMatrixSCN(partialMatrix);
}

void Window::multiplyMatrixSCN(double m[][SCN_MATRIX_SIZE]) {
    double mult[SCN_MATRIX_SIZE][SCN_MATRIX_SIZE];

    for (int i = 0; i < SCN_MATRIX_SIZE; ++i) {
        for (int j = 0; j < SCN_MATRIX_SIZE; ++j) {
            mult[i][j] = 0;
            for (int k = 0; k < SCN_MATRIX_SIZE; ++k) {
                mult[i][j] += _SCNdescriptionMatrix[i][k] * m[k][j];
            }
        }
    }

    std::cout << "matriz multiplicada:" << std::endl;
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (j == 0) {
                std::cout << "|" << mult[i][j];
            } else if (j == 2) {
                std::cout << mult[i][j] << "|" << std::endl;
            } else {
                std::cout << " " << mult[i][j] << " ";
            }

        }
    }

    /* atualiza a matriz de descrição SCN com o resultado da matriz multiplicada */
    for (int i = 0; i < SCN_MATRIX_SIZE; i++) {
        for (int j = 0; j < SCN_MATRIX_SIZE; j++) {
            _SCNdescriptionMatrix[i][j] = mult[i][j];
        }
    }
}