// SpotLight.cpp
#include "SpotLight.h"
#include <glm/gtc/matrix_transform.hpp>

SpotLight::SpotLight() : PointLight() {
    direction = glm::vec3(0.0f, -1.0f, 0.0f);
    edge = 20.0f;
    procEdge = cosf(glm::radians(edge));
}

SpotLight::SpotLight(
    glm::vec3 color,
    GLfloat aIntensity, GLfloat dIntensity,
    glm::vec3 pos,
    glm::vec3 dir,
    GLfloat con, GLfloat lin, GLfloat exp,
    GLfloat edg
)
    : PointLight(color, aIntensity, dIntensity, pos, con, lin, exp)
{
    direction = glm::normalize(dir);
    edge = edg;
    procEdge = cosf(glm::radians(edge));
}


void SpotLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
    GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
    GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
    GLuint edgeLocation)
{
    glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);

    glUniform3f(positionLocation, position.x, position.y, position.z);
    glUniform3f(directionLocation, direction.x, direction.y, direction.z);

    glUniform1f(constantLocation, constant);
    glUniform1f(linearLocation, linear);
    glUniform1f(exponentLocation, exponent);

    glUniform1f(edgeLocation, procEdge);
}

void SpotLight::SetFlash(glm::vec3 pos, glm::vec3 dir) {
    position = pos;
    direction = glm::normalize(dir);
}

SpotLight::~SpotLight() {}
