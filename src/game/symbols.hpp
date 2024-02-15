#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	WEAK symbol<void(int channel, const char* fmt, ...)> Com_Printf{ game_offset(0x103F6400) };

	WEAK symbol<DWORD> command_id{ game_offset(0x10752C70) };
	WEAK symbol<DWORD> cmd_argc{ game_offset(0x10752CB4) };
	WEAK symbol<char**> cmd_argv{ game_offset(0x10752CD4) };
	WEAK symbol<qos::cmd_function_s*> cmd_functions{ game_offset(0x10752CF8) };
}
