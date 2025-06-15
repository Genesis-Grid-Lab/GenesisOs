#pragma once
#include "Renderer/RenderCommand.h"
#include <UrbanEngine.h>

using namespace UE;

class OSLayer : public Layer {
 public:
  OSLayer(){}
  virtual void OnAttach() override {
    UE_CORE_INFO("On Attach()");
  }
  virtual void OnUpdate(Timestep ts) override {
    UE_CORE_INFO("On Update()");
    RenderCommand::SetClearColor({1.0f, 0.1f, 0.1f, 1.0f});
    RenderCommand::Clear();
  }
};
