#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
#include <vector>


class Tema2 : public SimpleScene
{
public:
	Tema2();
	~Tema2();

	void Init() override;

	Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3 color);
	void RenderSimpleMeshEnvironment(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
	void RenderSimpleMeshCookie(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
	void RenderBoard(float deltaTimeSeconds);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	void updateJump(float deltaTimeSeconds);
	void PositionsAndSetColor();
	void GenerateMap();
	void GenerateRow(float z);

	bool renderCameraTarget;
	//Laborator::Camera1* camera;

	float scaleX_fuel;
	float scaleX_fuel_original;
	float scaleX_box, scaleZ_box;
	float translateX, translateY, translateZ;
	float translateX_box, translateY_box, translateZ_box;
	float translateX_box1, translateY_box1, translateZ_box1;
	float square_side = 0.5;

	glm::mat4 modelMatrix;

	float jump_acc;
	float acc;
	float maxY_jump;
	float current_jump;
	float total_jump;
	bool jump = false;
	float squareSide = 0.5;
	bool gameEnd = false;
	bool ok = true;
	bool third = true;
	bool falling = false;
	bool isTouch = false;
	int numberOfBoards = 7;
	bool needRow = false;
	bool full_speed = false;
	float radius;
	bool initialGenerate;
	float currDist = 0;
	bool firstBatch;
	bool secondBatch;
	float powerupTime;
	float startTime;
	bool inAir;
	float isNoise;
	float isNoiseCounter;


	float x_camera, y_camera, z_camera;
	float x_rot_camera, y_rot_camera, z_rot_camera;


	//Light & material properties

	glm::vec3 lightPositionFirst = glm::vec3(0, 1, 1);
	glm::vec3 lightPositionSecond = glm::vec3(0, 1, 1);
	glm::vec3 lightPositionThird = glm::vec3(0, 1, 1);
	glm::vec3 lightDirection;
	int materialShininess = 30;
	float materialKd = 0.5;
	float materialKs = 0.5;


	int spot = 1;
	int point = 1;
	float angle = glm::radians(45.0);
	float moneyRotation = 0.0f;

	glm::vec3 forward, up, right;


};