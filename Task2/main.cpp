//internal includes
#include "common.h"
#include "ShaderProgram.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>
#include <glm/glm.hpp>

#include "Figure.hpp"

static const GLsizei WIDTH = 640, HEIGHT = 480; //размеры окна

static const GLfloat NEAR = 0.1;
static const GLfloat FAR = 30;

glm::mat4 Projection;
glm::mat4 View;
glm::vec3 pos;

int programMode = 1;

void KeyboardProcessing(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if (action == GLFW_RELEASE)
		return;

	if (key == GLFW_KEY_1)
		programMode = 1;

	if (key == GLFW_KEY_2)
		programMode = 2;

	if (key == GLFW_KEY_F)
		pos.y -= 0.1;

	if (key == GLFW_KEY_R)
		pos.y += 0.1;	

	if (key == GLFW_KEY_A)
		pos.x -= 0.1;

	if (key == GLFW_KEY_D)
		pos.x += 0.1;

	if (key == GLFW_KEY_W)
		pos.z += 0.1;

	if (key == GLFW_KEY_S)
		pos.z -= 0.1;
}

int main(int argc, char** argv) {
	if(!glfwInit())
		return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

  	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "MG3", nullptr, nullptr);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 
	glfwSetKeyCallback(window, KeyboardProcessing);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	int res = 0;

	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: "   << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: "  << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: "	 << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	GLenum gl_error = glGetError();

	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	std::unordered_map<GLenum, std::string> depthShaders;
	depthShaders[GL_VERTEX_SHADER]   = "shadow_vertex.glsl";
	depthShaders[GL_FRAGMENT_SHADER] = "shadow_fragment.glsl";

	std::unordered_map<GLenum, std::string> shaders;
	shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
	shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";

	std::unordered_map<GLenum, std::string> bufferProgram;
	bufferProgram[GL_VERTEX_SHADER]   = "vertex_depth.glsl";
	bufferProgram[GL_FRAGMENT_SHADER] = "fragment_depth.glsl";

	ShaderProgram depthProgram(depthShaders);
	ShaderProgram program(shaders);
	ShaderProgram debug(bufferProgram);

  	glfwSwapInterval(1); // force 60 frames per second
  	glEnable(GL_DEPTH_TEST);
  	glViewport(0, 0, WIDTH, HEIGHT);
  	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
 
 	pos = glm::vec3(0, 1.3, -6.5);

 	Projection[0][0] = 1 / tan(M_PI / 8);
	Projection[1][1] = WIDTH * 1.0 / HEIGHT / tan(M_PI / 8);
	Projection[2][2] = (FAR + NEAR) / (FAR - NEAR);
	Projection[2][3] = 1;
	Projection[3][2] = -2 * FAR * NEAR / (FAR - NEAR);

 	glm::vec3 light(0, 1, 1);
 	std::vector<Figure*> figures;

 	figures.push_back(new Flatness(5, glm::vec3(0, 0, 2), glm::vec3(1, 0, 0)));

 	figures.push_back(new Sphere(0.25, glm::vec3(-0.9, 1.2, 0.8), glm::vec3(1, 0, 0)));
 	figures.push_back(new Sphere(0.6, glm::vec3(-0.3, 0.5, 1.3), glm::vec3(1, 0, 0)));
 	
 	figures.push_back(new Cube(0.5, glm::vec3(100, 0, 2), glm::vec3(1, 0, 0)));

 	figures.push_back(new Cube(0.5, glm::vec3(-2.5, 0, -0.5), glm::vec3(1, 0, 1)));
 	figures.push_back(new Cube(0.5, glm::vec3(2, 0.9, -0.5), glm::vec3(0, 1, 1)));
 	figures.push_back(new Cube(0.5, glm::vec3(2, 0, 4), glm::vec3(0, 0, 1)));

 	figures[0]->SetTexture("../textures/flatness.png");
 	figures[1]->SetTexture("../textures/earth.jpg");
 	figures[2]->SetTexture("../textures/sun.jpg");

 	Figure *Figure = new Flatness(1, glm::vec3(-0.1, -0.05, 0.5), glm::vec3(0, 0, 0));
	Figure->Rotate(glm::vec3(M_PI / 2, 0, 0));

 	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//цикл обработки сообщений и отрисовки сцены каждый кадр
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		light.x = -5;
		light.y = 1 + sin(glfwGetTime()) / 2;
		light.z = 0;

		View = glm::translate(glm::mat4(1.0f), -pos);
		
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, NEAR, FAR);
		glm::mat4 lightView = glm::lookAt(light, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		depthProgram.StartUseShader();						   GL_CHECK_ERRORS;

		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		depthProgram.SetUniform("P", lightProjection);
		depthProgram.SetUniform("V", lightView);

		for (int i = 0; i < figures.size(); i++) {
			depthProgram.SetUniform("T", figures[i]->Translation);
			depthProgram.SetUniform("R", figures[i]->Rotation);

			figures[i]->Draw();
		}

		depthProgram.StopUseShader();
		
		if (programMode == 1) {
			program.StartUseShader();				   GL_CHECK_ERRORS;
	
			glViewport(0, 0, WIDTH, HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			program.SetUniform("P", Projection);
			program.SetUniform("V", View);
			program.SetUniform("LP", lightProjection);
			program.SetUniform("LV", lightView);

			program.SetUniform("lightPos", light);
			program.SetUniform("viewPos", pos);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			program.SetUniform("shadowMap", 1);

			for (int i = 0; i < figures.size(); i++) {
				if (figures[i]->id != -1) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, figures[i]->id);
					program.SetUniform("texture1", 0);
				}

				program.SetUniform("mode", figures[i]->id == -1 ? 1 : 0);
				program.SetUniform("T", figures[i]->Translation);
				program.SetUniform("R", figures[i]->Rotation);

				figures[i]->Draw();
			}

			program.StopUseShader();
		}
		else {
			debug.StartUseShader();

			glViewport(0, 0, WIDTH, HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			debug.SetUniform("P", Projection);
			debug.SetUniform("V", glm::mat4(1.0f));
			debug.SetUniform("T", Figure->Translation);
			debug.SetUniform("R", Figure->Rotation);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			debug.SetUniform("texture1", 0);

			Figure->Draw();

			debug.StopUseShader();
		}

		for (size_t i = 0; i < figures.size(); i++) {
			if (i && i < 4)
				figures[i]->Rotate(0.05, 0.1f);
			
			if (i >= 4)
				figures[i]->Move(0.02f);
		}

		program.StopUseShader();

		glfwSwapBuffers(window); 
	}

	glfwTerminate();

	delete Figure;

	for (size_t i = 0; i < figures.size(); i++)
		delete figures[i];

	return 0;
}
