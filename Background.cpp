#include <windows.h>
#include <GL/glut.h>
#include "config.h"
#include "ResourceManager.h"
#include "Background.h"
#include "C:Simple OpenGL Image Library/src/SOIL.h"
#include <cstdio>
#include <string>
#include <sstream>

// Constructor
Background::Background() : frameFondo(0), frameFuego(0) {}

void Background::inicializa() {
    glClearColor(0.53, 0.81, 0.92, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 900.0, 0.0, 729.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Fondo animado
    for (int i = 1; i <= 3; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Fondo/Fondo " << i << ".png";
        std::string ruta = oss.str();
        GLuint textura = ResourceManager::loadTexture(ruta);
        if (textura) texturasFondo.push_back(textura);
    }

    //Fuego animado
    for (int i = 1; i <= 6; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Fuego/Fuego " << i << ".png";
        std::string ruta = oss.str();
        GLuint textura = ResourceManager::loadTexture(ruta);
        if (textura) texturasFuego.push_back(textura);
    }
}

// Dibujar cuadro con textura
void Background::dibujarCuadro(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLuint textura) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x1, y1);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x2, y1);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x2, y2);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x1, y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// Renderizar
void Background::renderizar() {
    // Fondo
    dibujarCuadro(0, 0, 900, 729, texturasFondo[frameFondo]);

    //Fuegos
    dibujarCuadro(100, 125, 175, 200, texturasFuego[frameFuego]);
    dibujarCuadro(725, 550, 800, 650, texturasFuego[frameFuego]);

}

// Actualizar frames de animacion
void Background::actualizar() {
    frameFondo = (frameFondo + 1) % texturasFondo.size();
    frameFuego = (frameFuego + 1) % texturasFuego.size();
}

