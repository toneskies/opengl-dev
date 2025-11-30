// SpotLight.h
#pragma once
#include "PointLight.h"

class SpotLight : public PointLight {
public:
    SpotLight();

    SpotLight(
        glm::vec3 color,
        GLfloat aIntensity, GLfloat dIntensity,
        glm::vec3 position,
        glm::vec3 direction,
        GLfloat con, GLfloat lin, GLfloat exp,
        GLfloat edge
    );


    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
        GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
        GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
        GLuint edgeLocation);

    void SetFlash(glm::vec3 pos, glm::vec3 dir);

    ~SpotLight();

private:
    glm::vec3 direction;
    GLfloat edge;      // degrees
    GLfloat procEdge;  // cos(radians(edge))
};
