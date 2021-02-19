#include "Tema2.h"
#include "Board.h"
//#include "Cookie.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>
#include "Transform2D.h"
#include <Laboratoare\Tema2\Object2D.h>

#define RADIUS 0.20
#define MONEY_ROTATION_MAX	2 * M_PI

using namespace std;

float clock1;

std::vector<Board*> boards;
//std::vector<Cookie*> cookies;
int xOffset = 5;
int harmChance = rand() % 100 + 1;
int spawnChance = 30;

bool ok;

Tema2::Tema2()
{

	scaleX_fuel = 2;
	scaleX_fuel_original = 2;

	translateX = 0;
	translateY = 0;
	translateZ = 0;

	translateX_box = 0;
	translateY_box = 0.92;
	translateZ_box = -3;

	scaleX_box = 0.75;
	scaleZ_box = 7;

	jump_acc = 1000.f;
	acc = -3500.f;

	maxY_jump = 2.f;
	current_jump = 0.0f;
	total_jump = 0.0f;

}

Tema2::~Tema2()
{
}

void Tema2::Init()
{

	glm::vec3 corner = glm::vec3(0, 0, 0);
	lightDirection = glm::vec3(0, 0, -1);
	forward = glm::vec3(0, 0, -1);
	lightDirection += forward * 150.0f;

	//camera = new Laborator::Camera1();
	renderCameraTarget = false;

	{
		x_camera = 0.082317;
		y_camera = 2.196695;
		z_camera = 4.729663;

		x_rot_camera = 0.130592;
		y_rot_camera = 0.001136;
		z_rot_camera = -0.000121;

		auto camera = GetSceneCamera();
		camera->SetPosition(glm::vec3(x_camera, y_camera, z_camera));
		camera->SetRotation(glm::vec3(x_rot_camera, y_rot_camera, z_rot_camera));
		camera->Update();
	}

	glm::ivec2 resolution = window->GetResolution();

	jump_acc = (float)resolution.y * 14;
	maxY_jump = (float)resolution.y / 21;

	//{
		//Mesh* mesh = new Mesh("sphere");
		//mesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters/SpaceShip", "space-shuttle-orbiter.obj");
		//meshes[mesh->GetMeshID()] = mesh;
	//}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}


	{
		Mesh* fuel = Object2D::CreateSquare("fuel", corner, square_side, glm::vec3(1, 0, 0), true);
		AddMeshToList(fuel);
	}

	{
		Mesh* square = Object2D::CreateSquare("square", corner, square_side, glm::vec3(1, 1, 1), false);
		AddMeshToList(square);
	}


	// Create a simple cube
	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.2)),
			VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
			VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0), glm::vec3(0.7, 0.7, 0.1)),
			VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
			VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(0.3, 0.5, 0.4)),
			VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.2, 0.9)),
			VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
			VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
		};

		vector<unsigned short> indices =
		{
			0, 1, 2,		1, 3, 2,
			2, 3, 7,		2, 7, 6,
			1, 7, 3,		1, 5, 7,
			6, 7, 4,		7, 5, 4,
			0, 4, 1,		1, 4, 5,
			2, 6, 4,		0, 2, 4,
		};

		CreateMesh("cube", vertices, indices);
	}

	AddMeshToList(Board::Create("platformBlue"));
	AddMeshToList(Board::Create("platformRed"));
	AddMeshToList(Board::Create("platformYellow"));
	AddMeshToList(Board::Create("platformOrange"));
	AddMeshToList(Board::Create("platformGreen"));

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader* shader = new Shader("Tema2");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("Player");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader_Player.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader_Player.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("Environment");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader_Environment.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader_Environment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
{
	unsigned int VAO = 0;
	// TODO: Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// TODO: Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// TODO: Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// TODO: Crete the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// TODO: Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec2)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}


void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
	lightPositionFirst = glm::vec3(translateX - RADIUS, translateY, translateZ);
	lightPositionSecond = glm::vec3(translateX + RADIUS, translateY, translateZ);
	lightDirection /= sqrt(lightDirection.x * lightDirection.x + lightDirection.y * lightDirection.y + lightDirection.z * lightDirection.z);
	clock1 = Engine::GetElapsedTime();

	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
	glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
	forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

	startTime += deltaTimeSeconds;
	if (isNoiseCounter < 5) {
		isNoiseCounter += deltaTimeSeconds;
	}
	else {
		isNoise = 0;
	}


	if (falling == true) {
		translateY -= deltaTimeSeconds / 2;

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(translateX, translateY, translateZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
		RenderSimpleMesh(meshes["sphere"], shaders["Player"], modelMatrix, glm::vec3(1, 0, 1));

		if (translateY < -1.5)
			gameEnd = true;
	}
	else {
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.70, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(translateX, translateY, translateZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));
		RenderSimpleMesh(meshes["sphere"], shaders["Player"], modelMatrix, glm::vec3(1, 0, 1));
	}


	/*
	for (int i = 0; i < cookies.size(); i++) {
		cookies[i]->Update(deltaTimeSeconds * 2.5f, deltaTimeSeconds);
		RenderSimpleMeshCookie(cookies[i]->getMesh(), shaders["Simple"], cookies[i]->getModelMatrix());
	}
	*/


	for (int i = 0; i < boards.size(); i++) {
		if (!full_speed) {
			boards[i]->Update(deltaTimeSeconds * 2.5f);
			RenderSimpleMeshEnvironment(boards[i]->getMesh(), shaders["Environment"], boards[i]->getModelMatrix(), boards[i]->getColor());
		}
		else if (full_speed && powerupTime > 0) {
			boards[i]->Update(deltaTimeSeconds * 6.0f);
			RenderSimpleMeshEnvironment(boards[i]->getMesh(), shaders["Environment"], boards[i]->getModelMatrix(), boards[i]->getColor());
		}
	}

	if (powerupTime > 0) {
		powerupTime -= deltaTimeSeconds;
		currDist += deltaTimeSeconds * 6;
	}
	else {
		currDist += deltaTimeSeconds * 2.5f;
		full_speed = false;
	}

	// n platforme -> n - 1 spatii
	// dupa ce trec primele 7 platforme, urmeaza de la 7 incolo sa generam
	// primele 14 vor avea dist 0 pt ca avem 7 * 3
	// dupa primele 14 platforme, o sa am distanta de 0.5

	spawnChance = initialGenerate ? 30 : 10;

	if (!initialGenerate) {
		GenerateMap();
		currDist = 0;
	}

	if (currDist >= 7 * 3) {
		if (!firstBatch) {
			GenerateMap();
			currDist = 0;
			firstBatch = true;
		}
		else if (firstBatch && !secondBatch) {
			GenerateMap();
			currDist = 0;
			secondBatch = true;
		}
	}

	if (currDist >= 7 * 3 + 6 * 0.5 && firstBatch && secondBatch) {
		GenerateMap();
		currDist = 0;
	}

	//cout << currDist << "\n";

	updateJump(deltaTimeSeconds);


	// fuel
	{
		if (0.5 * scaleX_fuel > 0) {
			scaleX_fuel -= deltaTimeSeconds / 25;
		}
		else if (0.5 * scaleX_fuel <= 0) {
			gameEnd = true;
		}

		glm::mat4 modelMatrix = glm::mat3(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-3, 3, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleX_fuel, 0.25, 0));
		RenderMesh(meshes["fuel"], shaders["VertexNormal"], modelMatrix);


		modelMatrix = glm::mat3(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-3, 3, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 0.25, 0));
		RenderMesh(meshes["square"], shaders["Simple"], modelMatrix);
	}

	{

		if (ok) {
			if (!third) {
				x_camera = translateX;
				y_camera = translateY + 0.75;
				z_camera = translateZ - 1;
				x_rot_camera = 0;
				y_rot_camera = 0;
				z_rot_camera = 0;
				auto camera = GetSceneCamera();
				camera->SetPosition(glm::vec3(x_camera, y_camera, z_camera));
				camera->SetRotation(glm::vec3(x_rot_camera, y_rot_camera, z_rot_camera));
				camera->Update();
			}

			else {
				x_camera = 0;
				y_camera = 1.3;
				z_camera = 3.5;
				x_rot_camera = 0;
				y_rot_camera = 0.001136;
				z_rot_camera = -0.000121;

				auto camera = GetSceneCamera();
				camera->SetPosition(glm::vec3(x_camera, y_camera, z_camera));
				camera->SetRotation(glm::vec3(x_rot_camera, y_rot_camera, z_rot_camera));
				camera->Update();
			}
		}

	}



	PositionsAndSetColor();


	// GAME OVER
	if (gameEnd) {
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cout << "\n>>>>>>> GAME OVER <<<<<<<\n";

		_Exit(0);

	}
}

void Tema2::PositionsAndSetColor() {

	if (isTouch && startTime > 4) {
		falling = true;
	}
	else {
		isTouch = true;
	}

	for (int i = 0; i < boards.size(); i++) {

		if (falling == false && boards[i]->Intersect(boards[i]->getPosition(),
			glm::vec3(translateX, translateY + 0.70, translateZ))) {

			inAir = false;

			if (boards[i]->getMakePurple() == false) {

				boards[i]->setMakePurple(true);

				if (boards[i]->getColor() == glm::vec3(0, 1, 0)) { // green
					if (scaleX_fuel + scaleX_fuel_original / 4 < 2)
						scaleX_fuel = scaleX_fuel + scaleX_fuel_original / 4;
					else
						scaleX_fuel = 2;
				}

				if (boards[i]->getColor() == glm::vec3(1, 1, 0)) {  // yellow
					scaleX_fuel -= scaleX_fuel_original / 8;
				}

				if (boards[i]->getColor() == glm::vec3(1, 0.5f, 0)) { // orange
					full_speed = true;
					powerupTime = 4.5;
					isNoise = 1;
					isNoiseCounter = 0;
				}

				if (boards[i]->getColor() == glm::vec3(1, 0, 0)) {  //red
					boards[i]->Update(10);
					falling = true;
				}

				boards[i]->setColor(glm::vec3(0.73, 0.16, 0.96));
			}
			isTouch = false;
		}
	}
	if (inAir) {
		isTouch = false;
	}
}


void Tema2::GenerateMap()
{
	int initialIndex = initialGenerate ? 7 : 0;

	for (int i = initialIndex; i < 14; ++i) {

		float z = initialGenerate ? -3.5 * i : -3 * i;
		GenerateRow(z);
	}
	initialGenerate = true;
}

void Tema2::GenerateRow(float z)
{
	Board* board = nullptr;
	//Cookie* cookie = nullptr;
	lightPositionThird = glm::vec3(0, 1, z);

	float movementSpeed = 0.05f;
	float acceleration = 0.002f;

	float var0_X = 0;
	float var1_X = -1.5;
	float var2_X = 1.5;


	if (boards.size() > 0) {
		movementSpeed = boards[boards.size() - 1]->getMovementSpeed();
		acceleration = boards[boards.size() - 1]->getAcceleration();
	}
	for (int i = 0; i < 3; i++) {
		int color = rand() % 5 + 1;
		int chance = rand() % 100 + 1;

		if (chance < 100.0f - spawnChance) {
			int harm = rand() % 100 + 1;
			if (harm < 100.0f - harmChance) {
				int fuelChance = rand() % 100 + 1;
				if (fuelChance < 80) {
					board = new Board(meshes["platformBlue"], glm::vec3(0, 0, 1), glm::vec3(0, 0, 0));
				}
				else {
					board = new Board(meshes["platformGreen"], glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
				}
			}
			else {
				int harmLevel = rand() % 100 + 1;
				if (harmLevel < 50) {
					board = new Board(meshes["platformYellow"], glm::vec3(1, 1, 0), glm::vec3(0, 0, 0));
				}
				else {
					if (harmLevel < 85) {
						board = new Board(meshes["platformOrange"], glm::vec3(1, 0.5f, 0), glm::vec3(0, 0, 0));
						//cookie = new Cookie(meshes["Cookie"], glm::vec3(1, 0.5f, 0));
					}
					else {
						board = new Board(meshes["platformRed"], glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
					}

				}
			}

			float current_X;
			if (i == 0) {
				current_X = var0_X;
			}

			if (i == 1) {
				current_X = var1_X;
			}

			if (i == 2) {
				current_X = var2_X;
			}

			board->setPosition(current_X, 0.5f, z);
			//cookie->setPosition(current_X, 0.5f, z);
			boards.push_back(board);
			//cookies.push_back(cookie);
		}


	}

}


void Tema2::FrameEnd()
{
	DrawCoordinatSystem();
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3 color) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// TODO : get shader location for uniform mat4 "Model"
	int location = glGetUniformLocation(shader->program, "Model");

	// TODO : set shader uniform "Model" to modelMatrix
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// TODO : get shader location for uniform mat4 "View"
	int location_view = glGetUniformLocation(shader->program, "View");

	// TODO : set shader uniform "View" to viewMatrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	glUniformMatrix4fv(location_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// TODO : get shader location for uniform mat4 "Projection"
	int location_projection = glGetUniformLocation(shader->program, "Projection");

	// TODO : set shader uniform "Projection" to projectionMatrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	glUniformMatrix4fv(location_projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Noise 
	int locationNoise = glGetUniformLocation(shader->program, "Noise");
	glUniform1f(locationNoise, isNoise);

	int location_clock = glGetUniformLocation(shader->program, "Clock");
	glUniform1f(location_clock, abs(sinf(clock1)) + abs(sinf(clock1)));

	int colorLocation = glGetUniformLocation(shader->program, "object_color");
	glUniform3fv(colorLocation, 1, glm::value_ptr(color));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema2::RenderSimpleMeshCookie(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) {
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// TODO : get shader location for uniform mat4 "Model"
	int location = glGetUniformLocation(shader->program, "Model");

	// TODO : set shader uniform "Model" to modelMatrix
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// TODO : get shader location for uniform mat4 "View"
	int location_view = glGetUniformLocation(shader->program, "View");

	// TODO : set shader uniform "View" to viewMatrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	glUniformMatrix4fv(location_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// TODO : get shader location for uniform mat4 "Projection"
	int location_projection = glGetUniformLocation(shader->program, "Projection");

	// TODO : set shader uniform "Projection" to projectionMatrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	glUniformMatrix4fv(location_projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	//int location_clock = glGetUniformLocation(shader->program, "Clock");
	//glUniform1f(location_clock, abs(sinf(clock1)) + abs(sinf(clock1)));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Tema2::RenderSimpleMeshEnvironment(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;


	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);


	// spot 
	int spot_location = glGetUniformLocation(shader->program, "spot");
	glUniform1i(spot_location, spot);

	//	angle
	int location = glGetUniformLocation(shader->program, "angle");
	glUniform1f(location, angle);

	// Set shader uniforms for light & material properties
	// TODO: Set light position uniform
	int lightPositionFirstLoc = glGetUniformLocation(shader->program, "lightPositionFirst");
	glUniform3f(lightPositionFirstLoc, lightPositionFirst.x, lightPositionFirst.y, lightPositionFirst.z);

	int lightPositionSecondLoc = glGetUniformLocation(shader->program, "lightPositionSecond");
	glUniform3f(lightPositionSecondLoc, lightPositionSecond.x, lightPositionSecond.y, lightPositionSecond.z);

	int lightPositionThirdLoc = glGetUniformLocation(shader->program, "lightPositionThird");
	glUniform3f(lightPositionThirdLoc, lightPositionThird.x, lightPositionThird.y, lightPositionThird.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// TODO: Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// TODO: Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	// add key press event

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (window->KeyHold(GLFW_KEY_A)) {
			translateX -= deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			translateX += deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_SPACE)) {
			jump = true;
			inAir = true;
		}
		else {
			jump = false;
			if (translateY > 0) {
				translateY -= deltaTime * 1.5;
			}
			else {
				inAir = false;
			}

		}
	}


}

void Tema2::updateJump(float deltaTimeSeconds) {
	if (jump) {
		current_jump = jump_acc * deltaTimeSeconds * deltaTimeSeconds / 4;
		total_jump += current_jump;
		translateY += current_jump / 6;

		if (total_jump >= maxY_jump) {
			jump = false;
			current_jump = 0.0f;
			total_jump = 0.0f;
		}
	}
}

void Tema2::OnKeyPress(int key, int mods)
{
	// add key press event

	if (key == GLFW_KEY_X)
		ok = !ok;

	if (key == GLFW_KEY_C) {
		third = !third;
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
		}

	}
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}

