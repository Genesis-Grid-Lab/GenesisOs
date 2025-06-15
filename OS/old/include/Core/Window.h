#pragma once

#include "Config.h"

class Window{
public:
  Window();
  ~Window();

  void SwapBuffer();

  SDL_Window* GetWindow() { return m_Window;}
private:
  SDL_Window* m_Window;
  SDL_GLContext gl_context;
};
