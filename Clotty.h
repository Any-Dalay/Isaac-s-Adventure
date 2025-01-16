#ifndef CLOTTY_H_INCLUDED
#define CLOTTY_H_INCLUDED
#pragma once
#include <vector>
#include <GL/glut.h>
#include "Isaac.h"


class Clotty {
public:
    Clotty();
    void initialize();
    void update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac);
    void render();

    std::vector<Projectile> projectiles;
    float shootTimer;
    float shootInterval;
    float projectileSpeed;

private:
    Vector2D position;
    Vector2D velocity;
    float size;
    float speed;
    float erraticDuration;
    float pursuitDuration;
    float stateTimer;
    bool isPursuing;

    std::vector<GLuint> animations[1];
    int currentFrame;
    float animationTime;
    float frameDuration;

    void shoot();
    int hitsTaken = 0; // Contador de impactos
    bool isAlive = true; // Estado de vida del enemigo

    GLuint loadTexture(const std::string& path);
};

#endif

