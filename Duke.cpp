#include <windows.h>
#include <GL/glut.h>
#define NOMINMAX
#include "config.h"
#include "Duke.h"
#include "ResourceManager.h"
#include "Isaac.h"
#include "Mosca.h"
#include "C:Simple OpenGL Image Library/src/SOIL.h"
#include <iostream>
#include <sstream>
#include <cmath>


Duke::Duke()
    : position(200.0f, 400.0f),  velocity(0.0f, 0.0f), size(100.0f),  speed(150.0f), spawnInterval(1.0f), spawnTimer(0.0f),
      currentFrame(0), animationTime(0.0f), frameDuration(0.1f), isAlive(true), victoryTexture(0), isVictory(false) {}

void drawTextD(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}


void Duke::loadVictoryTexture() {
    std::ostringstream oss;
    oss << BASE_PATH << "Assets/Victory.png";
    victoryTexture = ResourceManager::loadTexture(oss.str());
    if (!victoryTexture) {
        std::cerr << "Error: No se pudo cargar la textura de victoria: " << oss.str() << std::endl;
    }
}

void Duke::initialize() {
    hitsTaken = 0;
    isAlive = true;
    position = {200, 400};
    flies.clear(); // Limpiar la lista de moscas al inicializar

    // Cargar animaciones del Duke
    for (int i = 1; i <= 4; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Duke/Duke" << i << ".png";
        GLuint textura = ResourceManager::loadTexture(oss.str());
        if (textura) animations[0].push_back(textura);
    }
        // Cargar la textura de victoria
    loadVictoryTexture();
}

void Duke::update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac) {
    if (!isAlive) return;

     // Calcular la dirección hacia Isaac
    Vector2D direction = isaacPosition - position;
    direction = direction.normalize();
        velocity = direction * speed;
    // Actualizar posición
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    // Generar moscas periódicamente
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
        spawnFly();
        spawnTimer = 0.0f;
    }

  // Verificar colisiones con proyectiles de Isaac
    std::vector<Projectile> newIsaacProjectiles;
    for (size_t i = 0; i < isaacProjectiles.size(); ++i) {
        float dx = isaacProjectiles[i].position.x - position.x;
        float dy = isaacProjectiles[i].position.y - position.y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < (isaacProjectiles[i].size + size) / 2.0f) {
            isaac.score += 50; // Incrementar el puntaje
            hitsTaken++;
            if (hitsTaken >= 10) {
                isAlive = false;
                PlaySound("monster_death.wav", NULL, SND_FILENAME | SND_ASYNC);
                return;
            }
        } else {
            newIsaacProjectiles.push_back(isaacProjectiles[i]);
        }
    }
    isaacProjectiles = newIsaacProjectiles;

    // Verificar colisión directa con Isaac
    float dx = isaacPosition.x - position.x;
    float dy = isaacPosition.y - position.y;
    float distance = sqrt(dx * dx + dy * dy);
    if (distance < (size + 65.0f) / 2.0f) { // 65.0f es el tamaño de Isaac
        isaac.takeDamage();
    }

    // Actualizar moscas
    for (size_t i = 0; i < flies.size(); ++i) {
        flies[i].update(deltaTime, isaacPosition, isaacProjectiles, isaac);
    }

    // Actualizar animación
    animationTime += deltaTime;
    if (animationTime >= frameDuration) {
        currentFrame = (currentFrame + 1) % animations[0].size();
        animationTime = 0.0f;
    }
}

void Duke::renderVictoryScreen() {
    isVictory =true;
    if (!victoryTexture) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, victoryTexture);

    // Obtener dimensiones de la ventana
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(windowWidth, 0);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(windowWidth, windowHeight);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0, windowHeight);
    glEnd();

    glDisable(GL_TEXTURE_2D);


    char Menu[50];
        sprintf(Menu, "Presiona ESC para volver al menu...");
        drawTextD(50.0f, windowHeight - 650.0f, Menu);
}

void Duke::render() {
      if (!isAlive) {
        renderVictoryScreen();
        return;
    }
    // Renderizar al Duke
    glEnable(GL_TEXTURE_2D);
    GLuint currentTexture = animations[0][currentFrame];
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(position.x, position.y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(position.x + size, position.y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(position.x + size, position.y + size);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(position.x, position.y + size);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Renderizar las moscas
    for (size_t i = 0; i < flies.size(); ++i) {
        flies[i].render();
    }
}

void Duke::spawnFly() {
    Vector2D flyPosition = position; // Generar moscas cerca del Duke
    flies.push_back(Fly(flyPosition));
}
