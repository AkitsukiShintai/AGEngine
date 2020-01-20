#include <PCH.h>
#include <Graphics.h>
#include <RenderPass.h>
#include <Pipeline.h>
#include <Framebuffer.h>
#include <CommandBuffer.h>
#include <Mesh.h>
#include <Descriptor.h>
#include <DescriptorSet.h>

static POINT mouseLastPos;
static POINT mouseCurrentPos;

glm::vec3 cameraPosition = { 0.0f,2.0f,10.0f };
glm::vec3 cameraDir = { 0.0f,0.0f,-1.0f };
glm::vec3 up = { 0,1,0 };
double m_ouseSpeed = 0.1f;
float deltaTime = 0.0f; 
float lastFrame = 0.0f;
float yaw = 0, pitch = 0;
float lastX = 400, lastY = 400;
bool firstMouse = true;
float fov = 60;
float nearClip = 0.1f;
float farClip = 1000.0f;

void main() {
	Graphics* g_Graphics = Graphics::GetInstance();

	g_Graphics->width = 720;
	g_Graphics->height = 720;
	g_Graphics->applicationName = "AGEngine";
	g_Graphics->Init();

	//prepare resource
	Mesh plane;
	plane.LoadMesh("../Resource/Model/plane.obj");
	g_Graphics->CreateVertexBuffer(plane);
	g_Graphics->CreateIndexBuffer(plane);

	GraphicsImage renderTarget = g_Graphics->CreateImage(g_Graphics->width, g_Graphics->height, g_Graphics->swapChainImageFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
	g_Graphics->CreateImageView(renderTarget, g_Graphics->swapChainImageFormat);
	g_Graphics->TransitionImageLayout(renderTarget, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	Camera cameraData;
	Buffer cameraBuffer = g_Graphics->CreateBuffer(sizeof(Camera), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	cameraData.viewMatrix = glm::lookAt(cameraPosition,cameraPosition + cameraDir,up);
	cameraData.inverseViewMatrix = glm::inverse(cameraData.viewMatrix);
	cameraData.projectionMatrix = glm::perspective(glm::radians(fov), g_Graphics->swapChainExtent.width / (float)g_Graphics->swapChainExtent.height, nearClip, farClip);
	cameraData.projectionMatrix[1][1] *= -1;
	cameraData.inverseProjectionMatrix = glm::inverse(cameraData.projectionMatrix);
	g_Graphics->CopyDataToBuffer(&cameraData, cameraBuffer, sizeof(Camera), 0);


	//create render pass
	RenderPass defaultpass(1);
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
	renderCommand.Draw(plane, k_Position, 1);
	MSG msg;
	while (!Graphics::GetInstance()->close) {
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
		mouseLastPos = mouseCurrentPos;
		GetCursorPos(&mouseCurrentPos);
		std::cout << "mouse " << mouseCurrentPos.x << std::endl;
		g_Graphics->DrawFrameAndPresent({ &renderCommand }, renderTarget);
	}
	
}