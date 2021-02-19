#include "Board.h"
#include <include/glm.h>
#include <algorithm>
#include <Core/Engine.h>
#include <iostream>

Mesh* Board::Create(std::string name) {
	Mesh* mesh = new Mesh(name);
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
	return mesh;
}


void Board::Update(float n)
{
	position.z += n;
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.02f, 3));
}

bool Board::Intersect(glm::vec3 point, glm::vec3 coords)
{
	float radius = 0.20f;
	float w = 1;
	float h = 3;
	float depth = 0.02f;
	float min_X = point.x - w / 2;
	float max_X = point.x + w / 2;
	float min_Z = point.z - h / 2;
	float max_Z = point.z + h / 2;
	float min_Y = point.y - depth / 2;
	float max_Y = point.y + depth / 2;
	float player_min_Y = coords.y - radius;

	//std::cout << point.x << " " << point.y << " " << point.z << "Limite platforma " << min_X << " "
	//			<< min_Y << " " << min_Z << " " << max_X << " " << max_Y << " " << max_Z <<"\n";
	//std::cout << "Player:" << "\n";
	//std::cout << coords.x << " " << coords.y << " " << coords.z << "\n";


	if (player_min_Y <= max_Y && 
		(coords.x >= min_X && coords.x <= max_X) &&
		(coords.z >= min_Z && coords.z <= max_Z)) {
		//std::cout << "Intersectat\n";
		return true;
	}
	
	return false;
}





