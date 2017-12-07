#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "framework/Application.h"
#include "framework/util/Logger.h"
#include "framework/input/InputHandler.h"
#include "framework/graphics/Shader.h"

#include "framework/io/ResourceManager.h"
#include "framework/graphics/Mesh.h"
#include "framework/graphics/Camera.h"
#include "framework/rendering/DeferredRenderer.h"
#include "framework/rendering/Renderable.h"
#include "framework/physics/GJK.h"
#include "framework/util/LineDrawer.h"
#include "framework/physics/Octree.h"
using namespace Framework;

glm::ivec2 initialSize = { 800, 600 };

Framework::Application *app;
float xPos = 0;
float yPos = 0;

double t = 0.0;
const double dt = 1.0 / 60.0;
double currentTime = glfwGetTime();
double accumulator = 0.0;

Camera cam = Camera({ -5.f, 0, 0 }, { 0, 1, 0 }, 0, 0);
Mesh *m;
Mesh *sphereMesh;
Renderable *cube1;
Renderable *cube2;
Renderable *sphere;
glm::mat4 projection;
DeferredRenderer *renderer;

Octree octree(1024);

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	app->get_context().viewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(45.f), (float)width / (float)height, 0.1f, 1000.f);
	renderer->resize({ width, height });
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	InputHandler::updateKeys(key, action);
}

void update()
{
	octree.clear();
	octree.insert(cube1->rigidbody());
	octree.insert(sphere->rigidbody());
	
	glm::vec3 camPos = cam.get_position();
	float speed = 1.f;
	if (InputHandler::checkButton("Up", ButtonState::HELD))
	{
		//camPos += cam.front() * speed;
		cube1->transform().translate({ 0, 0.1f, 0 });
	}
	else if (InputHandler::checkButton("Down", ButtonState::HELD))
	{
		//camPos -= cam.front() * speed;
		cube1->transform().translate({ 0, -0.1f, 0 });
	}

	if (InputHandler::checkButton("Left", ButtonState::HELD))
	{
		//camPos -= cam.right() * speed;
		cube1->transform().translate({ 0, 0, -0.1f });
	}
	else if (InputHandler::checkButton("Right", ButtonState::HELD))
	{
		//camPos += cam.right() * speed;
		cube1->transform().translate({ 0, 0, 0.1f });
	}
	cam.set_position(camPos);

	float yaw = cam.yaw();
	if (InputHandler::checkButton("RotLeft", ButtonState::HELD))
	{
		yaw -= speed;
	}
	else if (InputHandler::checkButton("RotRight", ButtonState::HELD))
	{
		yaw += speed;
	}
	cam.set_rotation(yaw, cam.pitch());

	auto neighb = octree.neighbours(cube1->rigidbody());
	for (const auto& n : neighb)
	{
		if (n == cube1->rigidbody()) continue;
		
		Collision c;
		if (GJK::intersect(*cube1->rigidbody(), *n, &c))
		{
			cube1->transform().translate(c.normal * c.depth);
			std::cout << "coll" << std::endl;
		}
	}
	
}

int main()
{
	app = new Framework::Application("Graphics 2 project",
		3, 2, initialSize.x, initialSize.y);

	projection = glm::perspective(glm::radians(46.f), (float)initialSize.x / (float)initialSize.y, 0.1f, 1000.f);;

	glfwSetFramebufferSizeCallback(app->get_window(), framebuffer_size_callback);
	glfwSetWindowSizeCallback(app->get_window(), framebuffer_size_callback);
	glfwSetKeyCallback(app->get_window(), key_callback);

	InputHandler::addInput("Up", GLFW_KEY_UP);
	InputHandler::addInput("Down", GLFW_KEY_DOWN);
	InputHandler::addInput("Left", GLFW_KEY_LEFT);
	InputHandler::addInput("Right", GLFW_KEY_RIGHT);
	InputHandler::addInput("RotLeft", GLFW_KEY_A);
	InputHandler::addInput("RotRight", GLFW_KEY_D);

	renderer = new DeferredRenderer(800, 600, &app->get_context(), nullptr, nullptr);
	renderer->init();
	renderer->setCamera(&cam);
	renderer->setProjection(projection);

	LineDrawer::init();

	PointLight light;
	light.position = { -3, 0, 0 };
	light.diffuse = Color::RED;
	light.intensity = 4;
	light.linearAttenuation = 0.01;
	renderer->pointLights().push_back(light);

	PointLight light2;
	light2.position = { -3, 0, 2.f };
	light2.diffuse = Color::GREEN;
	light2.intensity = 1;
	light2.linearAttenuation = 0.003;
	renderer->pointLights().push_back(light2);

	PointLight light3;
	light3.position = { -3, 0, -2.f };
	light3.diffuse = Color::BLUE;
	light3.intensity = 1;
	light3.linearAttenuation = 0.001;
	renderer->pointLights().push_back(light3);

	DirectionalLight dirLight;
	dirLight.diffuse = Color::WHITE;
	dirLight.ambient = {0.4f, 0.4f, 0.4f, 1}; // grey
	dirLight.direction = { 180, 0, 0 };
	dirLight.intensity = 2;
	renderer->setDirLight(dirLight);

	Mesh linePoints = Mesh({ 
		Vertex({ 0, 0, 0 }, glm::vec3(0), glm::vec2(0), glm::vec4(1)),
		Vertex({ 1, 1, 1 }, glm::vec3(0), glm::vec2(0), glm::vec4(1)), }, 
	{ 0, 1 });

	Shader *shader = ResourceManager::Load<Shader>("assets/shaders/basicDeferred");

	m = ResourceManager::Load<Mesh>("assets/models/cube.obj");
	sphereMesh = ResourceManager::Load<Mesh>("assets/models/sphere.obj");
	Texture2D *tex = ResourceManager::Load<Texture2D>("assets/textures/storm_covenant_carbine_diffcoloured.png");
	cube1 = new Renderable(m, shader, tex, true);
	cube2 = new Renderable(m, shader, tex);
	sphere = new Renderable(sphereMesh, shader, tex, true);
	cube1->transform().translate({ 0, 0, 2 });
	//cube2->transform().translate({ 0, 0, -2 });
	sphere->transform().translate({ 0, 0, -2 });

	octree.insert(cube1->rigidbody());
	octree.insert(sphere->rigidbody());

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(app->get_window()))
	{
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		// only update at specified delta-time
		while (accumulator >= dt)
		{
			InputHandler::handleInput();
			update();
			accumulator -= dt;
			t += dt;
		}

		glClearColor(0.f, 0.f, 0.f, 1.f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//cube1->transform().rotate(glm::angleAxis((float)dt, glm::vec3(0, 1, 0)));
		
		//cube2->transform().rotate(glm::angleAxis((float)dt, glm::vec3(1, 0, 0)));

		glm::mat4 view = cam.view();

		renderer->beginFrame();
		cube1->draw(view, projection);
		sphere->draw(view, projection);
		//cube2->draw(view, projection);

		renderer->endFrame();

		LineDrawer::drawLine(linePoints, Color::GREEN, projection, view);
		
		glfwSwapBuffers(app->get_window());
		glfwPollEvents();

		//std::cout << "FPS: " << (1.0 / frameTime) << " dt:" << frameTime << std::endl;
	}
	
	delete renderer;
	delete app;
	return 0;
}