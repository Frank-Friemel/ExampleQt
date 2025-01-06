#include "StringIDs.h"
#include "exception"
#include "Languages.h"

using namespace std;
using namespace string_literals;

namespace Localization::German
{
	string GetString(int id)
	{
		switch (id)
		{
		case StringID::OK:
			return "OK"s;

		case StringID::CLOSE:
			return "Schlie"s + CW2AEX(L"\xdf"s) + "en"s;

		case StringID::CANCEL:
			return "Abbrechen"s;

		case StringID::MENU_FILE:
			return "&Datei"s;

		case StringID::MENU_QUIT:
			return "&Beenden"s;

		case StringID::MENU_EDIT:
			return "B&earbeiten"s;

		case StringID::MENU_OPTIONS:
			return "&Optionen..."s;

		case StringID::MENU_HELP:
			return "&Hilfe"s;

		case StringID::MENU_ABOUT:
			return CW2AEX(L"\xdc"s) + "be&r..."s;

		case StringID::MENU_SHOW_APP_WINDOW:
			return "&Anzeigen"s;			

		case StringID::DIALOG_ABOUT:
			return CW2AEX(L"\xdc"s) + "ber"s;

		}
		return English::GetString(id);
	}
}