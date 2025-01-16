#ifndef DUKE_H_INCLUDED
#define DUKE_H_INCLUDED
#pragma once
#include <vector>
#include <GL/glut.h>
#include <string>
#include "Isaac.h"
#include "Mosca.h"

class Duke{
public:
    Duke();
    void initialize();
    void update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac);
    void render();

    void renderVictoryScreen();
    bool isVictory;

private:
    Vector2D position;
    Vector2D velocity;
    float size;
    float speed;

    float spawnInterval;
    float spawnTimer;
    std::vector<Fly> flies;
    void spawnFly();            // Generar una nueva mosca

    std::vector<GLuint> animations[1];
    int currentFrame = 0;
    float animationTime = 0.0f;
    float frameDuration = 0.2f;

    int hitsTaken = 0; // Contador de impactos
    bool isAlive = true; // Estado de vida del enemigo

    GLuint victoryTexture;
    void loadVictoryTexture();
    GLuint loadTexture(const std::string& path);

    void checkCollisions(std::vector<Projectile>& isaacProjectiles); // Verificar colisiones con proyectiles
};

#endif

