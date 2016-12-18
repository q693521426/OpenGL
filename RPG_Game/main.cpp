#include "all_headers.h"

static bool keys[1024] = { false };
static GLfloat lastX = 0;
static GLfloat lastY = 0;
static bool firstMouse = true;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void keyMove(GLfloat deltaTime);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "RPG", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	//glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	initilizeGlobalResource();

	while (!glfwWindowShouldClose(window))
	{

		glfwPollEvents();
		keyMove(timer->getDeltaTime());

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		role->draw();
		monster->draw();
		monsterTextManager->draw();
		scene->draw();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}



void keyMove(GLfloat deltaTime)
{
	if (keys[GLFW_KEY_W])
	{
		role->moveForward(deltaTime);
	}
	if (keys[GLFW_KEY_S])
	{
		role->moveBackward(deltaTime);
	}
	if (keys[GLFW_KEY_A])
	{
		role->moveLeft(deltaTime);

	}
	if (keys[GLFW_KEY_D])
	{
		role->moveRight(deltaTime);
	}

}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
		if (key == GLFW_KEY_P) {
			vec3print(monsterTextManager->fixPos);
			//vec3print(glm::vec3(monsterTextManager->getScreenPos(),1.0f));
			auto opengl_pos = monster->get_all_trans() * glm::vec4(monsterTextManager->fixPos, 1.0f);
			vec4print(opengl_pos);
		}
		else if (key == GLFW_KEY_Z) {
			monsterTextManager->reactWithHit('z');
		}
		else if (key == GLFW_KEY_X) {
			monsterTextManager->reactWithHit('x');
		}
		else if (key == GLFW_KEY_C) {
			monsterTextManager->reactWithHit('c');
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	generalCamera->processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	generalCamera->processMouseScroll(yoffset);
}