// Licensed under the MIT License.

#pragma once
#include "option.hpp"
#include "submenu.hpp"
#include "../script.h"
#include "../common.hpp"


#define BUILD_1311_COMPATIBLE FALSE // If true, the menu will be compatible with game version <= 1311.12
#define ALLOCATE_CONSOLE FALSE // To be used for debugging purposes
#if ALLOCATE_CONSOLE
	#define LOG(msg)(std::cout << msg << std::endl)
#endif


namespace Menu
{
	class CNativeMenu
	{
	private:
		static inline std::unordered_map<int, Submenu> g_SubmenusMap;
	public:
		Submenu* CurrentSubmenu = nullptr;			// Current Submenu
		int CurrentSubmenuID = Submenu_Invalid;		// Current Submenu ID
		int SelectionIndex = 0;						// Current Selected Option Index

		void AddSubmenu(const std::string& header, const std::string& subHeader, int id, int numVisibleOptions, std::function<void(Submenu*)> submenuFunc)
		{
			Submenu submenu(header, subHeader, id, numVisibleOptions, submenuFunc);
			g_SubmenusMap[id] = submenu;
		}


		// NOTE: THIS FUNCTION IS A LITTLE BROKEN
		// bSetRememberedSelection: Should be TRUE if going backwards
		void GoToSubmenu(int id, bool bSetRememberedSelection)
		{
			if (!g_SubmenusMap.contains(id)) {
#if ALLOCATE_CONSOLE
				std::cout << "[CNativeMenu::GoToSubmenu] [ERROR]: Submenu '" << id << "' doesn't exist" << "\n";
#endif
				return;
			}

			// This should only be the case when first initializing the menu
			if (CurrentSubmenu == nullptr) {
				CurrentSubmenu = &g_SubmenusMap[id];
				return;
			}

			if (id > CurrentSubmenu->m_ID) {
				// we went forward
				m_PrevSubmenuIds.push_back(CurrentSubmenu->m_ID);
				m_SubmenuLastSelections[CurrentSubmenu->m_ID] = SelectionIndex;
			}
			else if (id < CurrentSubmenu->m_ID) {
				// we went back
				m_PrevSubmenuIds.pop_back();
				m_SubmenuLastSelections.erase(CurrentSubmenu->m_ID);
			}

			CurrentSubmenu = &g_SubmenusMap[id];

			if (bSetRememberedSelection)
				SelectionIndex = m_SubmenuLastSelections[CurrentSubmenu->m_ID];
			else
				SelectionIndex = 0;
		}


		Submenu* GetSubmenu(int id)
		{
			if (g_SubmenusMap.contains(id)) {
				return &g_SubmenusMap[id];
			}

#if ALLOCATE_CONSOLE
			std::cout << "[CNativeMenu::GetSubmenu] [ERROR]: Invalid ID (" << id << "), returning nullptr" << "\n";
#endif
			return nullptr;
		}


		Option* GetSelectedOption()
		{
			return &g_SubmenusMap[CurrentSubmenu->m_ID].m_Options[SelectionIndex];
		}


		bool DoesSubmenuExist(int id)
		{
			return g_SubmenusMap.contains(id);
		}


		// Remove submenus entirely
		void RemoveSubmenusAtAndAfterThisID(int id)
		{
			for (auto it = g_SubmenusMap.begin(); it != g_SubmenusMap.end(); it++) {
				if (it->first >= id) {
					g_SubmenusMap.erase(it);
				}
			}
		}


		// Remove a submenu entirely
		void RemoveSubmenu(int id...)
		{
			std::vector<int> toRemove = { id };
			for (int i = 0; i < toRemove.size(); i++) {
				if (DoesSubmenuExist(toRemove[i])) {
					g_SubmenusMap.erase(toRemove[i]);
				}
			}
		}


		// Clear all options from a submenu
		void ClearSubmenu(Submenu* sub...)
		{
			std::vector<Submenu*> toClear = { sub };
			for (int i = 0; i < toClear.size(); i++) {
				if (DoesSubmenuExist(toClear[i]->m_ID)) {
					toClear[i]->Clear();
				}
			}
		}


		// Clear all options from a submenu
		void ClearSubmenu(int id...)
		{
			std::vector<int> toClear = { id };
			for (int i = 0; i < toClear.size(); i++) {
				if (DoesSubmenuExist(toClear[i])) {
					GetSubmenu(toClear[i])->Clear();
				}
			}
		}
		

		bool IsOpen() const
		{
			return m_IsOpen;
		}


		void SetEnabled(bool bEnabled, bool bPlaySounds);
		void Update();
		void RegisterUIPrompts();

	private:
		// Misc Variables

		bool m_IsOpen = false;
		// Contains submenu IDs you were previously at to properly backtrack
		std::vector<int> m_PrevSubmenuIds = {};
		// Contains the last selection you were at in a sub menu
		// Submenu ID, Selection Index
		std::unordered_map<int, int> m_SubmenuLastSelections = {};

		// Input

		int m_ControlIndex = INPUT_GROUP_KEYBOARD;
		bool m_OpenKeyPressed = false;
		bool m_BackKeyPressed = false;
		bool m_EnterKeyPressed = false;
		bool m_UpKeyPressed = false;
		bool m_DownKeyPressed = false;
		bool m_LeftKeyPressed = false;
		bool m_RightKeyPressed = false;

		// Prompts

		Prompt m_SelectPrompt = NULL;
		Prompt m_BackPrompt = NULL;
		bool m_SelectPromptCompleted = false;
		bool m_BackPromptCompleted = false;

		// Func declarations

		void CheckInput();
		void HandleInput();
		void DisableCommonInputs();
	};
	
	inline std::unique_ptr<CNativeMenu> g_Menu;
}
