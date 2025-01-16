#ifndef ISAAC_H_INCLUDED
#define ISAAC_H_INCLUDED
#pragma once
#include <vector>
#include <GL/glut.h>
#include <string>
#include <cmath>

struct Vector2D {
    float x, y;
    Vector2D(float x = 0, float y = 0) : x(x), y(y) {}
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    float magnitude() const {
        return sqrt(x * x + y * y);
    }
    Vector2D normalize() const {
        float mag = magnitude();
        return (mag > 0) ? *this * (1.0f / mag) : Vector2D(0, 0);
    }
};

struct Projectile {
    Vector2D position;
    Vector2D velocity;
    float size;

    Projectile(Vector2D pos, Vector2D vel, float sz)
        : position(pos), velocity(vel), size(sz) {}
};

enum Direction { UP, DOWN, LEFT, RIGHT };

class Isaac {
public:
    Isaac();
    int score;
    int health;
    void initialize();
    void update(float deltaTime);
    void render();
    void setDirection(Direction dir);
    void stopMovement();

    Vector2D getPosition() const;
    std::vector<Projectile>& getProjectiles() ;
    void shoot(Direction dir);

    void updateTears(float deltaTime);
    void renderTears();
    void loadTearTexture();
    void renderHealth();

    void takeDamage();
    bool checkCollision(const Vector2D& projectilePos, float projectileSize);
    bool isGameOverState() const { return isGameOver; }
    bool isGameOver;

private:
    Vector2D position;
    Vector2D velocity;
    float size;
    Direction currentDir;
    bool isMoving;

    std::vector<GLuint> animations[4];
    int currentFrame = 0;
    float animationTime = 0.0f;
    float frameDuration = 0.2f;

    GLuint gameOverTexture;
    void loadGameOverTexture();

    std::vector<Projectile> tears;
    GLuint tearTexture;
    float shootCooldown;
    float currentShootCooldown;
    Direction shootDir;
    bool isShooting;
    float tearSpeed;

    GLuint loadTexture(const std::string& path);
};


#endif

