#include "StatsPanel.h"

#include <icons/IconsMaterialDesignIcons.h>
#include <imgui/imgui.h>

#include "../Utils/UI.h"

namespace ArcEngine
{
	StatsPanel::StatsPanel(const char* name)
		: BasePanel(name, ICON_MDI_INFORMATION_VARIANT, true)
	{
	}

	void StatsPanel::OnImGuiRender()
	{
		ARC_PROFILE_SCOPE();
		
		float avg = 0.0f;

		const size_t size = m_FrameTimes.size();
		if (size >= 50)
			m_FrameTimes.erase(m_FrameTimes.begin());

		m_FrameTimes.emplace_back(ImGui::GetIO().Framerate);
		for (uint32_t i = 0; i < size; i++)
		{
			m_FpsValues[i] = m_FrameTimes[i];
			avg += m_FrameTimes[i];
		}
		
		avg /= (float)size;

		if (OnBegin())
		{
			{
				const auto stats = Renderer2D::GetStats();
				ImGui::Text("2D");

				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::Text("Quads: %d", stats.QuadCount);
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::Text("Tris: %d", stats.GetTotalTriangleCount());

				ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
			}

			ImGui::Separator();

			{
				const auto stats = Renderer3D::GetStats();
				ImGui::Text("3D");

				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::SameLine();
				ImGui::PushItemWidth(-1);
				ImGui::Text("Indices: %d", stats.IndexCount);
			}

			UI::BeginProperties();
			bool vSync = Application::Get().GetWindow().IsVSync();
			if (UI::Property("VSync Enabled", vSync))
				Application::Get().GetWindow().SetVSync(vSync);
			UI::EndProperties();

			ImGui::PlotLines("##FPS", m_FpsValues, (int)size);
			ImGui::Text("FPS: %f", avg);
			const float fps = (1.0f / avg) * 1000.0f;
			ImGui::Text("Frame time (ms): %f", fps);

			OnEnd();
		}
	}
}
