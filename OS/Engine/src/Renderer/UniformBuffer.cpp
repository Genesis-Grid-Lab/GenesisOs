#include "uepch.h"
#include "Renderer/UniformBuffer.h"
#include "Core/UE_Assert.h"
#include "Renderer/Renderer.h"
#include "Platform/OpenGl/OpenGLUniformBuffer.h"

namespace UE {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    UE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		UE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
