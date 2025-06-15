#pragma once

#include <UrbanEngine.h>
#include "OSLayer.h"

using namespace UE;
class OSApp : public Application {
public:
  OSApp(ApplicationCommandLineArgs args)
    :Application("Genesis OS", {1800, 1000},args){
    PushLayer(new OSLayer());
  }

  ~OSApp(){}
};
