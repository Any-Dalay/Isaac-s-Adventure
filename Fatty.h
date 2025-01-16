#ifndef FATTY_H_INCLUDED
#define FATTY_H_INCLUDED
#pragma once
#include <vector>
#include <GL/glut.h>
#include <string>
#include "Isaac.h"


class Fatty {
public:
    Fatty();
    void initialize();
    void update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac);
    void render();

private:
    Vector2D position;
    Vector2D velocity;
    float size;
    float speed;

    std::vector<GLuint> animations[1];
    int currentFrame = 0;
    float animationTime = 0.0f;
    float frameDuration = 0.2f;

     int hitsTaken = 0; // Contador de impactos
    bool isAlive = true; // Estado de vida del enemigo

    GLuint loadTexture(const std::string& path);
};

#endif

