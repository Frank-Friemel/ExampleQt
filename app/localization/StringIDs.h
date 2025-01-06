#pragma once
#include "LayerCake.h"
#include "languages/ILanguageManager.h"
#include <vector>

enum StringID {
	OK = 1,
	CANCEL,
	CLOSE,
	MENU_FILE,
	MENU_QUIT,
	MENU_EDIT,
	MENU_OPTIONS,
	MENU_HELP,
	MENU_ABOUT,
	MENU_SHOW_APP_WINDOW,

	DIALOG_ABOUT,

};

 enum MsgBoxType {
	NoIcon = 0,
	Information = 1,
	Warning = 2,
	Critical = 3,
	Question = 4
};

int ShowMessageBox(const SharedPtr<IValueCollection>& config, int message, void* parent = nullptr, MsgBoxType type = MsgBoxType::Information,
	std::vector<int> buttons = { StringID::OK });

SharedPtr<ILanguageManager> GetLanguageManager(const SharedPtr<IValueCollection>& config);
