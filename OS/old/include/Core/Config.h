#pragma once

#include <SDL2/SDL.h>
#include <SDL_opengles2.h>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <array>
#include <fstream>
#include <cstdint>
#include <queue>
#include <map>
#include <list>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

//--------------------- Scope = unique pointer --------------------
template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args){

  return std::make_unique<T>(std::forward<Args>(args)...);
}

//--------------------- Ref = shared pointer -----------------------
template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args){

  return std::make_shared<T>(std::forward<Args>(args)...);
}
