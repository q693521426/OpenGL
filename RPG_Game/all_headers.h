#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cstdlib>
#include<cstddef>
#include <iostream>
#include <vector>
#include<assimp\types.h>

#include <string>
#include <iostream>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <map>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H




class Timer {
public:
	Timer();
	GLfloat getDeltaTime();

	GLfloat lastFrame;
};

class Shader {
public:
	Shader(std::string vertexPath, std::string fragmentPath);
	void use();
	GLuint program;

	GLuint compileShader(std::string path, int createID, std::string errTag);
	void linkShader(GLuint vertex, GLuint fragment, std::string errTag);
};

struct Vertex {
public:
	glm::vec3 Position;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	Mesh(std::vector<Vertex> vertices_value, std::vector<GLuint> indices_value);
	void Draw(Shader shader);

	GLuint VAO, VBO, EBO;

	void setupMesh();
};

class Model {
public:
	Model(GLchar* path);

	void Draw(Shader shader);
	std::vector<Mesh> meshes;

	void loadModel(std::string path);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

class Camera {
public:
	Camera();
	glm::mat4 getViewMatrix();
	void processMouseMovement(GLfloat xoffset, GLfloat yoffset);
	void processMouseScroll(GLfloat yoffset);
	glm::vec3 getPosition();
	glm::mat4 getPerspectiveMatrix();

	glm::vec3 worldUpDir;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;
};

class Role {
public:
	Role::Role(
		GLchar * objectPath,
		glm::vec3  position_value,
		GLfloat speed_value,
		GLfloat size_value,
		glm::vec3 color_value
	);
	void moveLeft(GLfloat deltaTime);
	void moveRight(GLfloat deltaTime);
	void moveForward(GLfloat deltaTime);
	void moveBackward(GLfloat deltaTime);
	void roleUseGeneralShader();
	void draw();
	glm::mat4 get_all_trans();

	Model object;
	glm::vec3 position;
	GLfloat speed;
	GLfloat size;
	glm::vec3 color;
};

struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class FontManager {
public:
	FontManager(std::string vertexPath, std::string fragmentPath);
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;
	Shader shader;
	glm::mat4 projection;
};

//class MonsterTextManager {
//public:
//	std::string text;
//	void draw();
//};

extern FontManager *fontManager;
extern Shader *generalShader;
extern Camera *generalCamera;
extern Timer *timer;
extern Role *role;
extern Role *scene;
extern Role *monster;
const int WIDTH = 800;
const int HEIGHT = 800;

void initilizeGlobalResource();

void useGeneralShader(glm::mat4 all_trans, glm::vec3 object_color);

std::string readWholeFile(std::string fileName);

void vec3print(glm::vec3 vec);

void vec4print(glm::vec4 vec);

void simplyRenderText(std::string text, GLfloat xpos, GLfloat ypos);