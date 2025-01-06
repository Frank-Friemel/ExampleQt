#include "StringIDs.h"
#include "exception"
#include "Languages.h"

using namespace std;
using namespace string_literals;

namespace Localization::English
{
	string GetString(int id)
	{
		switch (id)
		{
		case StringID::OK:
			return "OK"s;

		case StringID::CANCEL:
			return "Cancel"s;

		case StringID::CLOSE:
			return "Close"s;

		case StringID::MENU_FILE:
			return "&File"s;

		case StringID::MENU_QUIT:
			return "&Quit"s;

		case StringID::MENU_EDIT:
			return "E&dit"s;

		case StringID::MENU_OPTIONS:
			return "&Options..."s;

		case StringID::MENU_HELP:
			return "&Help"s;

		case StringID::MENU_ABOUT:
			return "&About..."s;

		case StringID::MENU_SHOW_APP_WINDOW:
			return "Sh&ow"s;

		case StringID::DIALOG_ABOUT:
			return "About"s;

		}
		throw runtime_error("undefined string-id");
	}
}