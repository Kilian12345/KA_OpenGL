#pragma once
#include "Light.h"
#include "OmniShadowMap.h"

#include <vector>

class PointLight : public Light
{
public: 
	PointLight();
	PointLight(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat near, GLfloat far,
		GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat con, GLfloat lin, GLfloat exp,
		GLfloat rad);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
		GLuint diffuseIntensityLocation, GLuint positionLocation,
		GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
		GLuint radiusLocation);

	glm::vec3 GetColor();

	std::vector <glm::mat4> CalculateLightTransform();
	GLfloat GetFarPlane();
	glm::vec3 GetPosition();

	~PointLight();


	glm::vec3 position;

protected:
	GLfloat constant, linear, exponent, radius;

	GLfloat farPlane;
};

