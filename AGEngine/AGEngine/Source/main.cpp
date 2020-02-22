#include <PCH.h>
#include <Graphics.h>
#include <RenderPass.h>
#include <Pipeline.h>
#include <Framebuffer.h>
#include <CommandBuffer.h>
#include <Mesh.h>
#include <Descriptor.h>
#include <DescriptorSet.h>
#include <Window.h>
#include <FrameTime.h>
static POINT mouseLastPos;
static POINT mouseCurrentPos;

glm::vec3 cameraPosition = { 0.0f,0.0f,-10.0f };
glm::vec3 cameraDir = { 0.0f,0.0f,1.0f };
glm::vec3 up = { 0,1,0 };
double m_ouseSpeed = 0.1f;
float lastFrame = 0.0f;
float yaw = 0, pitch = 0;
float lastX = 400, lastY = 400;
bool firstMouse = true;
float fov = 60;
float nearClip = 0.1f;
float farClip = 1000.0f;
Buffer cameraBuffer;

void scroll_callback(double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 110.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 110.0f)
		fov = 110.0f;
}

static void MouseCallback (glm::vec2 delta) {
	//std::cout << "delta " << delta.x << " " << delta.y << std::endl;
	float xoffset = delta.x;
	float yoffset = delta.y; // 注意这里是相反的，因为y坐标的范围是从下往上的

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraDir = glm::normalize(front);
}

static void KeyCallback() {
	Window* window = Window::GetInstance();
	float cameraSpeed = 5.0f * FrameTime::GetInstance()->DeltaTime();
	if (window->IsPressed(VK_W)) {
		std::cout << "w" << std::endl;
		cameraPosition += cameraSpeed * cameraDir;
	}		
	if (window->IsPressed(VK_S))
		cameraPosition -= cameraSpeed * cameraDir;
	if (window->IsPressed(VK_A))
		cameraPosition -= glm::normalize(glm::cross(cameraDir, up)) * cameraSpeed;
	if (window->IsPressed(VK_D))
		cameraPosition += glm::normalize(glm::cross(cameraDir, up)) * cameraSpeed;
	if (window->IsTriggered(VK_P))
		Print(cameraPosition,"camera position");
}
void UpdateCameraData() {
	Graphics* g_Graphics = Graphics::GetInstance();
	Camera cameraData;
	cameraData.viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraDir, up);
	cameraData.inverseViewMatrix = glm::inverse(cameraData.viewMatrix);
	cameraData.projectionMatrix = glm::perspective(glm::radians(fov), g_Graphics->swapChainExtent.width / (float)g_Graphics->swapChainExtent.height, nearClip, farClip);
	cameraData.projectionMatrix[1][1] *= -1;
	cameraData.inverseProjectionMatrix = glm::inverse(cameraData.projectionMatrix);
	cameraData.position = cameraPosition;
	g_Graphics->CopyDataToBuffer(&cameraData, cameraBuffer, sizeof(Camera), 0);

}


void main() {
	Window* window = Window::GetInstance();
	Graphics* g_Graphics = Graphics::GetInstance();
	
	FrameTime* framControl = FrameTime::GetInstance();


	//window->mouseUpdateCallbacks.push_back(MouseCallback);
	window->keyUpdateCallbacks.push_back(KeyCallback);
	g_Graphics->bufferUpdateCallbacks.push_back(UpdateCameraData);

	g_Graphics->width = 720;
	g_Graphics->height = 720;
	g_Graphics->applicationName = "AGEngine";
	g_Graphics->Init();


	//
	std::vector<glm::vec2> vertexes{ {1,0},{0,1},{0,0} };
	uint32_t index[3] = { 0,1,2 };
	Buffer vertexBuffer = g_Graphics->CreateBuffer(sizeof(vertexes), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	Buffer indexBuffer = g_Graphics->CreateBuffer(sizeof(index), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
	g_Graphics->CopyDataToGUPBuffer(vertexes.data(), vertexBuffer, sizeof(vertexes));
	g_Graphics->CopyDataToGUPBuffer(index, indexBuffer, sizeof(index));
	//



	//prepare resource
	Mesh plane;
	plane.LoadMesh("../Resource/Model/Cube.obj");
	g_Graphics->CreateVertexBuffer(plane);
	g_Graphics->CreateIndexBuffer(plane);

	GraphicsImage renderTarget = g_Graphics->CreateImage(g_Graphics->width, g_Graphics->height, g_Graphics->swapChainImageFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
	g_Graphics->CreateImageView(renderTarget, g_Graphics->swapChainImageFormat);
	g_Graphics->TransitionImageLayout(renderTarget, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	Camera cameraData;
	cameraBuffer = g_Graphics->CreateBuffer(sizeof(Camera), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	cameraData.viewMatrix = glm::lookAt(cameraPosition,cameraPosition + cameraDir,up);
	cameraData.inverseViewMatrix = glm::inverse(cameraData.viewMatrix);
	cameraData.projectionMatrix = glm::perspective(glm::radians(fov), g_Graphics->swapChainExtent.width / (float)g_Graphics->swapChainExtent.height, nearClip, farClip);
	cameraData.projectionMatrix[1][1] *= -1;
	cameraData.inverseProjectionMatrix = glm::inverse(cameraData.projectionMatrix);
	g_Graphics->CopyDataToBuffer(&cameraData, cameraBuffer, sizeof(Camera), 0);


	//create render pass
	RenderPass defaultpass(1,g_Graphics->swapChainExtent);
	defaultpass.CreateAttachmentDescriptionForColorAttachment(0,g_Graphics->swapChainImageFormat, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
	VkAttachmentReference reference = RenderPass::CreateAttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	defaultpass.CreateSubpassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, { reference },false, VkAttachmentReference{});
	defaultpass.SetClearColor({ {0,0,0,0} });
	defaultpass.CreateRenderPass();


	//create pipeline
	Pipeline defaultPipeline(g_Graphics->swapChainExtent,0);
	defaultPipeline.LoadShader({ "../Resource/shader/default.vert","../Resource/shader/default.frag" });
	VkPipelineColorBlendAttachmentState blendstate = Pipeline::GetColorBlendAttachmentState(false);
	defaultPipeline.SetColorBlendings({ blendstate });
	defaultPipeline.CreatePipeline(defaultpass);

	DescriptorSet descriptorSet;
	descriptorSet.CreateDescriptorSet(defaultPipeline.descriptorSetLayout);
	Descriptor camerabufferDescriptor(cameraBuffer,0);
	descriptorSet.UpdateDescriptorSet({ camerabufferDescriptor });

	//framebuffer
	Framebuffer framebuffer;
	framebuffer.CreateFramebuffer({ renderTarget }, defaultpass, g_Graphics->swapChainExtent.width, g_Graphics->swapChainExtent.height);

	CommandBuffer renderCommand = g_Graphics->CreateCommandBuffer(false,true);
	renderCommand.BeginRenderpass(defaultpass, framebuffer);
	renderCommand.BeginPipeline(defaultPipeline, defaultpass, descriptorSet);
	renderCommand.Draw(vertexBuffer,indexBuffer, 3, 1);
	//renderCommand.Draw(plane, k_Position, 1);
	MSG msg;
	while (!Graphics::GetInstance()->close) {
		framControl->FrameStart();
		window->Update();
		window->UpdateMousePos(g_Graphics->winWindowHandle);
		if (PeekMessage(&msg, g_Graphics->winWindowHandle, 0, 0, PM_REMOVE))
		{
			if (msg.message != WM_IME_SELECT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				
			}
			else
			{
				std::cout << "change" << std::endl;
			}	
		}
		window->CallUpdateCallbacks();
		g_Graphics->DrawFrameAndPresent({ &renderCommand }, renderTarget);
		framControl->FrameEnd();
	}
	
}