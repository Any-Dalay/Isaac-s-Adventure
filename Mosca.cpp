#include <windows.h>
#include <GL/glut.h>
#define NOMINMAX
#include "Mosca.h"
#include "ResourceManager.h"
#include "config.h"
#include "Isaac.h"
#include "C:Simple OpenGL Image Library/src/SOIL.h"
#include <cmath>

Fly::Fly(const Vector2D& startPosition)
    : position(startPosition), size(30.0f), speed(150.0f), alive(true) {
    initialize();
}

void Fly::initialize() {
    // Cargar la textura de la mosca
    texture = ResourceManager::loadTexture(BASE_PATH + "Assets/Mosca.png");
}

void Fly::update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac) {
    if (!isAlive) return;

    // Actualizar la posición de la mosca
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

        // Limitar posición para no salir de la ventana
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x + size > windowWidth) position.x = windowWidth - size;
    if (position.y + size > windowHeight) position.y = windowHeight - size;

    // Verificar colisiones con proyectiles de Isaac
    std::vector<Projectile> newIsaacProjectiles;
    for (size_t i = 0; i < isaacProjectiles.size(); ++i) {
        float dx = isaacProjectiles[i].position.x - position.x;
        float dy = isaacProjectiles[i].position.y - position.y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < (isaacProjectiles[i].size + size) / 2.0f) {
            isaac.score += 10; // Incrementar el puntaje
            hitsTaken++;
            if (hitsTaken >= 2) {
                isAlive = false;
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
}

void Fly::render() {
    if (!isAlive) return;


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(position.x, position.y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(position.x + size, position.y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(position.x + size, position.y + size);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(position.x, position.y + size);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

