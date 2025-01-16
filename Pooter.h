#ifndef POOTER_H_INCLUDED
#define POOTER_H_INCLUDED
#pragma once
#include <vector>
#include <GL/glut.h>
#include "Isaac.h"



class Pooter {
public:
    Pooter();
    void initialize();
    void update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac);
    void render();

private:
    Vector2D position;
    float size;
    float shootInterval;
    float shootTimer;
    std::vector<Projectile> projectiles;
    float projectileSpeed;
    void shoot(const Vector2D& direction);

    std::vector<GLuint> animations[1];
    int currentFrame;
    float animationTime;
    float frameDuration;

    int hitsTaken = 0; // Contador de impactos
    bool isAlive = true; // Estado de vida del enemigo

    GLuint loadTexture(const std::string& path);
};

#endif
