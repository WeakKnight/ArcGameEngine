#pragma once

#include <ArcEngine.h>

#include "BasePanel.h"
#include "EditorContext.h"

namespace ArcEngine
{
	class PropertiesPanel : public BasePanel
	{
	public:
		explicit PropertiesPanel(const char* name = "Properties");

		virtual ~PropertiesPanel() override = default;

		PropertiesPanel(const PropertiesPanel& other) = delete;
		PropertiesPanel(PropertiesPanel&& other) = delete;
		PropertiesPanel& operator=(const PropertiesPanel& other) = delete;
		PropertiesPanel& operator=(PropertiesPanel&& other) = delete;

		virtual void OnImGuiRender() override;

		void SetContext(const EditorContext context)
		{
			if (m_Locked && m_Context.Data)
				return;

			m_Context = context;
		}

		void ForceSetContext(const EditorContext context) { m_Context = context; }

	private:
		void DrawComponents(Entity entity);
		void DrawFileProperties(const char* filepath);

		template<typename Component>
		void DrawAddComponent(Entity entity, const char* name, const char* category = nullptr) const;

	private:
		EditorContext m_Context = {};
		bool m_Locked = false;
		ImGuiTextFilter m_Filter;
		Ref<Scene> m_Scene;
	};
}
