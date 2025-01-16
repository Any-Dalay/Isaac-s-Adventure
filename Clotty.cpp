#include <windows.h>
#include <GL/glut.h>
#include "config.h"
#include "Clotty.h"
#include "ResourceManager.h"
#include "Isaac.h"
#include <cmath>
#include <sstream>

Clotty::Clotty()
    : position(600.0f, 300.0f), velocity(0.0f, 0.0f), size(70.0f), speed(150.0f),
      erraticDuration(2.0f), pursuitDuration(3.0f), stateTimer(0.0f), isPursuing(true),
      currentFrame(0), animationTime(0.0f), frameDuration(0.03f),
      shootTimer(0.0f), shootInterval(0.6f), projectileSpeed(400.0f) {}

void Clotty::shoot() {
    // Disparar en las cuatro direcciones diagonales
    float diagonalSpeed = projectileSpeed * 0.707f; // cos(45°) para velocidad diagonal

    // Diagonal superior derecha
    projectiles.push_back(Projectile(
        Vector2D(position.x + size/2, position.y + size/2),
        Vector2D(diagonalSpeed, diagonalSpeed),
        15.0f
    ));

    // Diagonal superior izquierda
    projectiles.push_back(Projectile(
        Vector2D(position.x + size/2, position.y + size/2),
        Vector2D(-diagonalSpeed, diagonalSpeed),
        15.0f
    ));

    // Diagonal inferior derecha
    projectiles.push_back(Projectile(
        Vector2D(position.x + size/2, position.y + size/2),
        Vector2D(diagonalSpeed, -diagonalSpeed),
        15.0f
    ));

    // Diagonal inferior izquierda
    projectiles.push_back(Projectile(
        Vector2D(position.x + size/2, position.y + size/2),
        Vector2D(-diagonalSpeed, -diagonalSpeed),
        15.0f
    ));
}

void Clotty::initialize() {
    hitsTaken = 0;
    isAlive = true;
    position = {600, 300};
    for (int i = 1; i <= 11; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Clotty/Clotty" << i << ".png";
        GLuint textura = ResourceManager::loadTexture(oss.str());
        if (textura) animations[0].push_back(textura);
    }
}

void Clotty::update(float deltaTime, const Vector2D& isaacPosition, std::vector<Projectile>& isaacProjectiles, Isaac& isaac) {
     if (!isAlive) return; // Si está muerto, no actualizar

    // Actualizar temporizador de disparo
    shootTimer += deltaTime;
    if (shootTimer >= shootInterval) {
        shoot();
        shootTimer = 0.0f;
    }

    // Actualizar proyectiles existentes
    for (size_t i = 0; i < projectiles.size(); ++i) {
        projectiles[i].position.x += projectiles[i].velocity.x * deltaTime;
        projectiles[i].position.y += projectiles[i].velocity.y * deltaTime;
    }

    // Eliminar proyectiles fuera de la pantalla
    std::vector<Projectile> newProjectiles;
    for (size_t i = 0; i < projectiles.size(); ++i) {
        if (!(projectiles[i].position.x < 0 ||
              projectiles[i].position.x > glutGet(GLUT_WINDOW_WIDTH) ||
              projectiles[i].position.y < 0 ||
              projectiles[i].position.y > glutGet(GLUT_WINDOW_HEIGHT))) {
            newProjectiles.push_back(projectiles[i]);
        }
    }
    projectiles = newProjectiles;


// Actualizar y verificar colisiones de proyectiles contra Isaac
    for (size_t i = 0; i < projectiles.size(); ) {
        projectiles[i].position.x += projectiles[i].velocity.x * deltaTime;
        projectiles[i].position.y += projectiles[i].velocity.y * deltaTime;

        // Verificar colisión con Isaac
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
            isaac.score += 30; // Incrementar el puntaje
            hitsTaken++;
            if (hitsTaken >= 4) {
                isAlive = false;
                PlaySound("monster_death2.wav", NULL, SND_FILENAME | SND_ASYNC);
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

    // Código movimiento
    stateTimer += deltaTime;
    if (isPursuing && stateTimer >= pursuitDuration) {
        isPursuing = false;
        stateTimer = 0.0f;
        float angle = static_cast<float>(rand() % 360) * (3.14159f / 180.0f);
        velocity = Vector2D(std::cos(angle) * speed, std::sin(angle) * speed);
    } else if (!isPursuing && stateTimer >= erraticDuration) {
        isPursuing = true;
        stateTimer = 0.0f;
    }

    if (isPursuing) {
        Vector2D direction = isaacPosition - position;
        direction = direction.normalize();
        velocity = direction * speed;
    }

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x + size > windowWidth) position.x = windowWidth - size;
    if (position.y + size > windowHeight) position.y = windowHeight - size;

    animationTime += deltaTime;
    if (animationTime >= frameDuration) {
        currentFrame = (currentFrame + 1) % animations[0].size();
        animationTime = 0.0f;
    }
}

void Clotty::render() {
     if (!isAlive) return; // Si está muerto, no actualizar

    // Renderizar Clotty
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
    glColor3f(0.7f, 0.0f, 0.7f);

    for (size_t i = 0; i < projectiles.size(); ++i) {
        glBegin(GL_QUADS);
        glVertex2f(projectiles[i].position.x, projectiles[i].position.y);
        glVertex2f(projectiles[i].position.x + projectiles[i].size, projectiles[i].position.y);
        glVertex2f(projectiles[i].position.x + projectiles[i].size, projectiles[i].position.y + projectiles[i].size);
        glVertex2f(projectiles[i].position.x, projectiles[i].position.y + projectiles[i].size);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}
