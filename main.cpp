#include <windows.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>
#include "ResourceManager.h"
#include "config.h"
#include "C:Simple OpenGL Image Library/src/SOIL.h"
#include "AudioManager.h"
#include "Background.h"
#include "Historia.h"
#include "Isaac.h"
#include "Fatty.h"
#include "Clotty.h"
#include "Pooter.h"
#include "Duke.h"
#include <cmath>
#include <stdexcept>

// Variables globales
AudioManager audioManager;
Background background;
Isaac isaac;
Fatty fatty;
Clotty clotty;
Pooter pooter;
Duke duke;
const int FRAME_DELAY = 100;

enum GameState {
    MENU,
    PLAYING,
    HISTORY,
    INSTRUCTIONS
};

// Variable global para el estado actual
GameState currentState = MENU;

void InicializaOpenGL() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 900.0, 0.0, 729.0);
    glEnable(GL_BLEND);
audioManager.setVolume(1000); // Configura el volumen
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}

// Estructura para los botones del menú
struct MenuButton {
    float x, y;
    float width, height;
    std::string text;

    // Creación de botones
    MenuButton(float x_, float y_, float width_, float height_, const std::string& text_)
        : x(x_), y(y_), width(width_), height(height_), text(text_) {}

    bool isMouseOver(int mouseX, int mouseY) {
        float glMouseY = 729 - mouseY;
        return (mouseX >= x && mouseX <= x + width &&
                glMouseY >= y && glMouseY <= y + height);
    }
};

// Vector de botones del menú inicializado en una función
std::vector<MenuButton> menuButtons;

// Función para inicializar los botones
void initializeButtons() {
    menuButtons.push_back(MenuButton(350, 400, 200, 50, "Jugar"));
    menuButtons.push_back(MenuButton(350, 300, 200, 50, "Historia"));
    menuButtons.push_back(MenuButton(350, 200, 200, 50, "Instrucciones"));
}

// Función para renderizar texto
void renderText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Función para renderizar un botón
void renderButton(const MenuButton& button) {
    // Dibujar el fondo del botón
    glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(button.x, button.y);
    glVertex2f(button.x + button.width, button.y);
    glVertex2f(button.x + button.width, button.y + button.height);
    glVertex2f(button.x, button.y + button.height);
    glEnd();

    // Dibujar el texto del botón
    glColor3f(0.0f, 0.0f, 0.0f);
    float textX = button.x + (button.width - glutBitmapLength(GLUT_BITMAP_HELVETICA_18,
                 (const unsigned char*)button.text.c_str())) / 2;
    float textY = button.y + (button.height + 5) / 2;
    renderText(button.text.c_str(), textX, textY);
}

// Función para renderizar el menú
void renderMenu() {
    glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Título del juego
    glColor3f(1.0f, 1.0f, 1.0f);
    const char* title = "ISAAC'S ADVENTURE";
    float titleX = (900 - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)title)) / 2;
    renderText(title, titleX, 500);

    // Renderizar cada botón
    for (size_t i = 0; i < menuButtons.size(); i++) {
        renderButton(menuButtons[i]);
    }

    glutSwapBuffers();
}

// Función para renderizar las instrucciones
void renderInstructions() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText("CONTROLES DEL VIDEOJUEGO", 250, 600);
    renderText("Usa las teclas W, A, S, D para mover a Isaac.", 200, 500);
    renderText("Usa las flechas del teclado para disparar en la direccion deseada", 200, 450);
    renderText("Derrota a todos los enemigos y evita sus ataques para ganar", 200, 350);
    renderText("¡SUERTE Y DISFRUTA EL JUEGO!", 200, 300);
    renderText("Presiona ESC para volver al menu...", 100, 200);
    glutSwapBuffers();
}

void cleanup() {
    audioManager.stopBackgroundMusic();
}

//Función para inicializar el juego
void initializeGame() {
    isaac.isGameOver = false;
    duke.isVictory = false;
    isaac.score =0;
    isaac.health = 10;
    background.inicializa();
    isaac.initialize();
    fatty.initialize();
    clotty.initialize();
    pooter.initialize();
    duke.initialize();
}

void updateGame() {
    static float lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Solo actualizar si estamos en estado PLAYING
    if (currentState == PLAYING) {
        isaac.update(deltaTime);
    }

    glutPostRedisplay();
}

void Display() {
    switch (currentState) {
        case MENU:
            renderMenu();
            audioManager.stopBackgroundMusic();
            break;
        case PLAYING:
            // Tu código original del juego
            glClear(GL_COLOR_BUFFER_BIT);
            background.renderizar();
            isaac.render();
            fatty.render();
            clotty.render();
            pooter.render();
            duke.render();
            glutSwapBuffers();
            break;
        case HISTORY:
            renderStoryAnimation();
            break;
        case INSTRUCTIONS:
            renderInstructions();
            break;
    }
}

// Función para manejar clicks del mouse
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && currentState == MENU) {
        for (size_t i = 0; i < menuButtons.size(); i++) {
            if (menuButtons[i].isMouseOver(x, y)) {
                switch (i) {
                    case 0: // Jugar
                        currentState = PLAYING;
                        audioManager.playBackgroundMusic((BASE_PATH + "bin/Debug/ambience.wav").c_str());
                        initializeGame();
                        break;
                    case 1: // Historia
                        currentState = HISTORY;
                        break;
                    case 2: // Instrucciones
                        currentState = INSTRUCTIONS;
                        break;
                }
                break;
            }
        }
    }
}

// Función para manejar el teclado
void keyboard(unsigned char key, int x, int y) {
    key = std::tolower(key); // Convertir la tecla a minúscula
    switch (currentState) {
        case MENU:
            if (key == 27) // ESC
            audioManager.stopBackgroundMusic();
                exit(0);
            break;

        case PLAYING:
            switch (key) {
                case 'w': isaac.setDirection(UP); break;
                case 's': isaac.setDirection(DOWN); break;
                case 'a': isaac.setDirection(LEFT); break;
                case 'd': isaac.setDirection(RIGHT); break;
                case 27: // ESC
                    currentState = MENU;
                    break;
            }
            break;

        case HISTORY:
        case INSTRUCTIONS:
            if (key == 27) // ESC
                currentState = MENU;
            break;
    }
}

//Función para actualizar el juego
void Actualizar(int value) {
    float deltaTime = 0.016f;
    // Solo actualizar si estamos en estado PLAYING
    if (currentState == PLAYING) {
        background.actualizar();
        isaac.update(deltaTime);
        fatty.update(deltaTime, isaac.getPosition(), isaac.getProjectiles(), isaac);
        clotty.update(deltaTime, isaac.getPosition(), isaac.getProjectiles(), isaac);
        pooter.update(deltaTime, isaac.getPosition(), isaac.getProjectiles(), isaac);
        duke.update(deltaTime, isaac.getPosition(), isaac.getProjectiles(), isaac);
    }

    glutPostRedisplay();
    glutTimerFunc(FRAME_DELAY, Actualizar, 0);
}

// Función para manejar el teclas especiales
void handleSpecialKeys(int key, int x, int y) {
    // Solo procesar teclas especiales si estamos en estado PLAYING
    if (currentState == PLAYING) {
        switch (key) {
            case GLUT_KEY_UP:
                PlaySound("hit.wav", NULL, SND_FILENAME | SND_ASYNC);
                isaac.shoot(UP);
                break;
            case GLUT_KEY_DOWN:
                PlaySound("hit.wav", NULL, SND_FILENAME | SND_ASYNC);
                isaac.shoot(DOWN);
                break;
            case GLUT_KEY_LEFT:
                PlaySound("hit.wav", NULL, SND_FILENAME | SND_ASYNC);
                isaac.shoot(LEFT);
                break;
            case GLUT_KEY_RIGHT:
                PlaySound("hit.wav", NULL, SND_FILENAME | SND_ASYNC);
                isaac.shoot(RIGHT);
                break;
        }
    }
}

void keyboardUp(unsigned char key, int x, int y) {
        // Solo procesar el evento de soltar tecla si estamos en estado PLAYING
    if (currentState == PLAYING) {
        isaac.stopMovement();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 729);
    glutCreateWindow("Isaac's Adventure");

    InicializaOpenGL();
    initializeButtons();
    initializeGame();

    glutDisplayFunc(Display);
    glutIdleFunc(updateGame);
    glutSpecialFunc(handleSpecialKeys);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouseClick);
    glutTimerFunc(FRAME_DELAY, Actualizar, 0);

    glutMainLoop();
    return 0;
}
