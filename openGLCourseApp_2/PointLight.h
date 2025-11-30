// PointLight.h
#pragma once
#include "Light.h"

class PointLight : public Light {
public:
    PointLight();

    PointLight(
        glm::vec3 color,
        GLfloat aIntensity, GLfloat dIntensity,
        glm::vec3 position,
        GLfloat con, GLfloat lin, GLfloat exp
    );


    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
        GLuint diffuseIntensityLocation, GLuint positionLocation,
        GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

    ~PointLight();

protected:
    glm::vec3 position;
    GLfloat constant, linear, exponent;
};
