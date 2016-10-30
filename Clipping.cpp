/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Clipping.cpp
 * Author: ltk
 * 
 * Created on October 5, 2016, 3:26 PM
 */

#include "Clipping.h"

Clipping::Clipping(std::vector<GeometricObject> graficObjects, Window * window) {
    myGraficObjects = graficObjects;
    myWindowReference = window;
}

Clipping::Clipping(const Clipping& orig) {
}

Clipping::~Clipping() {
}

Clipping::direction Clipping::binCode::getDirection() {
    switch (codeToInt()) {
        case 1000: return north;
        case 1010: return northeast;
        case 0010: return east;
        case 0110: return southeast;
        case 0100: return south;
        case 0101: return southwest;
        case 0001: return west;
        case 1001: return northwest;
        case 0000: return center;

    }
}
//-----------------------------------------------

int Clipping::binCode::codeToInt() {
    return ( ((int) code[0] * 1000) + ((int) code[1] * 100) + ((int) code[2] * 10) + ((int) code[3] * 1));
}

Clipping::binCode Clipping::binCode::operator&(Clipping::binCode otherCode) {
    binCode r;
    r.setBit0(this->code[0] && otherCode.code[0]);
    r.setBit1(this->code[1] && otherCode.code[1]);
    r.setBit2(this->code[2] && otherCode.code[2]);
    r.setBit3(this->code[3] && otherCode.code[3]);
    return r;
}

bool Clipping::binCode::operator==(Clipping::binCode otherCode) {
    return this->code[0] == otherCode.code[0] &&
            this->code[1] == otherCode.code[1] &&
            this->code[2] == otherCode.code[2] &&
            this->code[3] == otherCode.code[3];
}

double Clipping::getAngularCoefficient(Line line) {
    std::vector<Point> points = line.getPointsVector();
    Point initialPoint = points.front();
    Point finalPoint = points.back();
    return ( initialPoint.getY() - finalPoint.getY()) / (initialPoint.getX() - finalPoint.getX());
}

Clipping::binCode Clipping::checkCode(Point point) {
    binCode retorno;

    retorno.setBit0(point.getY() > myWindowReference->getYmax());
    retorno.setBit1(point.getY() < myWindowReference->getYmin());
    retorno.setBit2(point.getX() > myWindowReference->getXmax());
    retorno.setBit3(point.getX() < myWindowReference->getXmin());

    return retorno;
}

Point Clipping::intersectionTop(Point point, double angularCoefficient) {
    return Point(point.getX() + (1 / angularCoefficient * (myWindowReference->getYmax() - point.getY())), myWindowReference->getYmax());
}

Point Clipping::intersectionRight(Point point, double angularCoefficient) {
    return Point(myWindowReference->getXmax(), angularCoefficient * (myWindowReference->getXmax() - point.getX()) + point.getY());
}

Point Clipping::intersectionLeft(Point point, double angularCoefficient) {
    return Point(myWindowReference->getXmin(), angularCoefficient * (myWindowReference->getXmin() - point.getX()) + point.getY());
}

Point Clipping::intersectionBotton(Point point, double angularCoefficient) {
    return Point(point.getX() + (1 / angularCoefficient * (myWindowReference->getYmin() - point.getY())), myWindowReference->getYmin());
}

bool Clipping::pointIsInsideWindow(Point point) {
    return point.getX() >= myWindowReference->getXmin() &&
            point.getX() <= myWindowReference->getXmax() &&
            point.getY() >= myWindowReference->getYmin() &&
            point.getY() <= myWindowReference->getYmax();
}

std::vector<GeometricObject> Clipping::clip() {
    std::vector<GeometricObject> clippedObjects;
    /* itera sobre todos os objetos que serão desenhados*/
    for (GeometricObject obj : myGraficObjects) {

        /* pega todos os pontos do objeto*/
        std::vector<Point> objPoints = obj.getPointsVector();
        /* pega o número de pontos do objeto*/
        int objNumPoints = objPoints.size();

        /* se tiver menos de dois pontos, quer dizer que não é possível desenhar
         nem uma reta, no caso, o objeto seria apenas um ponto */
        if (objNumPoints < 2) {
            break;
        }

        Point p1, p2;
        double xp1, yp1;
        double xp2, yp2;

        /* define o ponto inicial (primeiro ponto do objeto) */
        p1 = objPoints[0];

        /* itera sobre todos os pontos do objeto */
        for (int i = 1; i < objNumPoints; i++) {
            p2 = objPoints[i];
            GeometricObject * obj = (clipLine(p1, p2));
            //ver se o objeto existe antes de adicionar
            if (obj) {
                clippedObjects.push_back(*obj);
            } else {
                std::cout << "Tirou o nullptr" << std::endl;
            }
            p1 = p2;
        }
        if (objNumPoints > 2) {
            p1 = objPoints[0];
            p2 = objPoints[objNumPoints - 1];
            GeometricObject * obj = (clipLine(p1, p2));
            if (obj) {
                clippedObjects.push_back(*obj);
            } else {
                std::cout << "Tirou o nullptr" << std::endl;
            }
            break;
        }
    }
    return clippedObjects;
}

GeometricObject * Clipping::clipLine(Point p1, Point p2) {

    bool draw = false;
    Line * line = new Line(p1, p2);

    //Rotaciona a reta para trabalharmos em coordenadas da window
    binCode p1Code = checkCode(p1);
    binCode p2Code = checkCode(p2);
    binCode code = p1Code & p2Code;
    binCode zero;
    zero.reset();
    if (p1Code == p2Code) { //codigos iguais
        if (p1Code == zero) { //Completamente dentro
            return line; //Retorna a reta original, ja que os dois pontos dela estao Completamente dentro da window!
        } else { //comeca e termina no mesmo quadrante, fora da window
            return nullptr;
        }
    } else { //codigos diferentes
        if (code == zero) {//and dos codigos = 0 -> possivel desenho
            double angularCoefficient = getAngularCoefficient(*line); //coeficiente angular da reta
            Point newP1;
            Point newP2;
            if (angularCoefficient != 0) {
                //Tratamento do ponto 1
                switch (p1Code.getDirection()) {
                    case Clipping::north:
                    {// possivelmente vai encostar no topo
                        newP1 = intersectionTop(p1, angularCoefficient);
                        draw = pointIsInsideWindow(newP1);
                        break;
                    }
                    case Clipping::northeast:
                    {
                        Point possiblePoint = intersectionTop(p1, angularCoefficient);
                        if (pointIsInsideWindow(possiblePoint)) {
                            newP1 = possiblePoint;
                            draw = true;
                            break;
                        }
                        possiblePoint = intersectionRight(p1, angularCoefficient);
                        if (pointIsInsideWindow(possiblePoint)) {
                            newP1 = possiblePoint;
                            draw = true;
                            break;
                        }
                        draw = false;
                        break;
                    }
                    case Clipping::east:
                    {
                        newP1 = intersectionRight(p1, angularCoefficient);
                        draw = pointIsInsideWindow(p1);
                        break;
                    }
                    case Clipping::southeast:
                    {
                        Point possiblePoint = intersectionBotton(p1, angularCoefficient);
                        if (pointIsInsideWindow(possiblePoint)) {
                            newP1 = possiblePoint;
                            draw = true;
                            break;
                        }
                        possiblePoint = intersectionRight(p1, angularCoefficient);
                        if (pointIsInsideWindow(possiblePoint)) {
                            newP1 = possiblePoint;
                            draw = true;
                            break;
                        }
                        break;
                    }
                    case Clipping::south:
                    {
                        newP1 = intersectionBotton(p1, angularCoefficient);
                        draw = pointIsInsideWindow(newP1);
                        break;

                    }
                    case Clipping::southwest:
                    {
                        Point possiblePoint = intersectionBotton(p1, angularCoefficient);
                        if (pointIsInsideWindow(possiblePoint)) {
                            newP1 = possiblePoint;
                            draw = true;
                            break;
                        }
                        possiblePoint = intersectionLeft(p1, angularCoefficient);
                        if (pointIsInsideWindow(possiblePoint)) {
                            draw = true;
                            newP1 = possiblePoint;
                            break;
                        }
                        break;
                    }
                    case Clipping::west:
                    {
                        newP1 = intersectionLeft(p1, angularCoefficient);
                        draw = pointIsInsideWindow(newP1);
                        break;
                    }
                    case Clipping::northwest:
                    {
                        Point possiblePoint = intersectionTop(p1, angularCoefficient);
                        if (pointIsInsideWindow(possiblePoint)) {
                            newP1 = possiblePoint;
                            draw = true;
                            break;
                        }
                        possiblePoint = intersectionLeft(p1, angularCoefficient);
                        if (pointIsInsideWindow(possiblePoint)) {
                            newP1 = possiblePoint;
                            draw = true;
                            break;
                        }
                        draw = false;
                        break;
                    }
                    case Clipping::center:
                    {//Ta dentro, nada a se fazer
                        newP1 = p1;
                        draw = true;
                        break;
                    }
                }//Switch codigo ponto inicial
                //Informacoes sobre o ponto inicial prontas. Verificar se ele esta desenhavel
                if (draw) {//primeiro ponto interceptou a window!
                    direction p2Direction = p2Code.getDirection();
                    switch (p2Direction) {
                        case Clipping::north:
                        {// possivelmente vai trombar com o topo
                            newP2 = intersectionTop(newP2, angularCoefficient);
                            draw = pointIsInsideWindow(newP2);
                            break;
                        }
                        case Clipping::northeast:
                        {
                            Point possiblePoint = intersectionTop(p2, angularCoefficient);
                            if (pointIsInsideWindow(possiblePoint)) {
                                draw = true;
                                newP2 = possiblePoint;
                                break;
                            }
                            possiblePoint = intersectionRight(p2, angularCoefficient);
                            if (pointIsInsideWindow(possiblePoint)) {
                                draw = true;
                                newP2 = possiblePoint;
                                break;
                            }
                            draw = false;
                            break;
                        }
                        case Clipping::east:
                        {
                            newP2 = intersectionRight(p2, angularCoefficient);
                            draw = pointIsInsideWindow(newP2);
                            break;
                        }
                        case Clipping::southeast:
                        {
                            Point possiblePoint = intersectionBotton(p2, angularCoefficient);
                            if (pointIsInsideWindow(possiblePoint)) {
                                draw = true;
                                newP2 = possiblePoint;
                                break;
                            }
                            possiblePoint = intersectionRight(p2, angularCoefficient);
                            if (pointIsInsideWindow(possiblePoint)) {
                                draw = true;
                                newP2 = possiblePoint;
                                break;
                            }
                            draw = false;
                            break;
                        }
                        case Clipping::south:
                        {
                            newP2 = intersectionBotton(p2, angularCoefficient);
                            draw = pointIsInsideWindow(newP2);
                            break;

                        }
                        case Clipping::southwest:
                        {
                            Point possiblePoint = intersectionBotton(p2, angularCoefficient);
                            if (pointIsInsideWindow(possiblePoint)) {
                                draw = true;
                                newP2 = possiblePoint;
                                break;
                            }
                            possiblePoint = intersectionLeft(p2, angularCoefficient);
                            if (pointIsInsideWindow(possiblePoint)) {
                                draw = true;
                                newP2 = possiblePoint;
                                break;
                            }
                            draw = false;
                            break;
                        }
                        case Clipping::west:
                        {
                            newP2 = intersectionLeft(p2, angularCoefficient);
                            draw = pointIsInsideWindow(newP2);
                            break;
                        }
                        case Clipping::northwest:
                        {
                            Point possiblePoint = intersectionTop(p2, angularCoefficient);
                            if (pointIsInsideWindow(possiblePoint)) {
                                draw = true;
                                newP2 = possiblePoint;
                                break;
                            }
                            possiblePoint = intersectionLeft(p2, angularCoefficient);
                            if (pointIsInsideWindow(possiblePoint)) {
                                draw = true;
                                newP2 = possiblePoint;
                                break;
                            }
                            draw = false;
                            break;
                        }
                        case Clipping::center:
                        {//Ta dentro, deixa la!
                            newP2 = p2;
                            draw = true;
                            break;
                        }
                    }//Switch codigo 2
                    Line newLine(newP1, newP2);
                    //novaReta = dynamic_cast<Reta *> (Tranformadas::rotacionar(novaReta, janela->obterRotacao(), janela->obterCentro()));
                    return &newLine;
                }/*Desenhar?*/ else {
                    //O primeiro ponto nao interceptou a window nem ta dentro. Nao desenha!!!
                    return nullptr;
                }//Else desenhar
            } //else {//coeficiente Angular = 0
            //                //Tratamento Ponto 1
            //                switch (codigoInicio.obterDirecaoCodigo()) {
            //
            //                    case CohenSutherland::leste:
            //                    {
            //                        novoPonto1 = set2DPoint(janela->obterXMaximo(), reta->obterCoordenadaInicial().y);
            //                        desenhar = true;
            //                        break;
            //                    }
            //                    case CohenSutherland::oeste:
            //                    {
            //                        novoPonto1 = set2DPoint(janela->obterXMinimo(), reta->obterCoordenadaInicial().y);
            //                        break;
            //                    }
            //                    case CohenSutherland::centro:
            //                    {//Ponto ja ta dentro, deixa assim mesmo
            //                        novoPonto1 = set2DPoint(reta->obterCoordenadaInicial().x, reta->obterCoordenadaInicial().y);
            //                        break;
            //                    }
            //                }//switch ponto 1
            //                //Tratamento ponto 2
            //                switch (codigoFim.obterDirecaoCodigo()) {
            //                    case CohenSutherland::leste:
            //                    {
            //                        novoPonto2 = set2DPoint(janela->obterXMaximo(), reta->obterCoordenadaFinal().y);
            //                        desenhar = true;
            //                        break;
            //                    }
            //                    case CohenSutherland::oeste:
            //                    {
            //                        novoPonto2 = set2DPoint(janela->obterXMinimo(), reta->obterCoordenadaFinal().y);
            //                        desenhar = true;
            //                        break;
            //                    }
            //                    case CohenSutherland::centro:
            //                    {//Ponto ja ta dentro, deixa assim mesmo
            //                        novoPonto2 = set2DPoint(reta->obterCoordenadaFinal().x, reta->obterCoordenadaFinal().y);
            //                        desenhar = true;
            //                        break;
            //                    }
            //                }//switch ponto 2
            //                Reta * novaReta = new Reta(objeto->nome, novoPonto1, novoPonto2);
            //                novaReta = dynamic_cast<Reta *> (Tranformadas::rotacionar(novaReta, janela->obterRotacao(), janela->obterCentro()));
            //                return novaReta;
            //            }
        } else {//and dos codigos != 0 -> nao desenha
            return nullptr;
        }
    }//codigos diferentes
    std::cout << "[WARNING][CohenSutherland] Nao coberto por nenhum caso. Comportamento anormal.\n";
    return nullptr;
}
//-----------------------------------------------