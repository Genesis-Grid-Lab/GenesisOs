#pragma once

#include "Config.h"

class OSGBM{
 public:
  void Init();
  void Render();
  void Cleanup();
 private:
  gbm_device *m_Device = nullptr;
  gbm_surface *m_Surface = nullptr;
};
