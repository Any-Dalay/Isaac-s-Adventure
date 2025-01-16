#ifndef RESOURCEMANAGER_H_INCLUDED
#define RESOURCEMANAGER_H_INCLUDED

#include <windows.h>
#include <string>
#include <map>
#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include "C:Simple OpenGL Image Library/src/SOIL.h"

class ResourceManager {
public:
    static GLuint loadTexture(const std::string& filePath) {
        if (textures.find(filePath) != textures.end()) {
            return textures[filePath]; // Devuelve la textura si ya fue cargada
        }

        GLuint texture = SOIL_load_OGL_texture(filePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
        if (texture) {
            textures[filePath] = texture;
        } else {
            printf("Error al cargar textura: %s\n", filePath.c_str());
        }

        return texture;
    }

    static void clearTextures() {
    for (std::map<std::string, GLuint>::iterator it = textures.begin(); it != textures.end(); ++it) {
        glDeleteTextures(1, &(it->second)); // Libera la memoria de la textura
    }
    textures.clear();
    }

private:
    static std::map<std::string, GLuint> textures; // Almacena las texturas cargadas
};


#endif // RESOURCEMANAGER_H_INCLUDED
