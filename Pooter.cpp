#include <windows.h>
#include <GL/glut.h>
#include "Pooter.h"
#include "config.h"
#include "ResourceManager.h"
#include "Isaac.h"
#include <cmath>
#include <sstream>

Pooter::Pooter()
    : position(200.0f, 500.0f), size(50.0f), shootInterval(0.5f), shootTimer(0.0f),
      projectileSpeed(500.0f), currentFrame(0), animationTime(0.0f), frameDuration(0.003f) {}

void Pooter::initialize() {
    hitsTaken = 0;
    isAlive = true;
    position = {200, 500};
    for (int i = 1; i <= 14; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Pooter/Pooter" << i << ".png";
        GLuint textura = ResourceManager::loadTexture(oss.str());
        if (textura) animations[0].push_back(textura);
    }
}

void Pooter::shoot(const Vector2D& direction) {
    Vector2D normalizedDir = direction.normalize();
    projectiles.push_back(Projectile(position, normalizedDir * projectileSpeed, 20.0f));
}

void Pooter::update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac) {
    if (!isAlive) return; // Si est  muerto, no actualizar

    // Actualizar disparo y proyectiles
    shootTimer += deltaTime;
    if (shootTimer >= shootInterval) {
        Vector2D direction = isaacPosition - position;
        shoot(direction);
        shootTimer = 0.0f;
    }

    // Actualizar y verificar colisiones de proyectiles con Isaac
    for (size_t i = 0; i < projectiles.size(); ) {
        projectiles[i].position.x += projectiles[i].velocity.x * deltaTime;
        projectiles[i].position.y += projectiles[i].velocity.y * deltaTime;

        // Verificar colisi n con Isaac
        if (isaac.checkCollision(projectiles[i].position, projectiles[i].size)) {
            isaac.takeDamage();
            projectiles.erase(projectiles.begin() + i);
        } else {
            ++i;
        }
    }

    // Verificar colisiones con proyectiles de Isaac
    std::vector<Projectile> newIsaacProjectiles;
    for (size_t i = 0; i < isaacProjectiles.size(); ++i) {
        float dx = isaacProjectiles[i].position.x - position.x;
        float dy = isaacProjectiles[i].position.y - position.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance < (isaacProjectiles[i].size + size) / 2.0f) {
            isaac.score += 15; // Incrementar el puntaje
            hitsTaken++;
            if (hitsTaken >= 3) {
                isAlive = false;
                PlaySound("monster_death2.wav", NULL, SND_FILENAME | SND_ASYNC);
                return;
            }
        } else {
            newIsaacProjectiles.push_back(isaacProjectiles[i]);
        }
    }
    isaacProjectiles = newIsaacProjectiles;

    // Verificar colision directa con Isaac
    float dx = isaacPosition.x - position.x;
    float dy = isaacPosition.y - position.y;
    float distance = sqrt(dx * dx + dy * dy);
    if (distance < (size + 65.0f) / 2.0f) { // 65.0f es el tama o de Isaac
        isaac.health--;
    }

    // Actualizar animacion
    animationTime += deltaTime;
    if (animationTime >= frameDuration) {
        currentFrame = (currentFrame + 1) % animations[0].size();
        animationTime = 0.0f;
    }
}


void Pooter::render() {
    if (!isAlive) return; // No renderizar si esta  muerto

    // Renderizar Pooter
    glEnable(GL_TEXTURE_2D);
    GLuint currentTexture = animations[0][currentFrame];
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(position.x, position.y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(position.x + size, position.y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(position.x + size, position.y + size);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(position.x, position.y + size);
    glEnd();

// Renderizar proyectiles
       glBindTexture(GL_TEXTURE_2D, 0);
        glColor3f(0.7,0.04,0.04);
    for (size_t i = 0; i < projectiles.size(); ++i) {
        glBegin(GL_QUADS);
        glVertex2f(projectiles[i].position.x, projectiles[i].position.y);
        glVertex2f(projectiles[i].position.x + projectiles[i].size, projectiles[i].position.y);
        glVertex2f(projectiles[i].position.x + projectiles[i].size, projectiles[i].position.y + projectiles[i].size);
        glVertex2f(projectiles[i].position.x, projectiles[i].position.y + projectiles[i].size);
        glEnd();
    }
   glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_TEXTURE_2D);

}

