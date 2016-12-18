#include "all_headers.h"

FontManager *fontManager = nullptr;
Shader *generalShader = nullptr;
Camera *generalCamera = nullptr;
Timer *timer = nullptr;
Role *role = nullptr;
Role *scene = nullptr;
Role *monster = nullptr;
MonsterTextManager *monsterTextManager = nullptr;


void vec4print(glm::vec4 vec) {
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")" << std::endl;
}

void simplyRenderText(std::string text, GLfloat xpos, GLfloat ypos) {
	fontManager->RenderText(text, xpos, ypos, 0.5f, glm::vec3(0.8f, 0.8f, 0.8f));
}

void initilizeGlobalResource() {
	generalShader = new Shader("shaders/general.vs", "shaders/general.frag");
	generalCamera = new Camera();
	timer = new Timer();
	role = new Role("models/character.obj", glm::vec3(0.0f, 0.0f, 0.0f), 3.0f, 0.1f, glm::vec3(0.7f, 0.8f, 0.2f));
	scene = new Role("models/plat.obj", glm::vec3(0.0f, -1.0f, 0.0f), 3.0f, 100.0f, glm::vec3(1.0f, 0.5f, 0.2f));
	monster = new Role("models/monster.obj", glm::vec3(-0.3f, 0.0f, 0.0f), 3.0f, 0.1f, glm::vec3(0.3f, 0.5f, 0.2f));
	fontManager = new FontManager("shaders/text.vs", "shaders/text.frag");
	monsterTextManager = new MonsterTextManager("zzxc", glm::vec3(0.0f, 1.01f, 0.0f));
}

void useGeneralShader(glm::mat4 all_trans, glm::vec3 object_color) {
	generalShader->use();
	glUniformMatrix4fv(glGetUniformLocation(generalShader->program, "all_trans"), 1, GL_FALSE, glm::value_ptr(all_trans));
	glUniform3fv(glGetUniformLocation(generalShader->program, "object_color"), 1, glm::value_ptr(object_color));
}


void vec3print(glm::vec3 vec) {
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

std::string readWholeFile(std::string fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open()) {
		std::cout << fileName << " can't open" << std::endl;
		std::exit(1);
	}
	std::stringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

Timer::Timer()
	:lastFrame(0.0f)
{}

GLfloat Timer::getDeltaTime()
{
	GLfloat currentFrame = glfwGetTime();
	GLfloat deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	return deltaTime;
}

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	auto vertex = compileShader(
		vertexPath,
		GL_VERTEX_SHADER,
		"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");

	auto fragment = compileShader(
		fragmentPath,
		GL_FRAGMENT_SHADER,
		"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");

	linkShader(
		vertex,
		fragment,
		"ERROR::SHADER::PROGRAM::LINKING_FAILED\n");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(program);
}

GLuint Shader::compileShader(std::string path, int createID, std::string errTag)
{
	std::string shaderCode = readWholeFile(path);
	const GLchar* shaderCodeC = shaderCode.c_str();
	auto shader = glCreateShader(createID);
	glShaderSource(shader, 1, &shaderCodeC, nullptr);
	glCompileShader(shader);

	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << errTag << infoLog << std::endl;
	}

	return shader;
}

void Shader::linkShader(GLuint vertex, GLuint fragment, std::string errTag)
{
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->program, 512, nullptr, infoLog);
		std::cout << errTag << infoLog << std::endl;
	}
}

Camera::Camera()
	:worldUpDir(0.0f, 1.0f, 0.0f),
	yaw(-90.0f),
	pitch(0.0f),
	moveSpeed(3.0f),
	mouseSensitivity(0.25f),
	zoom(45.0f)
{}

glm::mat4 Camera::getViewMatrix()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	auto frontDir = glm::normalize(front);
	auto rightDir = glm::normalize(glm::cross(frontDir, worldUpDir));
	auto upDir = glm::normalize(glm::cross(rightDir, frontDir));

	return glm::lookAt(getPosition(), getPosition() + frontDir, upDir);
}

void Camera::processMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	yaw += xoffset*mouseSensitivity;
	pitch += yoffset*mouseSensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}

void Camera::processMouseScroll(GLfloat yoffset)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= yoffset;
	if (zoom <= 1.0f)
		zoom = 1.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;
}


glm::vec3 Camera::getPosition()
{
	return role->position + glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::mat4 Camera::getPerspectiveMatrix()
{
	return glm::perspective(zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
}

Mesh::Mesh(std::vector<Vertex> vertices_value, std::vector<GLuint> indices_value)
	:vertices(vertices_value), indices(indices_value)
{
	this->setupMesh();
}

void Mesh::Draw(Shader shader)
{
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Position));

	glBindVertexArray(0);
}

Model::Model(GLchar * path)
{
	loadModel(path);
}

void Model::Draw(Shader shader)
{
	for (GLuint i = 0; i < meshes.size(); ++i)
		meshes[i].Draw(shader);
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	for (GLuint i = 0; i < node->mNumMeshes; ++i)
	{
		auto mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->processMesh(mesh, scene));
	}

	for (GLuint i = 0; i < node->mNumChildren; ++i)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	for (GLuint i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;
		vertices.push_back(vertex);
	}

	for (GLuint i = 0; i < mesh->mNumFaces; ++i)
	{
		auto face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(vertices, indices);
}

Role::Role(
	GLchar * objectPath,
	glm::vec3  position_value,
	GLfloat speed_value,
	GLfloat size_value,
	glm::vec3 color_value
)
	:object(objectPath),
	position(position_value),
	speed(speed_value),
	size(size_value),
	color(color_value)
{}

void Role::moveLeft(GLfloat deltaTime)
{
	position += deltaTime * speed *glm::vec3(-1.0f, 0.0f, 0.0f);
}

void Role::moveRight(GLfloat deltaTime)
{
	position += deltaTime * speed *glm::vec3(1.0f, 0.0f, 0.0f);
}

void Role::moveForward(GLfloat deltaTime)
{
	position += deltaTime * speed *glm::vec3(0.0f, 0.0f, -1.0f);
}

void Role::moveBackward(GLfloat deltaTime)
{
	position += deltaTime * speed *glm::vec3(0.0f, 0.0f, 1.0f);
}

void Role::roleUseGeneralShader()
{
	useGeneralShader(get_all_trans(), color);
}

void Role::draw()
{
	roleUseGeneralShader();
	object.Draw(*generalShader);
}

glm::mat4 Role::get_all_trans()
{
	auto model_matrix = glm::scale(glm::translate(glm::mat4(), position), glm::vec3(size, size, size));
	return generalCamera->getPerspectiveMatrix() * generalCamera->getViewMatrix() * model_matrix;
}

FontManager::FontManager(std::string vertexPath, std::string fragmentPath)
	:shader(vertexPath, fragmentPath),
	projection(glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT)))
{
	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	shader.use();
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void FontManager::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

MonsterTextManager::MonsterTextManager(std::string text_value, glm::vec3 fixPos_value)
	:text(text_value), fixPos(fixPos_value)
{
}

void MonsterTextManager::draw()
{
	auto pos = this->getScreenPos();
	simplyRenderText(text, pos.x, pos.y);
}

glm::vec2 MonsterTextManager::coordConvert(glm::vec4 opengl_pos)
{
	return glm::vec2((opengl_pos.x / opengl_pos.z + 1.0f) / 2.0f*WIDTH, (opengl_pos.y / opengl_pos.z + 1.0f) / 2.0f*HEIGHT);
}

glm::vec2 MonsterTextManager::getScreenPos()
{
	auto opengl_pos = monster->get_all_trans() * glm::vec4(fixPos, 1.0f);
	return this->coordConvert(opengl_pos);
}

void MonsterTextManager::reactWithHit(char ch)
{
	if (ch == text[0]) {
		text = text.substr(1, text.size());
	}
}
