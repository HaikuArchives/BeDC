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
	"Fänster",
	"Visa hubbar",
	
	"Inställningar",
	"Generella",
	"Personlig information",
	"Anslutnings inställningar",
	"Nick (Måste)",
	"E-Post",
	"Beskrivning",
	"Anslutning",
	"Aktiv",
	"Passiv (Jag är bakom brandvägg)",
	"IP",
	"Port",
	
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
	"Redirecting to ",
	"Hubben är redan full.",
	"Kan inte hitta användare",
	"Kommandot finns inte.",
	
	// START /help
	"Tillgängliga kommandon:\n"
	"\t\t\t/close - Stäng anslutning.\n"
	"\t\t\t/help - Visar det här medelandet.\n"
	"\t\t\t/msg <namn> <text> - SKickar ett privat medelande.\n"
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

const char ** DC_STR_USE = 0;	// Set to the current language in use
const char * DC_KEY_USE = 0;	// Set to the current key set in use

const char * 
DCStr(int str)
{
	return DC_STR_USE[str];	// invalid index will cause a CRASH
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
			
		default:
			break;
	};
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
