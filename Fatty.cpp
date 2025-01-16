#include <windows.h>
#include <GL/glut.h>
#include "config.h"
#include "Fatty.h"
#include "ResourceManager.h"
#include "Isaac.h"
#include "C:Simple OpenGL Image Library/src/SOIL.h"
#include <iostream>
#include <sstream>
#include <cmath>

Fatty::Fatty() : position(600.0f, 400.0f), velocity(0.0f, 0.0f), size(90.0f), speed(200.0f), frameDuration(0.005f)  {}

void Fatty::initialize() {
    hitsTaken = 0;
    isAlive = true;
    position = {600, 400};
    for (int i = 1; i <= 8; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Fatty/Fatty" << i << ".png";
        GLuint textura = ResourceManager::loadTexture(oss.str());
        if (textura) animations[0].push_back(textura);
    }
}

void Fatty::update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac) {
    if (!isAlive) return; // Si está muerto, no actualizar

    // Calcular dirección hacia Isaac
    Vector2D direction = isaacPosition - position;
    direction = direction.normalize();

    // Establecer velocidad en dirección hacia Isaac
    velocity = direction * speed;

    // Actualizar posición
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
            isaac.score += 40; // Incrementar el puntaje
            hitsTaken++;
            if (hitsTaken >= 6) {
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

    // Actualizar animación
    animationTime += deltaTime;
    if (animationTime >= frameDuration) {
        currentFrame = (currentFrame + 1) % animations[0].size();
        animationTime = 0.0f;
    }
}

void Fatty::render() {
    if (!isAlive) return; // No renderizar si está muerto

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
}
