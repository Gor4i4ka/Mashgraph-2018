#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SOIL/SOIL.h"

GLfloat GetRandomFloat(GLfloat from, GLfloat to) {
	return from + rand() * (to - from) / RAND_MAX;
}

class Figure {
	GLfloat TranslationTime;
	GLfloat RotationTime;

	GLuint vertexArrayObject;
	GLuint vertexBufferObject;
	GLuint vertexColorBufferObject;
	GLuint vertexNormalsBufferObject;
	GLuint texturesCoordBufferObject; 

protected:
	std::vector<GLfloat> vertexArray;
	std::vector<GLfloat> uvArray;
	std::vector<GLfloat> normalArray;
	std::vector<GLfloat> colorArray;

	glm::vec3 movingVector;

	void AddTriangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 color, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3);
	void Configure(glm::vec3 pos);

public:
	void Draw() const;

	GLuint id;
	glm::mat4 Rotation;
	glm::mat4 Translation;
	
	GLuint GetTexture() const { return id; }
	void SetTexture(const std::string& path);

	void Move(GLfloat dt);
	void Rotate(GLfloat dt, GLfloat alpha);
	void Rotate(glm::vec3 r);

	~Figure();
};

class Flatness : public Figure {
public:
	Flatness(GLfloat size, glm::vec3 pos, glm::vec3 color) {
		glm::vec3 A(-size / 2, 0, -size / 2);
		glm::vec3 B(size / 2, 0, -size / 2);
		glm::vec3 C(size / 2, 0, size / 2);
		glm::vec3 D(-size / 2, 0, size / 2);

		AddTriangle(A, C, B, color, glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0));
		AddTriangle(A, D, C, color, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1));

		Configure(pos);
	}
};

class Cube : public Figure {
public:
	Cube(GLfloat a, glm::vec3 pos, glm::vec3 color) {
		glm::vec3 A(0, 0, 0);
		glm::vec3 B(a, 0, 0);
		glm::vec3 C(a, 0, a);
		glm::vec3 D(0, 0, a);
		
		glm::vec3 A1(0, a, 0);
		glm::vec3 B1(a, a, 0);
		glm::vec3 C1(a, a, a);
		glm::vec3 D1(0, a, a);

		AddTriangle(A, B, C, color, glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1));
		AddTriangle(A, C, D, color, glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(0, 1));

		AddTriangle(A1, C1, B1, color, glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0));
		AddTriangle(A1, D1, C1, color, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1));

		AddTriangle(A, A1, B1, color, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1));
		AddTriangle(A, B1, B, color, glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0));

		AddTriangle(D, C1, D1, color, glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 1));
		AddTriangle(D, C, C1, color, glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1));

		AddTriangle(A, D1, A1, color, glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 1));
		AddTriangle(A, D, D1, color, glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1));

		AddTriangle(B, B1, C1, color, glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1));
		AddTriangle(B, C1, C, color, glm::vec2(0, 0), glm::vec2(1, 1), glm::vec2(1, 0));

		Configure(pos);
	}
};

class Sphere : public Figure {
public:
	Sphere(GLfloat r, glm::vec3 pos, glm::vec3 color) {
		int n = 80;

		std::vector<std::vector<glm::vec3>> s(n, std::vector<glm::vec3>(n));
		std::vector<std::vector<glm::vec2>> uv(n, std::vector<glm::vec2>(n));

		for (int i = 0; i < n; i++) {
			GLfloat phi = M_PI * i / (n - 1);

			for (int j = 0; j < n; j++) {
				s[i][j] = glm::vec3(r * sin(phi) * sin(j * M_PI * 2 / n), r * cos(phi), r * sin(phi) * cos(j * M_PI * 2 / n));

				uv[i][j].x = atan2(s[i][j].x, s[i][j].z) / (2 * M_PI) + 0.5;
				uv[i][j].y = 0.5 - (s[i][j].y) / r * 0.5;
			}
		}

		for (int i = 1; i < n; i++) {
			for (int j = 0; j < n; j++) {
				AddTriangle(s[i - 1][j], s[i - 1][(j + 1) % n], s[i][j], color, uv[i - 1][j], uv[i - 1][(j + 1) % n], uv[i][j]);
				AddTriangle(s[i][j], s[i - 1][(j + 1) % n], s[i][(j + 1) % n], color, uv[i][j], uv[i - 1][(j + 1) % n], uv[i][(j + 1) % n]);
			}
		}

		Configure(pos);
	}
};

void Figure::AddTriangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 color, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) {
	glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));

	for (int i = 0; i < 3; i++) {
		vertexArray.push_back(A[i]);
		colorArray.push_back(color[i]);
		normalArray.push_back(normal[i]);
	}

	for (int i = 0; i < 3; i++) {
		vertexArray.push_back(B[i]);
		colorArray.push_back(color[i]);
		normalArray.push_back(normal[i]);
	}

	for (int i = 0; i < 3; i++) {
		vertexArray.push_back(C[i]);
		colorArray.push_back(color[i]);
		normalArray.push_back(normal[i]);
	}
	
	uvArray.push_back(uv1[0]);
	uvArray.push_back(uv1[1]);

	uvArray.push_back(uv2[0]);
	uvArray.push_back(uv2[1]);

	uvArray.push_back(uv3[0]);
	uvArray.push_back(uv3[1]);
}

void Figure::SetTexture(const std::string& path) {
	int width, height;
	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(image);
}

void Figure::Configure(glm::vec3 pos) {
	id = -1;
	
	Rotation = glm::mat4(1.0f);
	RotationTime = 0;

	Translation = glm::mat4(1.0f);
	Translation[3][0] = GetRandomFloat(-0.5, 0.5);
	Translation[3][1] = 2;
	Translation[3][2] = GetRandomFloat(0.5, 8);

	for (int i = 0; i < 3; i++)
		Translation[3][i] = pos[i];

	TranslationTime = 0;
	movingVector = glm::vec3(GetRandomFloat(0.01, 0.06), 0.01, 0);

	vertexArrayObject = 0;
	vertexBufferObject = 0;
	vertexColorBufferObject = 0;
	vertexNormalsBufferObject = 0;
	texturesCoordBufferObject = 0;

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glEnableVertexAttribArray(0);
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexArray.size() * sizeof(GLfloat), vertexArray.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (colorArray.size() > 0) {
		glEnableVertexAttribArray(1);
		glGenBuffers(1, &vertexColorBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexColorBufferObject);
		glBufferData(GL_ARRAY_BUFFER, colorArray.size() * sizeof(GLfloat), colorArray.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); GL_CHECK_ERRORS;
	}

	if (normalArray.size() > 0) {
		glEnableVertexAttribArray(2);
		glGenBuffers(1, &vertexNormalsBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexNormalsBufferObject);
		glBufferData(GL_ARRAY_BUFFER, normalArray.size() * sizeof(GLfloat), normalArray.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (uvArray.size() > 0)  {
		glEnableVertexAttribArray(3);
		glGenBuffers(1, &texturesCoordBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, texturesCoordBufferObject);
		glBufferData(GL_ARRAY_BUFFER, uvArray.size() * sizeof(GLfloat), uvArray.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
}

void Figure::Draw() const {
	glBindVertexArray(vertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, vertexArray.size() / 3);
}

void Figure::Move(GLfloat dt) {
	GLfloat t = glfwGetTime();

	if (t - TranslationTime > dt) {
		TranslationTime = t;

		Translation[3][0] += movingVector.x;
		Translation[3][1] += movingVector.y;
		Translation[3][2] += movingVector.z;

		if (Translation[3][0] > 2) {
			Translation[3][0] -= movingVector.x;
			movingVector.z = movingVector.x;
			movingVector.x = 0;
			movingVector.y = -movingVector.y;
		}
		else if (Translation[3][2] > 4) {
			Translation[3][2] -= movingVector.z;
			movingVector.x = -movingVector.z;
			movingVector.z = 0;
			movingVector.y = -movingVector.y;
		}
		else if (Translation[3][0] < -2.5) {
			Translation[3][0] -= movingVector.x;
			movingVector.z = movingVector.x;
			movingVector.x = 0;
			movingVector.y = -movingVector.y;
		}
		else if (Translation[3][2] < -0.5) {
			Translation[3][2] -= movingVector.z;
			movingVector.x = -movingVector.z;
			movingVector.z = 0;
			movingVector.y = -movingVector.y;
		}
	}
}

void Figure::Rotate(GLfloat dt, GLfloat alpha) {
	GLfloat t = glfwGetTime();

	if (t - RotationTime > dt) {
		RotationTime = t;

		Rotation = glm::rotate(Rotation, alpha, glm::vec3(0, 1, 0));
	}
}

void Figure::Rotate(glm::vec3 r) {
	Rotation = glm::rotate(Rotation, r.x, glm::vec3(1, 0, 0));
	Rotation = glm::rotate(Rotation, r.y, glm::vec3(0, 1, 0));
	Rotation = glm::rotate(Rotation, r.z, glm::vec3(0, 0, 1));
}

Figure::~Figure() {
	glDeleteBuffers(1, &vertexBufferObject);
	glDeleteBuffers(1, &vertexColorBufferObject);
	glDeleteBuffers(1, &vertexNormalsBufferObject);
	glDeleteBuffers(1, &texturesCoordBufferObject);
	glDeleteVertexArrays(1, &vertexArrayObject);
}