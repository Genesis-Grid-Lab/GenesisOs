#include "Core/Application.h"
#include <glad/glad.h>
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Core/ResourceManager.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{

  // UE_CORE_ASSERT(!s_Instance, "Application already exists!");

  s_Instance = this;
  m_Window = CreateScope<Window>();
  // m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

  // UE::Renderer::Init();

  // m_ScreenShader = Shader::Create("Data/Shaders/Screen.glsl");

// #if UE_DEBUG
//   m_ImGuiLayer = new ImGuiLayer();
//   PushOverlay(m_ImGuiLayer);
// #endif

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << "Failed to initialize OpenGL context.\n";
  }

}

Application::~Application(){
  // UE_PROFILE_FUNCTION();
  // UE::Renderer::Shutdown();
}

void Application::PushLayer(Layer* layer)
{    
  // UE_PROFILE_FUNCTION();
  m_LayerStack.PushLayer(layer);
  layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
  // UE_PROFILE_FUNCTION();
  m_LayerStack.PushOverlay(layer);
  layer->OnAttach();
}

void Application::PopLayer(Layer* layer) {
  // UE_PROFILE_FUNCTION();
  m_LayerStack.PopLayer(layer);
}

void Application::Close(){
  m_Running = false;
}

// void Application::OnEvent(Event& e){
//   UE_PROFILE_FUNCTION();
//   EventDispatcher dispatcher(e);
//   dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
//   dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

//   for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
//     {
//       if (e.Handled) 
// 	break;
//       (*it)->OnEvent(e);
//     }
// }

void Application::Run(){

  Shader ourShader = ResourceManager::LoadShader("/usr/share/genesisos/Resources/Shaders/shader.vs","/usr/share/genesisos/Resources/Shaders/shader.fs", nullptr, "Test");
  Texture2D ourTex = ResourceManager::LoadTexture("/usr/share/genesisos/Resources/Textures/wall.jpg", false, "TextTex");

  // Triangle vertices
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind and upload
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    SDL_Event event;
    // UE_PROFILE_FUNCTION();
  while(m_Running){
    // UE_PROFILE_SCOPE("RunLopp");
    // float time = (float)glfwGetTime();
    Timestep timestep = 10;
    // m_LastFrameTime = time;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
	m_Running = false;
      }
    }
                
    if(!m_Minimized){
      {
	// UE_PROFILE_SCOPE("LayerStack OnUpdate");
	for(Layer* layer : m_LayerStack)
	  layer->OnUpdate(timestep);
      }
          
    }

    // Clear screen
    glClearColor(1.1f, 1.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ourShader.Use();
    glActiveTexture(GL_TEXTURE0);
    ourTex.Bind();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    m_Window->SwapBuffer();

    while (!m_LayerActionQueue.empty()) {
      LayerAction action = m_LayerActionQueue.front();
      m_LayerActionQueue.pop();
            
      if (action.Type == LayerActionType::Push) {
	PushLayer(action.LayerPtr);
      } else if (action.Type == LayerActionType::Pop) {
	PopLayer(action.LayerPtr);
	delete action.LayerPtr;
      }
    }
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}
