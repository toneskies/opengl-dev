// PointLight.cpp
#include "PointLight.h"

PointLight::PointLight() : Light() {
    position = glm::vec3(0.0f);
    constant = 1.0f;
    linear = 0.09f;
    exponent = 0.032f;
}

PointLight::PointLight(
    glm::vec3 color,
    GLfloat aIntensity, GLfloat dIntensity,
    glm::vec3 pos,
    GLfloat con, GLfloat lin, GLfloat exp
) : Light(color.r, color.g, color.b, aIntensity, dIntensity)
{
    position = pos;
    constant = con;
    linear = lin;
    exponent = exp;
}

void PointLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
    GLuint diffuseIntensityLocation, GLuint positionLocation,
    GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation)
{
    glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);

    glUniform3f(positionLocation, position.x, position.y, position.z);
    glUniform1f(constantLocation, constant);
    glUniform1f(linearLocation, linear);
    glUniform1f(exponentLocation, exponent);
}

PointLight::~PointLight() {}
