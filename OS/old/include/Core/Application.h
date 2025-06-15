#pragma once

#include "Window.h"
#include "LayerStack.h"

enum class LayerActionType{
  Push,
    Pop
    };
    
struct LayerAction {
  LayerActionType Type;
  Layer* LayerPtr;
};

class Application{
 public:
  Application();
  ~Application();
    
  /* void OnEvent(Event& e); */
    
  void PushLayer(Layer* layer);
  void PushOverlay(Layer* layer);
  void PopLayer(Layer* layer);
  /* ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer;} */

  /* Ref<Shader>& GetScreenShader() { return m_ScreenShader;} */
        
  void Close();
        
  Window& GetWindow() { return *m_Window;}
  static Application& Get() { return *s_Instance;}
   
    
  void QueueLayerAction(LayerActionType type, Layer* layer) {
    m_LayerActionQueue.push({ type, layer });
  }
 private:
  void Run();
  /* bool OnWindowClose(WindowCloseEvent& e); */
  /* bool OnWindowResize(WindowResizeEvent& e); */
 private:
  Scope<Window> m_Window;
  bool m_Running = true;
  bool m_Minimized = false;
  LayerStack m_LayerStack;
  float m_LastFrameTime = 0.0f;
  std::queue<LayerAction> m_LayerActionQueue;
  /* Ref<Shader> m_ScreenShader; */
 private:
  static Application* s_Instance;
  friend int main(int argc, char** argv);
};
