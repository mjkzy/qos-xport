#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	//WEAK symbol<void(int controller, const char* text)> Cbuf_InsertText{ game_offset(0x103F50E0) };

	WEAK symbol<void(int channel, const char* fmt, ...)> Com_Printf{ game_offset(0x103F6400) };

	WEAK symbol<void()> Scr_BeginLoadScripts{0x474B60};
	WEAK symbol<void()> Scr_EndLoadScripts{0x474FA0};

	WEAK symbol<int(char*)> Scr_LoadScript{0x474D80};
	WEAK symbol<void()> GScr_LoadGameTypeScript{0x503F90};

	WEAK symbol<void(const char* string)> Scr_AddString{0x483770};

	WEAK symbol<serverStatic_t> svs{0xD35700};

	// retail version of cod 2 1.3
	WEAK symbol<scrVmPub_t> scrVmPub{0xF4B900};
	//WEAK symbol<VariableValueInternal> scr_VarGlob{0xE08F00};
	//WEAK symbol<scrVarPub_t> scr_VarPub{0xF08F88};
	//WEAK symbol<scrVmPub_t> fs{0xF4B900};

	WEAK symbol<int> level_time{0x193A96C};
}