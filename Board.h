#pragma once

#include <string>
#include "Object.h"

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

class Board : public Object
{

public:
	enum  Type
	{
		None = 0,
		Fuel = 1,
		LoseFuel = 2,
		LockSpeed = 4,
		Dead = 8
	};

	static Mesh* Create(std::string name);

	Board() {
		name = "board";
		position = glm::vec3(0, 0, 0);
		mesh = nullptr;
		color = glm::vec3(1, 1, 1);
		movementSpeed = 0.01f;
		acceleration = 0.001f;
		type = None;
		make_purple = false;

	}

	std::string getName() {
		return name;
	}

	Board(Mesh* mesh_, glm::vec3 color_, glm::vec3 position_) : Object(mesh_, position_, color_) {
		name = "board";
		movementSpeed = 0.01f;
		acceleration = 0.001f;
	}

	float getTranslateZ() {
		return translateZ;
	}

	void setTranslateX(float value) {
		translateX = value;
	}

	void setTranslateZ(float value) {
		translateZ = value;
	}

	float getTranslateX() {
		return translateX;
	}

	float getMovementSpeed() {
		return movementSpeed;
	}

	void setMovementSpeed(float value) {
		movementSpeed = value;
	}

	float getAcceleration() {
		return acceleration;
	}

	void setAcceleration(float value) {
		acceleration = value;
	}

	Type getType() {
		return type;
	}

	void setType(Type value) {
		type = value;
	}

	bool getMakePurple() {
		return make_purple;
	}

	void setMakePurple(bool is_purple) {
		make_purple = is_purple;
	}

	void setColor(glm::vec3 purple_color) {
		color = purple_color;
	}

	void Update(float n);
	bool Intersect(glm::vec3 point, glm::vec3 coords);
	void Accelerate() {
		movementSpeed += acceleration;
	}

protected:
	float translateX;
	float translateZ;
	float movementSpeed;
	float acceleration;
	Type type;
	bool make_purple;


};