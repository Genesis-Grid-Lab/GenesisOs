#include "Core/Window.h"

Window::Window(){

  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    std::cerr << "SDL failed to init: " << SDL_GetError() << "\n";
  }

  SDL_DisplayMode mode;
  SDL_GetCurrentDisplayMode(0, &mode);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  m_Window = SDL_CreateWindow(
			      "GenesisOS",
			      0, 0, mode.w, mode.h,
			      SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL
			      );
  if(!m_Window){
    std::cerr << "Failed to create SDL window: " << SDL_GetError() << "\n";
    SDL_Quit();
  }

  gl_context = SDL_GL_CreateContext(m_Window);
  
  if (!gl_context) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
  }

  SDL_ShowCursor(SDL_DISABLE);

  SDL_GL_SetSwapInterval(1);
}


Window::~Window(){
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(m_Window);
  SDL_Quit();
}

void Window::SwapBuffer(){
  SDL_GL_SwapWindow(m_Window);
}
