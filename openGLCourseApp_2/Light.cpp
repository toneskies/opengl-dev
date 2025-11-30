// Light.cpp
#include "Light.h"

Light::Light() {
    colour = glm::vec3(1.0f);
    ambientIntensity = 0.1f;
    diffuseIntensity = 0.5f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue,
    GLfloat aIntensity, GLfloat dIntensity)
{
    colour = glm::vec3(red, green, blue);
    ambientIntensity = aIntensity;
    diffuseIntensity = dIntensity;
}

Light::~Light() {}
