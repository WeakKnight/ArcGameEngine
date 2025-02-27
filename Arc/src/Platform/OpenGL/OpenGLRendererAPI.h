#pragma once

#include "Arc/Renderer/RendererAPI.h"

namespace ArcEngine
{
	class VertexArray;

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void Draw(const Ref<VertexArray>& vertexArray, uint32_t count) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		virtual void EnableCulling() override;
		virtual void DisableCulling() override;
		virtual void FrontCull() override;
		virtual void BackCull() override;
		virtual void SetDepthMask(bool value) override;
		virtual void SetDepthTest(bool value) override;
		virtual void SetBlendState(bool value) override;
	};
}
