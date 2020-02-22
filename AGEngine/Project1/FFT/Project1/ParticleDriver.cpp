// ParticleDriver.cpp
// -- front end for particle system
// cs561 1/20
#include <iostream>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "HelpFunction.h"
#include "Particle.h"
using namespace std;


const glm::mat4 ID(1);
const glm::vec3 EX(1, 0, 0),
EY(0, 1, 0),
EZ(0, 0, 1);


class Client {
public:
	Client(SDL_Window* w);
	~Client(void);
	void draw(double dt);
	void keypress(SDL_Keycode kc);
	void resize(int W, int H);
	void mousedrag(int x, int y, bool left_button);
	void updateCamera();
	void imGuiDraw();
	float dt;
	public:
	SDL_Window* window;
	GLint program;
	GLuint vao,
		vbos[3];
	glm::mat4 VP,
		global_rotation;
	System system;
	bool cpu_load;
	float z;
	float width;
	float height;
	bool HoldShift;

	glm::vec3 global_move = { 0,0,0 };
	glm::mat4 global_move_Matrix;
	glm::vec3 cameraPosition = { 0.0f,0.0f,10.0f };
	glm::vec3 cameraDir = { 0.0f,0.0f,-1.0f };
	glm::vec3 up = { 0,1,0 };
	float lastFrame = 0.0f;
	float yaw = 90, pitch = 90;
	float fov = 60;
	float nearClip = 0.1f;
	float farClip = 1000.0f;
	std::string generateText;
};


/////////////////////////////////////////////////////////////////
const char* vertex_shader_text = R"blah(
  #version 130
  in vec3 position;
  in vec3 normal;
  uniform mat4 VP_matrix;
  uniform mat4 model_matrix;
  uniform mat4 normal_matrix;
  flat out vec3 world_normal;
  void main() {
    gl_Position = VP_matrix * model_matrix * vec4(position,1);
    world_normal = vec3(normal_matrix * vec4(normal,0));
  }
)blah";


const char* fragment_shader_text = R"blah(
  #version 130
  uniform vec3 light_direction;
  uniform vec3 diffuse_color;
  flat in vec3 world_normal;
  out vec4 frag_color;
  void main(void) {
    vec3 m = normalize(world_normal);
    float ml = max(0.0,dot(m,light_direction));
    vec3 color = ml * diffuse_color;
    frag_color = vec4(color,1);
  }
)blah";


Client::Client(SDL_Window* w)
	: window(w), z(10) {
	// shader program
	program = glCreateProgram();
	GLenum type[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	const char* source[2] = { vertex_shader_text, fragment_shader_text };
	GLuint shader[2];
	for (int i = 0; i < 2; ++i) {
		shader[i] = glCreateShader(type[i]);
		glShaderSource(shader[i], 1, source + i, 0);
		glCompileShader(shader[i]);
		GLint value;
		glGetShaderiv(shader[i], GL_COMPILE_STATUS, &value);
		if (!value) {
			char buffer[1024];
			glGetShaderInfoLog(shader[i], 1024, 0, buffer);
			cerr << "shader " << i << " error:" << endl;
			cerr << buffer << endl;
		}
		glAttachShader(program, shader[i]);
	}
	glLinkProgram(program);
	glDeleteShader(shader[0]);
	glDeleteShader(shader[1]);

	// mesh
	glGenVertexArrays(1, &vao);
	glGenBuffers(3, vbos);
	glBindVertexArray(vao);
	// (1) vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glm::vec3 verts[5] = { {1,1,1}, {-1,1,1}, {-1,-1,1}, {1,-1,1}, {0,0,-1} };
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	GLint loc = glGetAttribLocation(program, "position");
	glVertexAttribPointer(loc, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(loc);
	// (2) normals
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glm::vec3 norms[5] = { {0,0,1}, {0,2,-1}, {-2,0,-1}, {0,-2,-1}, {2,0,-1} };
	for (int i = 0; i < 5; ++i)
		norms[i] = glm::normalize(norms[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(norms), norms, GL_STATIC_DRAW);
	loc = glGetAttribLocation(program, "normal");
	glVertexAttribPointer(loc, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(loc);
	// (3) faces
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);
	unsigned faces[6 * 3] = { 1,2,0, 2,3,0, 0,4,1, 1,4,2, 2,4,3, 0,3,4 };
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);
	glBindVertexArray(0);

	// misc
	glEnable(GL_DEPTH_TEST);
	resize(500, 500);
	glUseProgram(program);
	loc = glGetUniformLocation(program, "light_direction");
	glUniform3f(loc, 0, 0, 1);
	global_rotation = glm::mat4(1);
	system.initialize();
	cpu_load = false;
}


/////////////////////////////////////////////////////////////////
Client::~Client(void) {
	glUseProgram(0);
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(3, vbos);
}


/////////////////////////////////////////////////////////////////
void Client::draw(double dt) {
	glClearColor(1, 1, 1, 1);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	GLint umodel_matrix = glGetUniformLocation(program, "model_matrix"),
		unormal_matrix = glGetUniformLocation(program, "normal_matrix");
	glUniform3f(glGetUniformLocation(program, "diffuse_color"), 1, 0, 1);

	glBindVertexArray(vao);
	for (Particle& p : system.particles) {
		if (p.time_remaining >= 0) {
			// orient mesh in direction of particle velocity,
			//   and parallel to plane of motion
			float lenOfV = glm::length2(p.velocity);
			glm::vec3 w = lenOfV < 0.00001f ? glm::vec3(0, 0, 1) : -glm::normalize(p.velocity);
			float dot = abs( glm::dot(w, p.motion_normal));
			glm::vec3 u = dot > 0.9995f?glm::vec3(1,0,0) :   glm::normalize(glm::cross(w, p.motion_normal));
				glm::vec3	v = glm::cross(u, w);
			glm::mat4 M = global_move_Matrix * global_rotation
				* glm::translate(ID, p.position)
				* glm::mat4(glm::mat3(u, v, w))
				* glm::scale(ID, 0.25f * p.size * glm::vec3(1, 1, 2)),
				N = glm::mat4(glm::mat3(M));
			glUniformMatrix4fv(umodel_matrix, 1, false, &M[0][0]);
			glUniformMatrix4fv(unormal_matrix, 1, false, &N[0][0]);
			glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_INT, 0);
		}
	}
	glBindVertexArray(0);

	if (cpu_load)
		this_thread::sleep_for(chrono::milliseconds(100));

	system.update(dt,generateText);
	system.DrawImgui();
}


void Client::keypress(SDL_Keycode kc) {
	if (kc == SDLK_SPACE) {
		cpu_load = !cpu_load;
		SDL_SetWindowTitle(window, cpu_load ? "CPU load on"
			: "CPU load off");
	}

	//float cameraSpeed = 5.0f * dt;
	//if (kc == SDLK_w) {
	   // std::cout << "w" << std::endl;
	   // cameraPosition += cameraSpeed * cameraDir;
	//}
	//if (kc == SDLK_s)
	   // cameraPosition -= cameraSpeed * cameraDir;
	//if (kc == SDLK_a)
	   // cameraPosition -= glm::normalize(glm::cross(cameraDir, up)) * cameraSpeed;
	//if (kc == SDLK_d)
	   // cameraPosition += glm::normalize(glm::cross(cameraDir, up)) * cameraSpeed;
	//if (kc == SDLK_p) {
	   // Print(cameraPosition, "camera position");
	   // Print(cameraDir, "camera direction");
	   // std::cout << " pitch " << pitch << " yaw " << yaw << std::endl;
	//}
	//if (kc == SDLK_q)
	   // yaw -= 1;
	//if (kc == SDLK_e)
	   // yaw += 1;

	float cameraSpeed = 5.0f * dt;
	if (kc == SDLK_w) {
		//std::cout << "w" << std::endl;
		z -= 1;
	}
	if (kc == SDLK_s)
		z += 1;
	if (kc == SDLK_LCTRL) {
		//std::cout << "w" << std::endl;
		HoldShift = !HoldShift;
	}
}


void Client::resize(int W, int H) {
	width = W;
	height = H;
	glViewport(0, 0, W, H);
	VP = glm::infinitePerspective(glm::radians(90.0f), float(W) / float(H), 0.01f)
		* glm::lookAt(glm::vec3(0, 0, z), glm::vec3(0, 0, 0), glm::vec3(0, 10, 0));
	glUseProgram(program);
	GLint loc = glGetUniformLocation(program, "VP_matrix");
	glUniformMatrix4fv(loc, 1, false, &VP[0][0]);
}


void Client::mousedrag(int x, int y, bool left_button) {
	if (!HoldShift)
	{
		return;
	}
	glm::vec3 axis = glm::mat3(global_rotation) * EZ;
	global_rotation = glm::rotate(ID, glm::radians(float(y)), EX)
		* glm::rotate(ID, glm::radians(float(x)), axis)
		* global_rotation;


	/*float xoffset = x;
	float yoffset =y;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;*/

}

void Client::updateCamera()
{
	/*if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraDir = glm::normalize(front);*/

	VP = glm::infinitePerspective(glm::radians(90.0f), width / height, 0.01f)
		* glm::lookAt(glm::vec3(0, 0, z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUseProgram(program);
	GLint loc = glGetUniformLocation(program, "VP_matrix");
	glUniformMatrix4fv(loc, 1, false, &VP[0][0]);


	global_move_Matrix = glm::translate(ID, global_move);

}

void Client::imGuiDraw()
{
	ImGui::SetNextWindowSize({ 300,400 });
	ImGui::Begin("Window");
	ImGui::SliderFloat3("Global move", &global_move[0], -10.0f, 5.0f);
	ImGui::TextUnformatted(generateText.c_str());
	ImGui::TextUnformatted("Use Ctrl to open or close camera rotation, or use check box");
	ImGui::Checkbox("Camera rotation", &HoldShift);
	if (ImGui::Button("Get Sample Data"))
	{
		system.UseSampleData();
		global_move = glm::vec3(0, -6.8f, 0);
	}
	ImGui::End();
	ImGui::Render();
}






/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {

	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	// SDL: initialize and create a window
	SDL_Init(SDL_INIT_VIDEO);
	const char* title = "CS 561 Particle Demo";
	int width = 500,
		height = 500;
	SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// GLEW: get function bindings (if possible)
	GLenum value = glewInit();
	if (value != GLEW_OK) {
		cout << glewGetErrorString(value) << endl;
		SDL_GL_DeleteContext(context);
		SDL_Quit();
		return -1;
	}


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	// animation loop
	bool done = false;
	Client* client = new Client(window);
	Uint32 ticks_last = SDL_GetTicks();
	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				done = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					done = true;
				else
					client->keypress(event.key.keysym.sym);
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
					client->resize(event.window.data1, event.window.data2);
				break;
			case SDL_MOUSEMOTION:
				if ((event.motion.state & SDL_BUTTON_LMASK) != 0
					|| (event.motion.state & SDL_BUTTON_RMASK) != 0)
					client->mousedrag(event.motion.xrel, event.motion.yrel,
					(event.motion.state & SDL_BUTTON_LMASK) != 0);
				break;
			}
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}
		client->generateText = "";


		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		client->updateCamera();
		Uint32 ticks = SDL_GetTicks();
		double dt = 0.001 * (ticks - ticks_last);
		client->dt = dt;
		ticks_last = ticks;
		client->draw(dt);
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		client->imGuiDraw();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);
	}

	// clean up
	delete client;
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}



