/*
----------------------
BeDC License
----------------------

Copyright 2002, The BeDC team.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions, and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions, and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the BeDC team nor the names of its 
   contributors may be used to endorse or promote products derived from 
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
*/
#include "DCStrings.h"

#include <UTF8.h>

#include <string.h>
#include <stdio.h>

// This is the implementation of internationalization... 
// If you want your own language to be implemented, translate the
// english strings to your language, and send it my way :)
const char * DC_STR_ENGLISH[STR_NUM] =
{
	"Hubs",
	"Connect",
	"Refresh",
	"Next 50",
	"Previous 50",
	
	"Name",
	"Address",
	"Description",
	"Users",
	
	"Idle.",
	"Connecting...",
	"Connected, retreiving server list...",
	"Error connecting.",
	"Error sending request.",
	"Error receiving data.",
	"Number of servers: ",
	
	"File",
	"About",
	"Close",
	"Edit",
	"Preferences",
	"Windows",
	"Show Hubs",
	
	"Preferences",
	"General",
	"Personal Information",
	"Connection Settings",
	"Nick (Required)",
	"E-Mail",
	"Description",
	"Connection",
	"Active",
	"Passive (behind a firewall)",
	"IP",
	"Port",
	"Colors",
	"System Label",
	"Text",
	"Error Label",
	"Remote Nick",
	"Local Nick",
	"Private Text",
		
	// "Name" is reused
	"Speed",
	"Description",
	"E-Mail",
	"Shared",
	"Chat:",
	"Close", 
	
	"OK",
	"Cancel",
	"Error",
	"Users",
	"Language",
	"Revert",
	
	"Please choose a nick name in Edit->Preferences.",
	
	"System: ",
	"Error: ",
	"Connecting to ",
	"Connected.",
	"Connection to server failed!",
	"Trying to reconnect to server...",
	"Disconnected from server.",
	"Your nick is already in use, please choose another.",
	"User %s has logged in.",
	"User %s has logged out.",	// the user name is prepended to this string
	"Redirecting to ",
	"Hub is full.",
	"User not found!",
	"Unknown command.",
	
	// START /help
	"Available commands:\n"
	"\t\t\t/close - Close this connect.\n"
	"\t\t\t/help - Show this help text.\n"
	"\t\t\t/msg <name> <text> - Send a private message.\n"
	"\t\t\t/quit - Quit BDC",
	// END /help
};


// If you want different key shortcuts then the defaults for english,
// include your own language version of key shortcuts
const char DC_KEY_ENGLISH[KEY_NUM] =
{
	'A',	// About
	'W',	// Close
	'P',	// Prefs
	'H'		// Hubs
};


/////////////////////////////////////////////////
// SWEDISH
/////////////////////////////////////////////////
const char * DC_STR_SWEDISH[STR_NUM] =
{
	"Hubbar",
	"Anslut",
	"Uppdatera",
	"Nästa 50",
	"Föregående 50",
	
	"Namn",
	"Adress",
	"Beskrivning",
	"Användare",
	
	"Vilande.",
	"Ansluter...",
	"Ansluten, hämtar serverlista...",
	"Fel vid anslutning.",
	"Fel vid förfrågan.",
	"Fel vid hämtning av data.",
	"Antal servrar: ",
	
	"Fil",
	"Om",
	"Stäng",
	"Redigera",
	"Inställningar",
	"Fönster",
	"Lista Hubbar",
	
	"Inställningar",
	"Generella",
	"Personlig information",
	"Anslutnings inställningar",
	"Nick ( Krävs )",
	"E-Post",
	"Beskrivning",
	"Anslutning",
	"Aktiv",
	"Passiv (Jag är bakom brandvägg)",
	"IP",
	"Port",
	"Colors",	// TRANSLATE
	"System Label",	// TRANSLATE
	"Text",	// TRANSLATE
	"Error Label",	// TRANSLATE
	"Remote Nick",	// TRANSLATE
	"Local Nick",	// TRANSLATE
	"Private Text",	// TRANSLATE
	
	// "Name" is reused
	"Uppkoppling",
	"Beskrivning",
	"E-Post",
	"Delade",
	"Chat:",
	"Stäng", 
	
	"OK",
	"Avbryt",
	"Fel",
	"Användare",
	"Språk",
	"Revert",		// TRANSLATE

	"Vargod välj ett nytt Nickname i Redigera->Inställningar.",
	
	"System: ",
	"Fel: ",
	"Ansluter till ",
	"Ansluten.",
	"Anslutning till server misslyckades",
	"Försöker att återansluta till server...",
	"Ej ansluten till server.",
	"Någon annan använder ditt nickname, Välj ett nytt.",
	"Användare %s har anslutigt.",
	"Användare %s har loggat ut.",	// the user name is prepended to this string
	"Återansluter till ",
	"Hubben är redan full.",
	"Kan inte hitta användare",
	"Kommandot finns inte.",
	
	// START /help
	"Tillgängliga kommandon:\n"
	"\t\t\t/close - Stäng anslutning.\n"
	"\t\t\t/help - Visar det här medelandet.\n"
	"\t\t\t/msg <namn> <text> - Skickar ett privat medelande.\n"
	"\t\t\t/quit - Avslutar BDC",
	// END /help
};

const char DC_KEY_SWEDISH[KEY_NUM] =
{
	'A',	// About
	'C',	// Close
	'I',	// Prefs
	'H'		// Hubs
};


////////////////////////////////////////////
// FINNISH
////////////////////////////////////////////
const char * DC_STR_FINNISH[STR_NUM] =
{
	"Hubit",
	"Yhdistä",
	"Päivitä",
	"Seuraavat 50",
	"Edelliset 50",
	
	"Nimi",
	"Osoite",
	"Kuvaus",
	"Käyttäjät",
	
	"Vapaa.",
	"Yhdistetään...",
	"Yhdistetty, haetaan palvelinluetteloa...",
	"Virhe yhdistettäessä.",
	"Virhe lähetettäessä pyyntöä.",
	"Virhe vastaanotettaessa tietoa.",
	"Palvelimien määrä: ",
	
	"Tiedosto",
	"Tietoja",
	"Sulje",
	"Muokkaa",
	"Asetukset",
	"Ikkunat",
	"Näytä Hubit",
	
	"Asetukset",
	"Yleiset",
	"Henkilökohtaiset Tiedot",
	"Yhteysasetukset",
	"Nimi (Pakollinen)",
	"Sähköposti",
	"Kuvaus",
	"Yhteys",
	"Aktiivinen",
	"Passiivinen (palomuurin takana)",
	"IP",
	"Portti",
	"Colors",	// TRANSLATE
	"System Label",	// TRANSLATE
	"Text",	// TRANSLATE
	"Error Label",	// TRANSLATE
	"Remote Nick",	// TRANSLATE
	"Local Nick",	// TRANSLATE
	"Private Text",	// TRANSLATE
	
	// "Name" is reused
	"Nopeus",
	"Kuvaus",
	"Sähköposti",
	"Jaettuna",
	"Keskustelu:",
	"Sulje", 
	
	"OK",
	"Peruuta",
	"Virhe",
	"Käyttäjiä",
	"Kieli",
	"Revert",		// TRANSLATE
	
	"Ole hyvä ja valitse nimi valikosta Muokkaa->Asetukset.",
	
	"Järjestelmä: ",
	"Virhe: ",
	"Yhdistetään palvelimelle ",
	"Yhdistetty.",
	"Yhdistäminen palvelimeen epäonnistui!",
	"Yritetään yhdistää uudelleen...",
	"Yhteys palvelimeen on katkennut.",
	"Nimesi on jo käytössä, ole hyvä ja valitse toinen.",
	"Käyttäjä %s on kirjautunut sisään.",
	"Käyttäjä %s on kirjautunut ulos.",	// the user name is prepended to this string
	"Edelleensiirretään palvelimelle ",
	"Hubi on täynnä.",
	"Käyttäjää ei löytynyt!",
	"Tuntematon komento.",
	
	// START /help
	"Mahdolliset komennot:\n"
	"\t\t\t/close - Sulje tämä yhteys.\n"
	"\t\t\t/help - Näytä tämä ohjeteksti.\n"
	"\t\t\t/msg <name> <text> - Lähetä yksityisviesti.\n"
	"\t\t\t/quit - Lopeta BDC",
	// END /help
};

const char DC_KEY_FINNISH[KEY_NUM] =
{
	'T',	// About
	'U',	// Close
	'S',	// Prefs
	'H'		// Hubs
};

//////////////////////////////////////////////////////
// GERMAN
//////////////////////////////////////////////////////
const char * DC_STR_GERMAN[STR_NUM] =
{
	"Server",
	"Verbinden",
	"Aktualisieren",
	"Nächste 50",
	"Vorherige 50",
	
	"Name",
	"Adresse",
	"Beschreibung",
	"Benutzer",
	
	"Idle.",
	"Verbindet...",
	"Verbunden, empfange Serverliste...",
	"Fehler beim Verbinden.",
	"Fehler beim Senden der Anfrage.",
	"Fehler beim Datenempfang.",
	"Anzahl Server: ",
	
	"Datei",
	"Über",
	"Schließen",
	"Bearbeiten",
	"Einstellungen",
	"Fenster",
	"Zeige Server",
	
	"Einstellungen",
	"Allgemein",
	"Persönliche Informationen",
	"Verbindungseinstellungen",
	"Name (benötigt)", // could be left 'Nick' imho
	"E-Mail",
	"Beschreibung",
	"Verbindung",
	"Active",		// leave as is imho
	"Passive (hinter einer Firewall)",
	"IP",
	"Port",
	"Colors",	// TRANSLATE
	"System Label",	// TRANSLATE
	"Text",	// TRANSLATE
	"Error Label",	// TRANSLATE
	"Remote Nick",	// TRANSLATE
	"Local Nick",	// TRANSLATE
	"Private Text",	// TRANSLATE
	
	// "Name" is reused
	"Geschwindigkeit",
	"Beschreibung",
	"E-Mail",
	"Freigegeben", // taken from BeShare :)) but its the best-fitting one
	"Chat:",
	"Schließen", 
	
	"OK",
	"Abbrechen",
	"Fehler",
	"Benutzer",
	"Revert",		// TRANSLATE
	
	"Bitte wählen Sie einen Namen im Menu Bearbeiten->Einstellungen.",
	
	"System: ",
	"Fehler: ",
	"Verbinde zu ",
	"Verbunden.",
	"Verbindung zum Server ist fehlgeschlagen!",
	"Versuche Verbindung wieder herzustellen...",
	"Verbindung zum Server beendet.",
	"Ihr Name ist schon in Benutzung. Bitte wählen Sie einen anderen.",
	"Benutzer %s ist jetzt verbunden.",
	"Benutzer %s hat die Verbindung beendet.",	// the user name is prepended to this string
	"Weiterleitung zu ",
	"Server ist voll.",
	"Benutzer nicht gefunden!",
	"Unbekannter Befehl.",
	
	// START /help
	"Mögliche Befehle:\n"
	"\t\t\t/close - Schließt diese Verbindung.\n"
	"\t\t\t/help - Zeigt diesen Hilfetext.\n"
	"\t\t\t/msg <name> <text> - Sendet eine private Nachricht.\n"
	"\t\t\t/quit - Schließt BDC",
	// END /help
};


const char DC_KEY_GERMAN[KEY_NUM] =
{
	'A',	// About
	'W',	// Close
	'P',	// Prefs
	'H'		// Hubs
};

//////////////////////////////////////////////////////
// NORWEGIAN
//////////////////////////////////////////////////////
const char * DC_STR_NORWEGIAN[STR_NUM] =
{
	"Huber",
	"Koble til",
	"Oppdater",
	"Neste 50",
	"Forrige 50",
	
	"Navn",
	"Addresse",
	"Beskrivelse",
	"Antall brukere",
	
	"Idle.",
	"Kobler til...",
	"Tilkoblet, henter server liste...",
	"Feil under tilkobling.",
	"Feil under sending av forespørselen.",
	"Feil under henting av data.",
	"Antall servere: ",
	
	"Fil",
	"Om",
	"Lukk",
	"Edit",
	"Instillinger",
	"Vinduer",
	"Vis Huber",
	
	"Instillinger",
	"Generelt",
	"Personlig informasjon",
	"Tilkoblingsinstillinger",
	"Nick (Required)",
	"E-mail",
	"Beskrivelse",
	"Tilkobling",
	"Aktiv",
	"Passiv (bak brannmur)",
	"IP",
	"Port",
	"Colors",	// TRANSLATE
	"System Label",	// TRANSLATE
	"Text",	// TRANSLATE
	"Error Label",	// TRANSLATE
	"Remote Nick",	// TRANSLATE
	"Local Nick",	// TRANSLATE
	"Private Text",	// TRANSLATE
	
	// "Name" is reused
	"Speed",
	"Beskrivelse",
	"E-mail",
	"Delt",
	"Chat:",
	"Lukk", 
	
	"OK",
	"Avbryt",
	"Feil",
	"Brukere",
	"Språk",
	"Revert",		// TRANSLATE

	"Velg et nick i Edit->Instillinger.",
	
	"System: ",
	"Feil: ",
	"Kobler til ",
	"Tilkoblet.",
	"Tilkoblingen feilet!",
	"Prøver å koble til på nytt...",
	"Frakoblet server.",
	"Nicket du har valgt er allerede i bruk, velg et annet.",
	"%s har logget inn.",
	"%s har logget ut.",	// the user name is prepended to this string
	"Redirecting to ",
	"Huben er full.",
	"Kan ikke finne brukeren!",
	"Ukjent kommando.",
	
	// START /help
	"Tilgjengelige kommandoer:\n"
	"\t\t\t/close - Lukker tilkoblingen.\n"
	"\t\t\t/help - Viser denne hjelpen.\n"
	"\t\t\t/msg <navn> <tekst> - Sender en privat beskjed.\n"
	"\t\t\t/quit - Lukker BDC",
	// END /help
};

const char DC_KEY_NORWEGIAN[KEY_NUM] =
{
	'A',	// About
	'W',	// Close
	'P',	// Prefs
	'H'		// Hubs
};

//////////////////////////////////////////////////////
// POLISH
//////////////////////////////////////////////////////
const char * DC_STR_POLISH[STR_NUM] =
{
	"Huby",
	"Połącz",
	"Odśwież",
	"Następne 50",
	"Poprzednie 50",
	
	"Nazwa",
	"Adres",
	"Opis",
	"Użytkownicy",
	
	"Czekam.",
	"Łączę...",
	"Pobieram listę hubów...",
	"Błąd podczas łączenia.",
	"Błąd przy wysyłaniu żądania.",
	"Błąd przy otrzymywaniu danych.",
	"Ilość hubów: ",
	
	"Plik",
	"O programie",
	"Zamknij",
	"Edycja",
	"Ustawienia",
	"Okna",
	"Pokaż Huby",
	
	"Ustawienia",
	"Główne",
	"Informacje osobiste",
	"Ustawienia połączenia",
	"Ksywka (wymagane)",
	"E-Mail",
	"Opis",
	"Połączenie",
	"Aktywny",
	"Pasywny (za firewallem)",
	"IP",
	"Port",
	"Colors",	// TRANSLATE
	"System Label",	// TRANSLATE
	"Text",	// TRANSLATE
	"Error Label",	// TRANSLATE
	"Remote Nick",	// TRANSLATE
	"Local Nick",	// TRANSLATE
	"Private Text",	// TRANSLATE
	
	// "Name" is reused
	"Łącze",
	"Opis",
	"E-Mail",
	"Udostępnione",
	"Chat:",
	"Zamknij",
	
	"OK",
	"Anuluj",
	"Błąd",
	"Użytkownicy",
	"Język",
	"Revert",		// TRANSLATE
	
	"Wpisz ksywkę w  Edycja -> Ustawienia.",
	
	"System: ",
	"Błąd: ",
	"Łączę z ",
	"Połączony.",
	"Nie udało się połączyć z serwerem!!",
	"Próbuję połączyć się ponownie...",
	"Rozłączono z serwerem.",
	"Twoja ksywka jest już używana: wybierz inną.",
	"Użytkownik %s zalogował się.",
	"Użytkownik %s rozłączył się.",	// the user name is prepended to this string
	"Przekierowuję do ",
	"Hub jest pełny.",
	"Nie ma takiego użytkownika!",
	"Nieznane polecenie.",
	
	// START /help
	"Dostępne polecenia:\n"
	"\t\t\t/close - Zamyka to połączenie.\n"
	"\t\t\t/help - Pokazuje ten tekst pomocy.\n"
	"\t\t\t/msg <ksywka> <tekst> - Wysyła prywatną wiadomość.\n"
	"\t\t\t/quit - Zamyka BDC",
	// END /help
};


// If you want different key shortcuts then the defaults for english,
// include your own language version of key shortcuts
const char DC_KEY_POLISH[KEY_NUM] =
{
	'A',	// About
	'W',	// Close
	'P',	// Prefs
	'H'		// Hubs
};

//////////////////////////////////////////////////////
// RUSSIAN
//////////////////////////////////////////////////////
const char * DC_STR_RUSSIAN[STR_NUM] =
{
	"Узлы", //"Hubs",
	"Соединение", //"Connect",
	"Обновить", //"Refresh",
	"Следующие 50", //"Next 50",
	"Предыдущие 50", //"Previous 50",
	
	"Имя", //"Name",
	"Адрес", //"Address",
	"Описание", //"Description",
	"Пользователи", //"Users",
	
	"В спячке", //"Idle.",
	"Соединяюсь...", //"Connecting...",
	"Есть соединение, получаю список серверов", //"Connected, retreiving server list...",
	"Ошибка соединения", //"Error connecting.",
	"Ошибка посылки запроса", //"Error sending request.",
	"Ошибка приема данных", //"Error receiving data.",
	"Число серверов", //"Number of servers: ",
	
	"Файл", //"File",
	"О программе", //"About",
	"Закрыть", //"Close",
	"Правка", //"Edit",
	"Настройки", //"Preferences",
	"Окна", //"Windows",
	"Показать узлы", //"Show Hubs",
	
	"Настройки", //"Preferences",
	"Общие", //"General",
	"Персональные данные", //"Personal Information",
	"Параметры соединения", //"Connection Settings",
	"Псевдоним (обязательно!)", //"Nick (Required)",
	"Адрес e-mail", //"E-Mail",
	"Описание", //"Description",
	"Соединение", //"Connection",
	"Активное", //"Active",
	"Пассивное (за файрволлом)", //"Passive (behind a firewall)",
	"IP", //"IP",
	"Порт", //"Port",
	
	// "", //"Name" is reused
	"Скорость", //"Speed",
	"Описание", //"Description",
	"Адрес e-mail", //"E-Mail",
	"Разделяемые файлы", //"Shared",
	"Беседка", //"Chat:",
	"Закрыть", //"Close", 
	
	"Да", //"OK",
	"Отмена", //"Cancel",
	"Ошибка", //"Error",
	"Пользователи", //"Users",
	"Language", //"Language",
	
	"", //"Please choose a nick name in Edit->Preferences.",
	
	"Система: ", //"System: ",
	"Ошибка: ", //"Error: ",
	"Соединяюсь с ", //"Connecting to ",
	"", //"Connected.",
	"Соединение с сервером не удалось", //"Connection to server failed!",
	"Повторяю попытку соединения с сервером...", //"Trying to reconnect to server...",
	"Соединение с сервером разорвано", //"Disconnected from server.",
	"Ваш псевдоним уже занят, используйте другой", //"Your nick is already in use, please choose another.",
	"Вошел пользователь %", //"User %s has logged in.",
	"Пользователь % вышел", //"User %s has logged out.",	// the user name is prepended to this string
	"Перенаправление на ", //Redirecting to ",
	"Нет места на узле", "Hub is full.",
	"Нет такого пользователя", "User not found!",
	"Неизвестная команда", "Unknown command.",
	
	// START /help
	"Доступные команды:\n"
	"\t\t\t/close - Закрыть соединение.\n"
	"\t\t\t/help - Показать эту справку.\n"
	"\t\t\t/msg <name> <text> - Послать личное сообщение.\n"
	"\t\t\t/quit - Выйти из BDC",
	// END /help
};

const char DC_KEY_RUSSIAN[KEY_NUM] =
{
	'A',	// About
	'W',	// Close
	'P',	// Prefs
	'H'		// Hubs
};


const char ** DC_STR_USE = 0;	// Set to the current language in use
const char * DC_KEY_USE = 0;	// Set to the current key set in use

const char * DC_LANGUAGES[DC_LANG_NUM] =
{
	"English",
	"Svensk",
	"suomi",
	"Deutsch",
	"Norsk",
	"polski",
	"Русский"	// I hope Copy+Paste worked ;)
};

int gCurrentLanguage = -1;

const char * 
DCStr(int str)
{
	return DC_STR_USE[str];	// invalid index will cause a CRASH
}

int
DCGetLanguage()
{
	return gCurrentLanguage;
}

void
DCSetLanguage(int lang)
{
	switch (lang)
	{
		case DC_LANG_ENGLISH:
		{
			DC_STR_USE = DC_STR_ENGLISH;
			DC_KEY_USE = DC_KEY_ENGLISH;
			break;
		}
		
		case DC_LANG_SWEDISH:
		{
			DC_STR_USE = DC_STR_SWEDISH;
			DC_KEY_USE = DC_KEY_ENGLISH;
			break;
		}
		
		case DC_LANG_FINNISH:
		{
			DC_STR_USE = DC_STR_FINNISH;
			DC_KEY_USE = DC_KEY_FINNISH;
			break;
		}
		
		case DC_LANG_GERMAN:
		{
			DC_STR_USE = DC_STR_GERMAN;
			DC_KEY_USE = DC_KEY_FINNISH;
			break;
		}
		case DC_LANG_NORWEGIAN:
		{
			DC_STR_USE = DC_STR_NORWEGIAN;
			DC_KEY_USE = DC_KEY_NORWEGIAN;
			break;
		}
		
		case DC_LANG_POLISH:
		{
			DC_STR_USE = DC_STR_POLISH;
			DC_KEY_USE = DC_KEY_POLISH;
			break;
		}
		
		case DC_LANG_RUSSIAN:
		{
			DC_STR_USE = DC_STR_RUSSIAN;
			DC_KEY_USE = DC_KEY_RUSSIAN;
			break;
		}
		
		default:
			break;
	};
	gCurrentLanguage = lang;
}

char
DCKey(int key)
{
	return DC_KEY_USE[key];
}

BString
DCUTF8(const char * str)
{
	BString ret(str);
	int32 srcSize = strlen(str) + 1;
	int32 convSize = srcSize * 2 + 4 /* 4 for padding... just in case */;
	char * convStr = new char[convSize + 1];
	
	if (convert_to_utf8(B_MS_WINDOWS_CONVERSION, str, &srcSize, convStr, &convSize, NULL) == B_OK)
	{
		ret.SetTo(convStr);
	}
	delete [] convStr;
	return ret;
}

// Convert UTF8 to MS encoding
BString
DCMS(const char * str)
{
	BString ret(str);
	int srcSize = strlen(str) + 1;
	char * convBuffer = new char[srcSize + 1];
	
	memset(convBuffer, 0, srcSize);
	if (convert_from_utf8(B_MS_WINDOWS_CONVERSION, str, (int32 *)&srcSize, convBuffer, (int32 *)&srcSize, 0) == B_OK)
	{
		ret.SetTo(convBuffer);
	}
	
	delete [] convBuffer;
	return ret;	
}
