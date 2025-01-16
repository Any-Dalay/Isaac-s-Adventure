#include <windows.h>
#include <GL/glut.h>
#include "config.h"
#include "Isaac.h"
#include "ResourceManager.h"
#include "C:Simple OpenGL Image Library/src/SOIL.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>


Isaac::Isaac()
    : position(300.0f, 300.0f), velocity(0.0f, 0.0f), size(65.0f),
      currentDir(DOWN), isMoving(false), shootCooldown(0.3f),
      currentShootCooldown(0.0f), tearSpeed(300.0f), isShooting(false), health(10), isGameOver(false), score(0) {}

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void Isaac::loadGameOverTexture() {
    std::ostringstream oss;
    oss << BASE_PATH << "Assets/GameOver.png";
    gameOverTexture = ResourceManager::loadTexture(oss.str());
    if (!gameOverTexture) {
        std::cerr << "Error: No se pudo cargar la textura de Game Over" << std::endl;
    }
}

void Isaac::loadTearTexture() {
    std::ostringstream oss;
    oss << BASE_PATH << "Assets/Lagrima.png";
    tearTexture = ResourceManager::loadTexture(oss.str());
    if (!tearTexture) {
        std::cerr << "Error: No se pudo cargar la textura de la lágrima: " << oss.str() << std::endl;
    }
}

void Isaac::initialize() {

    position = {300, 300};
    // Cargar frames para la animación de movimiento hacia diferentes direcciones
    for (int i = 1; i <= 11; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Issac/AbajoCaminar/Abajo " << i << ".png";
        GLuint textura = ResourceManager::loadTexture(oss.str());
        if (textura) animations[DOWN].push_back(textura);
    }

    for (int i = 1; i <= 10; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Issac/ArribaCaminar/Arriba " << i << ".png";
        GLuint textura = ResourceManager::loadTexture(oss.str());
        if (textura) animations[UP].push_back(textura);
    }

    for (int i = 1; i <= 10; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Issac/IzquierdaCaminar/Izquierda " << i << ".png";
        GLuint textura = ResourceManager::loadTexture(oss.str());
        if (textura) animations[LEFT].push_back(textura);
    }

    for (int i = 1; i <= 10; ++i) {
        std::ostringstream oss;
        oss << BASE_PATH << "Assets/Issac/DerechaCaminar/Derecha " << i << ".png";
        GLuint textura = ResourceManager::loadTexture(oss.str());
        if (textura) animations[RIGHT].push_back(textura);
    }

    loadTearTexture();
    loadGameOverTexture();


}

void Isaac::takeDamage() {
    health--;
    if (health == 0) {
        isGameOver = true;
    }
}

bool Isaac::checkCollision(const Vector2D& projectilePos, float projectileSize) {
    // Calcular distancia entre centros
    float dx = (position.x + size/2) - (projectilePos.x + projectileSize/2);
    float dy = (position.y + size/2) - (projectilePos.y + projectileSize/2);
    float distance = sqrt(dx*dx + dy*dy);

    // Colisión si la distancia es menor que la suma de los radios
    return distance < (size + projectileSize) / 2;
}

void Isaac::shoot(Direction dir) {
    if (currentShootCooldown > 0) return;

    // Determinar la velocidad de la lágrima según la dirección
    Vector2D tearVelocity(0.0f, 0.0f);
    switch (dir) {
        case UP:    tearVelocity = Vector2D(0.0f, tearSpeed); break;
        case DOWN:  tearVelocity = Vector2D(0.0f, -tearSpeed); break;
        case LEFT:  tearVelocity = Vector2D(-tearSpeed, 0.0f); break;
        case RIGHT: tearVelocity = Vector2D(tearSpeed, 0.0f); break;
    }

    // Calcular la posición inicial de la lágrima (centro del personaje)
    float tearX = position.x + (size - 20.0f) / 2;
    float tearY = position.y + (size - 20.0f) / 2;

    // Crear nueva lágrima
    tears.push_back(Projectile(Vector2D(tearX, tearY), tearVelocity, 20.0f));
    currentShootCooldown = shootCooldown;
}

void Isaac::update(float deltaTime) {
     if (isGameOver) return;  // No actualizar si está en Game Over
    if (isMoving) {
        // Calcular la nueva posición tentativa
        float newX = position.x + velocity.x * deltaTime;
        float newY = position.y + velocity.y * deltaTime;
        // Obtener el tamaño de la ventana
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // Limitar la posición X
        if (newX < 0) {
            newX = 0;
        } else if (newX + size > windowWidth) {
            newX = windowWidth - (size+100);
        }

        // Limitar la posición Y
        if (newY < 0) {
            newY = 0;
        } else if (newY + size > windowHeight) {
            newY = windowHeight - (size+100);
        }

        // Actualizar la posición con los valores limitados
        position.x = newX;
        position.y = newY;

        // Actualizar la animación
        animationTime += deltaTime;
        if (animationTime >= frameDuration) {
            currentFrame = (currentFrame + 1) % animations[currentDir].size();
            animationTime = 0.0f;
        }
    }

    if (currentShootCooldown > 0) {
        currentShootCooldown -= deltaTime;
    }

    updateTears(deltaTime);
}

void Isaac::updateTears(float deltaTime) {
    // Actualizar posición de las lágrimas
    for (size_t i = 0; i < tears.size(); ++i) {
        tears[i].position.x += tears[i].velocity.x * deltaTime;
        tears[i].position.y += tears[i].velocity.y * deltaTime;
    }

    // Eliminar lágrimas fuera de la pantalla
    std::vector<Projectile> newTears;
    for (size_t i = 0; i < tears.size(); ++i) {
        if (!(tears[i].position.x < 0 ||
              tears[i].position.x > glutGet(GLUT_WINDOW_WIDTH) ||
              tears[i].position.y < 0 ||
              tears[i].position.y > glutGet(GLUT_WINDOW_HEIGHT))) {
            newTears.push_back(tears[i]);
        }
    }
    tears = newTears;
}

void Isaac::renderTears() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tearTexture);

    for (size_t i = 0; i < tears.size(); ++i) {
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(tears[i].position.x, tears[i].position.y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(tears[i].position.x + tears[i].size, tears[i].position.y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(tears[i].position.x + tears[i].size, tears[i].position.y + tears[i].size);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(tears[i].position.x, tears[i].position.y + tears[i].size);
        glEnd();
    }
}



void Isaac::render() {

    if (isGameOver) {
        // Renderizar pantalla de Game Over
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, gameOverTexture);

        // Obtener dimensiones de la ventana
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(windowWidth, 0);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(windowWidth, windowHeight);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0, windowHeight);
        glEnd();

        glDisable(GL_TEXTURE_2D);

        // Mostrar puntaje sobre el fondo
        glColor3f(1.0f, 1.0f, 1.0f);
        char scoreText[50];
        sprintf(scoreText, "Score: %d", score);
        drawText(400.0f, windowHeight - 60.0f, scoreText);
        char Menu[50];
        sprintf(Menu, "Presiona ESC para volver al menu...");
        drawText(50.0f, windowHeight - 650.0f, Menu);


        return;
    }
    // Renderizar Isaac
    glEnable(GL_TEXTURE_2D);
    GLuint currentTexture = animations[currentDir][currentFrame];
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(position.x, position.y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(position.x + size, position.y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(position.x + size, position.y + size);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(position.x, position.y + size);
    glEnd();

    // Renderizar lágrimas
    renderTears();

    // Renderizar la vida y score de Isaac
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    char healthText[50];
    sprintf(healthText, "Vida: %d", health);
    drawText(10.0f, glutGet(GLUT_WINDOW_HEIGHT) - 30.0f, healthText); // Muestra la vida

    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    drawText(10.0f, glutGet(GLUT_WINDOW_HEIGHT) - 60.0f, scoreText);

    glDisable(GL_TEXTURE_2D);

}


void Isaac::setDirection(Direction dir) {
    currentDir = dir;
    isMoving = true;

    // Establece la velocidad según la dirección.
    switch (dir) {
        case UP:    velocity = {0.0f, 100.0f}; break;
        case DOWN:  velocity = {0.0f, -100.0f}; break;
        case LEFT:  velocity = {-100.0f, 0.0f}; break;
        case RIGHT: velocity = {100.0f, 0.0f}; break;
    }
}

void Isaac::stopMovement() {
    isMoving = false;
    velocity = {0, 0};
}

Vector2D Isaac::getPosition() const {
    return position;
}

std::vector<Projectile>& Isaac::getProjectiles() {
    return tears;
}
