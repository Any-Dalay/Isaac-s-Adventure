#ifndef MOSCA_H_INCLUDED
#define MOSCA_H_INCLUDED
#pragma once
#include <vector>
#include <GL/glut.h>
#include <string>
#include "Isaac.h"

class Fly {
public:
    Fly(const Vector2D& startPosition);
    void initialize();
    void update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac);
    void render();

private:
    Vector2D position;
    Vector2D velocity;
    float size;
    float speed;
    bool alive; // Indica si la mosca sigue activa
    GLuint texture;

    int hitsTaken = 0; // Contador de impactos
    bool isAlive = true; // Estado de vida del enemigo
    GLuint loadTexture(const std::string& path);
};

#endif
