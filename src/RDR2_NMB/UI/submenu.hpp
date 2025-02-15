// Licensed under the MIT License.

#pragma once
#include "menu.hpp"
#include "submenu_ids.hpp"

namespace Menu
{
	class Submenu
	{
	private:
		std::function<void(Submenu*)> m_StructureFunc; // The function that sets the submenu layout

	public:
		std::vector<Option> m_Options;

		std::string m_Header = "";
		std::string m_SubHeader = "";
		int m_ID = Submenu_Invalid;
		int m_NumVisibleOptions = 8; // 8 = DEFAULT
		int m_NumOptions = 0;


		Submenu() = default;
		Submenu(const std::string& header, const std::string& subHeader, int id, int numVisibleOptions, std::function<void(Submenu*)> &submenuFunc)
		{
			m_Header = header;
			m_SubHeader = subHeader;
			m_ID = id;
			m_NumVisibleOptions = numVisibleOptions;
			m_StructureFunc = submenuFunc;
			std::invoke(m_StructureFunc, static_cast<Submenu*>(this)); // Create the submenu
		}


		/// Regular plain ol' option.
		/// Passed function will execute when the option is pressed.
		void AddRegularOption(const std::string& text, const std::string& footer, std::function<void()> func = [] {})
		{
			Option option(true, false, false, false, false);
			option.SetText(text);
			option.SetFooter(footer);
			option.SetFunction(func);
			option.m_Index = (int)m_Options.size();
			m_Options.push_back(option);
			m_NumOptions++;
		}


		/// An on/off (true/false) boolean toggle option.
		/// Note: bToggle parameter must be an already defined boolean variable.
		/// Passed function will execute when the option is pressed.
		void AddBoolOption(const std::string& text, const std::string& footer, bool* bToggle, std::function<void()> func = [] {})
		{
			Option option(false, true, false, false, false);
			option.SetText(text);
			option.SetFooter(footer);
			option.SetFunction(func);
			option.SetBoolPtr(bToggle);
			option.m_Index = (int)m_Options.size();
			m_Options.push_back(option);
			m_NumOptions++;
		}


		/// An option with left and right arrows with multiple choosable options.
		/// Passed function will execute when the vector option is switched.
		template<typename T>
		void AddVectorOption(const std::string& text, const std::string& footer, std::vector<T> vec, std::function<void()> func = [] {})
		{
			Option option(false, false, true, false, false);
			option.SetVector(vec);
			option.SetText(text);
			option.SetFooter(footer);
			option.SetFunction(func);
			option.m_Index = (int)m_Options.size();
			m_Options.push_back(option);
			m_NumOptions++;
		}


		void AddVectorOption(const std::string& text, const std::string& footer, std::vector<const char*> _vec, std::function<void()> func = [] {})
		{
			std::vector<std::string> _temp;
			for (int i = 0; i < _vec.size(); i++)
				_temp.push_back(std::string(_vec[i]));
			AddVectorOption(text, footer, _temp, func);
		}


		void AddVectorOption(const std::string& text, const std::string& footer, int numOptions, const std::string& startText = "", const std::string& endText = "", std::function<void()> func = [] {})
		{
			std::vector<std::string> _temp;
			for (int i = 0; i < numOptions; i++)
				_temp.push_back(startText + std::to_string(i) + endText);
			AddVectorOption(text, footer, _temp, func);
		}


		/// An option that brings you to a new submenu
		void AddSubmenuOption(const std::string& text, const std::string& footer, int id)
		{
			Option option(false, false, false, true, false);
			option.SetText(text);
			option.SetFooter(footer);
			option.SetSubmenuID(id);
			option.m_Index = (int)m_Options.size();
			m_Options.push_back(option);
			m_NumOptions++;
		}


		/// Inserts a blank spot into a page
		/// Mainly used for organization purposes
		void AddPageBreak(const std::string &text = "")
		{
			Option option(false, false, false, false, true);
			option.SetText(text);
			option.m_Index = (int)m_Options.size();
			m_Options.push_back(option);
			m_NumOptions++;
		}


		Option* GetOption(int index)
		{
			if (index >= m_Options.size()) {
#if ALLOCATE_CONSOLE
				std::cout << "[Submenu::GetOption] [ERROR]: Bad index (" << index << "). Submenu ID: " << m_ID << ", m_Options.size(): " << m_Options.size() << ", returning nullptr" << "\n";
#endif
				return nullptr;
			} else {
				return &m_Options[index];
			}
		}


		void DeleteOption(Option* option)
		{
			for (auto it = m_Options.begin(); it != m_Options.end(); it++) {
				if (it->m_Index == option->m_Index) {
					m_Options.erase(it);
					m_NumOptions--;
					break;
				}
			}
		}


		void DeleteOption(int optionIndex)
		{
			for (auto it = m_Options.begin(); it != m_Options.end(); it++) {
				if (it->m_Index == optionIndex) {
					m_Options.erase(it);
					m_NumOptions--;
					break;
				}
			}
		}


		// Clear all options in this submenu
		void Clear()
		{
			m_Options.clear();
			m_Options.shrink_to_fit();
			m_NumOptions = 0;
		}
	};
}
