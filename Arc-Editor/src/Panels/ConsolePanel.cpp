#include "ConsolePanel.h"

#include <Arc/ImGui/Modules/ExternalConsoleSink.h>

#include <glm/glm.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
#include <icons/IconsMaterialDesignIcons.h>

#include "../Utils/UI.h"

namespace ArcEngine
{
	ConsolePanel::ConsolePanel(const char* name)
		: BasePanel(name, ICON_MDI_CONSOLE, true)
	{
		ARC_PROFILE_SCOPE();

		s_MessageBufferRenderFilter |= Log::Level::Trace;
		s_MessageBufferRenderFilter |= Log::Level::Info;
		s_MessageBufferRenderFilter |= Log::Level::Debug;
		s_MessageBufferRenderFilter |= Log::Level::Warn;
		s_MessageBufferRenderFilter |= Log::Level::Error;
		s_MessageBufferRenderFilter |= Log::Level::Critical;

		ExternalConsoleSink::SetConsoleSink_HandleFlush([this](const eastl::string& message, Log::Level level){ AddMessage(message, level); });
		m_MessageBuffer = eastl::vector<Ref<ConsolePanel::Message>>(m_Capacity);
	}

	ConsolePanel::~ConsolePanel()
	{
		ExternalConsoleSink::SetConsoleSink_HandleFlush(nullptr);
	}

	void ConsolePanel::AddMessage(const eastl::string& message, Log::Level level)
	{
		ARC_PROFILE_SCOPE();

		static uint32_t id = 0;

		*(m_MessageBuffer.begin() + m_BufferBegin) = CreateRef<Message>(id, message, level);
		if (++m_BufferBegin == m_Capacity)
			m_BufferBegin = 0;
		if (m_BufferSize < m_Capacity)
			m_BufferSize++;

		// Request to scroll to bottom of the list to view the new message
		m_RequestScrollToBottom = m_AllowScrollingToBottom;

		id++;
	}

	const ConsolePanel::Message* ConsolePanel::GetRecentMessage()
	{
		ARC_PROFILE_SCOPE();

		if (m_BufferBegin == 0)
			return nullptr;
		
		return (m_MessageBuffer.begin() + m_BufferBegin - 1)->get();
	}

	void ConsolePanel::Clear()
	{
		ARC_PROFILE_SCOPE();

		for (auto& message : m_MessageBuffer)
			message = nullptr;

		m_BufferBegin = 0;
	}

	void ConsolePanel::OnImGuiRender()
	{
		ARC_PROFILE_SCOPE();

		if (OnBegin())
		{
			ImGuiRenderHeader();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();
			ImGuiRenderMessages();

			OnEnd();
		}
	}

	void ConsolePanel::SetFocus() const
	{
		ARC_PROFILE_SCOPE();

		ImGui::SetWindowFocus(m_ID.c_str());
	}

	void ConsolePanel::ImGuiRenderHeader()
	{
		ARC_PROFILE_SCOPE();

		if (ImGui::Button(ICON_MDI_COGS))
			ImGui::OpenPopup("SettingsPopup");

		if (ImGui::BeginPopup("SettingsPopup"))
		{
			ImGuiRenderSettings();
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		float spacing = ImGui::GetStyle().ItemSpacing.x;
        ImGui::GetStyle().ItemSpacing.x = 2;
        float levelButtonWidth = (ImGui::CalcTextSize(Message::GetLevelIcon(Log::Level(1))) + ImGui::GetStyle().FramePadding * 2.0f).x;
        float levelButtonWidths = (levelButtonWidth + ImGui::GetStyle().ItemSpacing.x) * 7;

		const float cursorPosX = ImGui::GetCursorPosX();
		m_Filter.Draw("###ConsoleFilter", ImGui::GetContentRegionAvail().x - levelButtonWidths);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		for(int i = 0; i < 6; i++)
		{
			ImGui::SameLine();
			auto level = Log::Level(glm::pow(2, i));

			bool levelEnabled = s_MessageBufferRenderFilter & level;
			glm::vec4 c = Message::GetRenderColor(level);
			if(levelEnabled)
				ImGui::PushStyleColor(ImGuiCol_Text, { c.r, c.g, c.b, c.a });
			else
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5, 0.5f, 0.5f));

			if(ImGui::Button(Message::GetLevelIcon(level)))
			{
				s_MessageBufferRenderFilter ^= level;
			}

			if(ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("%s", Message::GetLevelName(level));
			}
			ImGui::PopStyleColor();
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_MDI_NOTIFICATION_CLEAR_ALL))
			Clear();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::GetStyle().ItemSpacing.x = spacing;

		if (!m_Filter.IsActive())
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosX + ImGui::GetFontSize() * 0.5f);
			ImGui::TextUnformatted(ICON_MDI_MAGNIFY " Search...");
		}
	}

	void ConsolePanel::ImGuiRenderSettings()
	{
		ARC_PROFILE_SCOPE();

		UI::BeginProperties(ImGuiTableFlags_SizingStretchSame);
		UI::Property("Scroll to bottom", m_AllowScrollingToBottom);
		UI::Property("DisplayScale", m_DisplayScale, 0.5f, 4.0f, nullptr, 0.1f, "%.1f");
		UI::EndProperties();
	}

	void ConsolePanel::ImGuiRenderMessages()
	{
		ARC_PROFILE_SCOPE();

		constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg
			| ImGuiTableFlags_ContextMenuInBody
			| ImGuiTableFlags_ScrollX
			| ImGuiTableFlags_ScrollY;

		if (ImGui::BeginTable("ScrollRegionTable", 1, tableFlags))
		{
			ImGui::SetWindowFontScale(m_DisplayScale);

			const auto& messageStart = m_MessageBuffer.begin() + m_BufferBegin;
			if (*messageStart) // If contains old message here
			{
				for (const auto* message = messageStart; message != m_MessageBuffer.end(); message++)
				{
					if (!(s_MessageBufferRenderFilter & (*message)->Level))
						continue;

					if (m_Filter.IsActive())
					{
						const auto& m = *message;
						if (m_Filter.PassFilter(m->Buffer.c_str()))
						{
							m->OnImGuiRender();
						}
					}
					else
					{
						(*message)->OnImGuiRender();
					}
				}
			}

			if (m_BufferBegin != 0) // Skipped first messages in vector
			{
				for (const auto* message = m_MessageBuffer.begin(); message != messageStart; message++)
				{
					if (!(s_MessageBufferRenderFilter & (*message)->Level))
						continue;

					if(m_Filter.IsActive())
					{
						if(m_Filter.PassFilter((*message)->Buffer.c_str()))
						{
							(*message)->OnImGuiRender();
						}
					}
					else
					{
						(*message)->OnImGuiRender();
					}
				}
			}

			if (m_RequestScrollToBottom && ImGui::GetScrollMaxY() > 0)
			{
				ImGui::SetScrollY(ImGui::GetScrollMaxY());
				m_RequestScrollToBottom = false;
			}

			ImGui::EndTable();
		}
	}

	ConsolePanel::Message::Message(uint32_t id, const eastl::string& message, Log::Level level)
		: ID(id), Buffer(message), Level(level)
	{
		ARC_PROFILE_SCOPE();

	}

	void ConsolePanel::Message::OnImGuiRender() const
	{
		ARC_PROFILE_SCOPE();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		glm::vec4 c = GetRenderColor(Level);
		ImGui::PushStyleColor(ImGuiCol_Text, { c.r, c.g, c.b, c.a });
		auto levelIcon = GetLevelIcon(Level);
		ImGui::Text("%s  %s", levelIcon, Buffer.c_str());
		ImGui::PopStyleColor();

		ImGui::PushID((int)ID);
		if(ImGui::BeginPopupContextItem("Popup"))
		{
			if(ImGui::MenuItem("Copy"))
				ImGui::SetClipboardText(Buffer.c_str());

			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	const char* ConsolePanel::Message::GetLevelName(Log::Level level)
	{
		switch (level)
		{
			case Log::Level::Trace:			return "Trace";
			case Log::Level::Info:			return "Info";
			case Log::Level::Debug:			return "Debug";
			case Log::Level::Warn:			return "Warning";
			case Log::Level::Error:			return "Error";
			case Log::Level::Critical:		return "Critical";
		}

		return "Unknown name";
	}

	glm::vec4 ConsolePanel::Message::GetRenderColor(Log::Level level)
	{
		switch (level)
		{
			case Log::Level::Trace:			return { 0.75f, 0.75f, 0.75f, 1.00f }; // White-ish gray
			case Log::Level::Info:			return { 0.20f, 0.80f, 0.20f, 1.00f }; // Green
			case Log::Level::Debug:			return { 0.00f, 0.50f, 0.50f, 1.00f }; // Cyan
			case Log::Level::Warn:			return { 0.80f, 0.80f, 0.20f, 1.00f }; // Yellow
			case Log::Level::Error:			return { 0.90f, 0.25f, 0.25f, 1.00f }; // Red
			case Log::Level::Critical:		return { 0.60f, 0.20f, 0.80f, 1.00f }; // Purple
		}

		return { 1.00f, 1.00f, 1.00f, 1.00f };
	}

	const char* ConsolePanel::Message::GetLevelIcon(Log::Level level)
    {
        switch(level)
        {
			case Log::Level::Trace:				return ICON_MDI_MESSAGE_TEXT;
			case Log::Level::Info:				return ICON_MDI_INFORMATION;
			case Log::Level::Debug:				return ICON_MDI_BUG;
			case Log::Level::Warn:				return ICON_MDI_ALERT;
			case Log::Level::Error:				return ICON_MDI_CLOSE_OCTAGON;
			case Log::Level::Critical:			return ICON_MDI_ALERT_OCTAGRAM;
        }

		return "Unknown name";
    }
}
