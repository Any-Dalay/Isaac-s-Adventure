#ifndef BACKGROUND_H_INCLUDED
#define BACKGROUND_H_INCLUDED

#include <windows.h>
#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
class Background {
public:
    Background();

    void inicializa();
    void renderizar();
    void actualizar();
    void dibujarCuadro(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLuint textura);

private:
    std::vector<GLuint> texturasFondo;
    std::vector<GLuint> texturasFuego;

    int frameFondo;
    int frameFuego;
};


#endif // BACKGROUND_H_INCLUDED

