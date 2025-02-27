#pragma once

#include "Framebuffer.h"
#include "Arc/Debug/Profiler.h"

namespace ArcEngine
{
	struct CameraData
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 ViewProjection;
		glm::vec3 Position;
	};

	struct RenderGraphData
	{
		Ref<Framebuffer> CompositePassTarget;
		Ref<Framebuffer> RenderPassTarget;
		Ref<Framebuffer> LightingPassTarget;
		Ref<Framebuffer> FXAAPassTarget;

		static const size_t MaxBlurSamples = 6;
		size_t BlurSamples = MaxBlurSamples;
		Ref<Framebuffer> PrefilteredFramebuffer;
		Ref<Framebuffer> TempBlurFramebuffers[MaxBlurSamples];
		Ref<Framebuffer> DownsampledFramebuffers[MaxBlurSamples];
		Ref<Framebuffer> UpsampledFramebuffers[MaxBlurSamples];

		RenderGraphData(uint32_t width, uint32_t height)
		{
			ARC_PROFILE_SCOPE();

			if (width <= 0 || height <= 0)
			{
				ARC_CORE_ERROR("Cannot create a RenderGraph with {}x{} dimensions, making it 1x1", width, height);
				width = 1;
				height = 1;
			}
			
			{
				FramebufferSpecification spec;
				spec.Attachments = { FramebufferTextureFormat::R11G11B10 };
				spec.Width = width;
				spec.Height = height;
				CompositePassTarget = Framebuffer::Create(spec);
			}

			{
				FramebufferSpecification spec;
				spec.Attachments = {
					FramebufferTextureFormat::RGBA8,				// Albedo
					FramebufferTextureFormat::RG16F,				// Normal
					FramebufferTextureFormat::RGBA8,				// Metallic, Roughness, AO
					FramebufferTextureFormat::RGBA8,				// rgb: EmissionColor, a: intensity
					FramebufferTextureFormat::Depth
				};
				spec.Width = width;
				spec.Height = height;
				RenderPassTarget = Framebuffer::Create(spec);
			}

			{
				FramebufferSpecification spec;
				spec.Attachments = { FramebufferTextureFormat::R11G11B10 };
				spec.Width = width;
				spec.Height = height;
				LightingPassTarget = Framebuffer::Create(spec);
				FXAAPassTarget = Framebuffer::Create(spec);
			}

			width /= 2;
			height /= 2;
			FramebufferSpecification bloomSpec;
			bloomSpec.Attachments = { FramebufferTextureFormat::R11G11B10 };
			bloomSpec.Width = width;
			bloomSpec.Height = height;
			PrefilteredFramebuffer = Framebuffer::Create(bloomSpec);

			BlurSamples = 0;
			for (size_t i = 0; i < MaxBlurSamples; i++)
			{
				++BlurSamples;

				width /= 2;
				height /= 2;

				if (width <= 0 || height <= 0)
					break;

				FramebufferSpecification blurSpec;
				blurSpec.Attachments = { FramebufferTextureFormat::R11G11B10 };
				blurSpec.Width = width;
				blurSpec.Height = height;
				TempBlurFramebuffers[i] = Framebuffer::Create(bloomSpec);
				DownsampledFramebuffers[i] = Framebuffer::Create(blurSpec);
				UpsampledFramebuffers[i] = Framebuffer::Create(blurSpec);
			}
		}

		void Resize(uint32_t width, uint32_t height)
		{
			ARC_PROFILE_SCOPE();

			CompositePassTarget->Resize(width, height);
			RenderPassTarget->Resize(width, height);
			LightingPassTarget->Resize(width, height);
			FXAAPassTarget->Resize(width, height);

			width /= 2;
			height /= 2;
			PrefilteredFramebuffer->Resize(width, height);

			BlurSamples = 0;
			for (size_t i = 0; i < MaxBlurSamples; i++)
			{
				++BlurSamples;

				width /= 2;
				height /= 2;

				if (width <= 0 || height <= 0)
					break;

				TempBlurFramebuffers[i]->Resize(width, height);
				DownsampledFramebuffers[i]->Resize(width, height);
				UpsampledFramebuffers[i]->Resize(width, height);
			}
		}
	};
}
