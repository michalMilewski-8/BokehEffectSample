#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "mesh.hpp"
#include "utils.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Program.h"
#include "Object.h"

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr float Pi = 3.14159265359;

constexpr int N = 144;

std::shared_ptr<Program> program;
std::vector<Object> objects;
std::shared_ptr<Model> dragon, fox, car, bob, sniper, cat;

std::vector<glm::vec2> CreateOffsets(float angle) {
	std::vector<glm::vec2> offsets = std::vector<glm::vec2>(N);
	const float aspecRatio = WindowWidth / (float)WindowHeight;

	const float radius = 0.5f;

	glm::vec2 pt = { radius * glm::cos(angle),radius * glm::sin(angle) };

	pt.x /= aspecRatio;

	for (int i = 0; i < N; i++) {
		float t = i / (N - 1.0f);
		offsets[i] = -pt + 2.0f * pt * t;
	}
	return offsets;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	//TBD
}

void RenderGui()
{
	ImGui::Begin("Menu");

	ImGui::SliderFloat("A", &program->A, 0.001f, 1.0f);
	ImGui::SliderFloat("S1", &program->S1, program->Far * 0.4f, 0.99f * program->Far);
	ImGui::SliderFloat("f", &program->f, 0.001f, 2.0f);
	//ImGui::SliderFloat("Far", &program->Far, 0.0f, 100.0f);
	ImGui::SliderFloat("maxCoc", &program->maxCoc, 0.0001f, 0.3f);
	//ImGui::SliderFloat("sensorHeight", &program->sensorHeight, 0.001f, 0.1f);

	ImGui::End();
}
int main() {

	// WINDOW SETUP

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	program = std::make_shared<Program>();
	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Motion Blur", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(100 / 255.0f, 222 / 255.0f, 250 / 255.0f, 1);

	glfwSetCursorPosCallback(window, mouse_callback);

	double time = glfwGetTime();

	// OBJECT CREATION

	Mesh quad = makeQuad();
	Mesh sky = makeCube(10.0f);
	GLuint firstProgram = loadShaders("assets/first.vert", "assets/first.frag");
	GLuint skyProgram = loadShaders("assets/sky.vert", "assets/sky.frag");
	GLuint quadProgram = loadShaders("assets/quad.vert", "assets/quad.frag");
	// TODO 2.1: Utworzenie oraz wczytanie programu odpowiedzialnego za rozmycie obrazu
	GLuint blurProgram = loadShaders("assets/quad.vert", "assets/blur.frag");
	GLuint blur2Program = loadShaders("assets/quad.vert", "assets/blur2.frag");
	GLuint blur3Program = loadShaders("assets/quad.vert", "assets/blur3.frag");
	GLuint skyTexture = loadTexture("assets/pngegg2.png");
	dragon = std::make_shared<Model>
		("assets/alduin-dragon-obj/alduin-dragon.obj",
			"assets/alduin-dragon-obj/alduin.jpg"
			, 1.0f);

	fox = std::make_shared<Model>
		("assets/low-poly-fox-by-pixelmannen-obj/low-poly-fox-by-pixelmannen.obj",
			"assets/low-poly-fox-by-pixelmannen-obj/texture.png"
			, 1.0f);

	car = std::make_shared<Model>
		("assets/camero-2010-low-poly-obj/camero-2010-low-poly.obj",
			"assets/camero-2010-low-poly-obj/camero_map.png"
			, 1.0f);

	bob = std::make_shared<Model>
		("assets/meet-bob-obj/meet-bob.obj",
			"assets/meet-bob-obj/rp_eric_rigged_001_dif.jpg"
			, 1.0f);

	sniper = std::make_shared<Model>
		("assets/ksr-29-sniper-rifle-obj/ksr-29-sniper-rifle.obj",
			"assets/ksr-29-sniper-rifle-obj/Sniper_KSR_29_Col.jpg"
			, 1.0f);

	cat = std::make_shared<Model>
		("assets/Cat/12221_Cat_v1_l3.obj",
			"assets/Cat/Cat_diffuse.jpg"
			, 1.0f);

	glm::vec3 cameraPos = { 0, 0, -5 };
	glm::vec2 cameraRotationDegrees = { 0,0 };
	double cursorX, cursorY;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	int spaceState = GLFW_RELEASE;
	bool running = true;


	glm::mat4 viewMatrix = glm::mat4(1);
	glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(45.f), WindowWidth / (float)WindowHeight, 0.1f, 40.0f);
	float modelAngle = 0;

	// FRAMEBUFFER CREATION

	GLuint gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	GLuint gPos, gNorm, gAlb, gDepth;

	glGenTextures(1, &gPos);
	glBindTexture(GL_TEXTURE_2D, gPos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPos, 0);

	glGenTextures(1, &gNorm);
	glBindTexture(GL_TEXTURE_2D, gNorm);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNorm, 0);

	glGenTextures(1, &gAlb);
	glBindTexture(GL_TEXTURE_2D, gAlb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlb, 0);

	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	// TODO 4: Dodanie tekstury predkosci do listy attachmentow
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(sizeof(attachments) / sizeof(*attachments), attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer incomplete" << '\n';
	}

	// TODO 1: Utworzenie framebuffera na rozmywany obraz
	GLuint gBlurBuffer, gBlurBuffer2, gBlurBuffer3, gBlurBuffer4;
	glGenFramebuffers(1, &gBlurBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer);

	GLuint gCol, gColB, gColA, gColC;
	glGenTextures(1, &gCol);
	glBindTexture(GL_TEXTURE_2D, gCol);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gCol, 0);
	glDrawBuffers(1, attachments);


	// MM: 3 bufor
	glGenFramebuffers(1, &gBlurBuffer3);
	glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer3);

	glGenTextures(1, &gColB);
	glBindTexture(GL_TEXTURE_2D, gColB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gColB, 0);
	glDrawBuffers(1, attachments);

	// MM: 2 bufor
	glGenFramebuffers(1, &gBlurBuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer2);

	glGenTextures(1, &gColA);
	glBindTexture(GL_TEXTURE_2D, gColA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gColA, 0);
	glDrawBuffers(1, attachments);


	// MM: 4 bufor
	glGenFramebuffers(1, &gBlurBuffer4);
	glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer4);

	glGenTextures(1, &gColC);
	glBindTexture(GL_TEXTURE_2D, gColC);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gColC, 0);
	glDrawBuffers(1, attachments);

	// MAIN LOOP

	auto offsets1 = CreateOffsets((Pi / 3.0f));
	auto offsets2 = CreateOffsets(-(Pi / 3.0f));
	auto offsets3 = CreateOffsets(0.0f);
	auto offsets4 = CreateOffsets(0.0f);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//objects.push_back(Object(fox, { 0.0f,0.0f,0.0f }));
	//objects.push_back(Object(dragon, { 0.0f,0.0f,0.0f }));
	//objects.push_back(Object(car, { 0.0f,0.0f,0.0f }));
	//objects.push_back(Object(bob, { 0.0f,0.0f,0.0f }));
	//objects.push_back(Object(sniper, { 0.0f,0.0f,0.0f }));
	objects.push_back(Object(cat, { 0.0f,0.0f,0.0f }));

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();
		RenderGui();

		double newTime = glfwGetTime();
		float dt = static_cast<float>(newTime - time);
		time = newTime;

		int newSpaceState = glfwGetKey(window, GLFW_KEY_SPACE);
		if (newSpaceState == GLFW_PRESS && spaceState == GLFW_RELEASE) running = !running;
		spaceState = newSpaceState;


		// OBJECT POSITION UPDATE

		float scale = 0.001f;
		if (running) {
			for (auto& obj : objects) obj.UpdateObject(dt);
		}

		// CAMERA UPDATE

		double newCursorX, newCursorY;
		glfwGetCursorPos(window, &newCursorX, &newCursorY);
		double cursorDX = newCursorX - cursorX;
		double cursorDY = newCursorY - cursorY;
		cursorX = newCursorX;
		cursorY = newCursorY;

		bool rotate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		bool goForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		bool goBack = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
		bool goLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		bool goRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		bool goUp = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
		bool goDown = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
		bool moveCamera = rotate || goForward || goBack || goLeft || goRight || goUp || goDown;

		if (rotate) {
			cameraRotationDegrees.x -= static_cast<float>(cursorDY) / 5;
			cameraRotationDegrees.y -= static_cast<float>(cursorDX) / 5;
			if (cameraRotationDegrees.x > 90) cameraRotationDegrees.x = 90;
			if (cameraRotationDegrees.x < -90) cameraRotationDegrees.x = -90;
			while (cameraRotationDegrees.y > 360) cameraRotationDegrees.y -= 360;
			while (cameraRotationDegrees.y < 0) cameraRotationDegrees.y += 360;
		}

		if (!ImGui::GetIO().WantCaptureMouse) {
			viewMatrix = glm::mat4(1);
			viewMatrix = glm::rotate(viewMatrix, glm::radians(cameraRotationDegrees.x), { 1, 0, 0 });
			viewMatrix = glm::rotate(viewMatrix, glm::radians(cameraRotationDegrees.y), { 0, 1, 0 });

			glm::vec4 cameraFront = { 0, 0, 1, 0 };
			glm::vec4 cameraUp = { 0, 1, 0, 0 };
			glm::vec4 cameraSide = { 1, 0, 0, 0 };

			cameraFront = cameraFront * viewMatrix;
			//cameraUp = cameraUp * viewMatrix;
			cameraSide = cameraSide * viewMatrix;

			const float movementSpeed = 5.0f * dt;
			if (goForward) cameraPos += glm::vec3(cameraFront) * movementSpeed;
			if (goBack) cameraPos -= glm::vec3(cameraFront) * movementSpeed;
			if (goLeft) cameraPos += glm::vec3(cameraSide) * movementSpeed;
			if (goRight) cameraPos -= glm::vec3(cameraSide) * movementSpeed;
			if (goUp) cameraPos -= glm::vec3(cameraUp) * movementSpeed;
			if (goDown) cameraPos += glm::vec3(cameraUp) * movementSpeed;

			viewMatrix = glm::translate(viewMatrix, cameraPos);
		}
		// RENDERING STAGE 1

		// STAGE 1 - object

		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// STAGE 1 - skybox

		glUseProgram(skyProgram);
		glDisable(GL_CULL_FACE);
		glUniformMatrix4fv(11, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(12, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

		glUniform1i(20, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skyTexture);
		glBindVertexArray(sky.vao);
		glDrawArrays(GL_TRIANGLES, 0, sky.count);


		glUseProgram(firstProgram);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		for (auto& obj : objects)
		{
			glUniformMatrix4fv(10, 1, GL_FALSE, glm::value_ptr(obj.modelMatrix));
			glUniformMatrix4fv(11, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(12, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
			glUniformMatrix4fv(13, 1, GL_FALSE, glm::value_ptr(glm::inverse(viewMatrix)));
			glUniformMatrix4fv(14, 1, GL_FALSE, glm::value_ptr(glm::inverse(obj.modelMatrix)));

			glUniform1i(20, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, obj.model->texture);
			glBindVertexArray(obj.model->mesh.vao);
			glDrawArrays(GL_TRIANGLES, 0, obj.model->mesh.count);
		}



		// STAGE 2
		// MM: przejscie do kolejnego etapu rysowania - rysowanie do textury kloloru
		glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(quadProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPos);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNorm);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlb);
		glUniform1i(20, 0);
		glUniform1i(21, 1);
		glUniform1i(22, 2);

		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, quad.count);

		// STAGE 3
		// MM: update tekstury koloru
		glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer);
		glUseProgram(blurProgram);

		// Uniforms MM: podpinanie tekstur potrzebnych do nadania efektu
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gCol);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gDepth);

		glUniform1f(2, program->A);
		glUniform1f(3, program->S1);
		glUniform1f(4, program->f);
		glUniform1f(5, program->Far);
		glUniform1f(6, program->maxCoc);
		glUniform1f(7, program->sensorHeight);

		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, quad.count);


		// MM: update tekstury koloru
		glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer2);
		glUseProgram(blur2Program);

		// Uniforms MM: podpinanie tekstur potrzebnych do nadania efektu
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gCol);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gDepth);

		glUniform2fv(2, N, (GLfloat*)offsets3.data());

		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, quad.count);

		// MM: update tekstury koloru
		glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer3);
		glUseProgram(blur2Program);

		// Uniforms MM: podpinanie tekstur potrzebnych do nadania efektu
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gColA);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gDepth);

		glUniform2fv(2, N, (GLfloat*)offsets2.data());

		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, quad.count);

		// MM: update tekstury koloru
		glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer2);
		glUseProgram(blur2Program);

		// Uniforms MM: podpinanie tekstur potrzebnych do nadania efektu
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gCol);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gDepth);

		glUniform2fv(2, N, (GLfloat*)offsets4.data());

		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, quad.count);

		// STAGE 4
		// MM: ostateczne narysowanie tekstury - obraz na ekran;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(blur3Program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gColA);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gColB);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gDepth);

		glUniform2fv(2, N, (GLfloat*)offsets1.data());

		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, quad.count);

		// SWAP
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}
