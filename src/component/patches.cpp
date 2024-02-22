#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"

#include "game/game.hpp"

#include <mmeapi.h>

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/string.hpp>

#define FORCE_BORDERLESS // still needs a few things fixed
#define XLIVELESS

#pragma pack(push, 4)
struct GfxWindowParms
{
	char __pad[8]; // 0
	bool fullscreen; // 8
	int x; // 12
	int y; // 16
	int padding; // 20
	int sceneWidth; // 24
	int sceneHeight; // 28
	int displayWidth; // 32
	int displayHeight; // 36
	int aaSamples; // 40
	// 44
};
#pragma pack(pop)

namespace patches
{
	namespace
	{
		int ret_one(DWORD*, int)
		{
			return 1;
		}

		HWND __stdcall create_window_ex_stub(DWORD ex_style, LPCSTR class_name, LPCSTR window_name, DWORD style, int x, int y, int width, int height, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
		{
			if (!strcmp(class_name, "JB_MP"))
			{
				ex_style = 0;
				style = WS_POPUP;
				x = 0;
				y = 0;
			}

			return CreateWindowExA(ex_style, class_name, window_name, style, x, y, width, height, parent, menu, inst, param);
		}

		utils::hook::detour create_window_hook;
		void create_window_stub(GfxWindowParms* wndParms)
		{
			create_window_hook.invoke<void>(wndParms);
			const auto parms = wndParms;
			printf("");
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
#ifdef FORCE_BORDERLESS
			// force fullscreen to always be false
			utils::hook::nop(game::game_offset(0x103BE1A2), 2);

			// return 0 for x & y pos
			/*
			utils::hook::call(game::game_offset(0x103BE2AD), vidpos_register_int_stub); // vid_xpos
			utils::hook::call(game::game_offset(0x103BE2DF), vidpos_register_int_stub); // vid_ypos
			utils::hook::call(game::game_offset(0x103BD364), vidpos_register_int_stub); // ^
			utils::hook::call(game::game_offset(0x103BD399), vidpos_register_int_stub); // ^
			*/

			// r_createwindow
			//create_window_hook.create(game::game_offset(0x103BD690), &create_window_stub);

			// intercept import for CreateWindowExA to change window stuff
			utils::hook::set(game::game_offset(0x1047627C), create_window_ex_stub);
#endif

			// un-cap fps
			utils::hook::set<uint8_t>(game::game_offset(0x103F696A), 0x00);

#ifdef DEBUG
			// hook linkxassetentry to debug stuff
			//link_xasset_entry_hook.create(game::game_offset(0x103E0640), link_xasset_entry_stub);
#endif

// support xliveless emulator
#ifdef XLIVELESS
			// bypass playlist + stats
			utils::hook::jump(game::game_offset(0x10240B30), ret_one);
			utils::hook::jump(game::game_offset(0x10240A30), ret_one);

			// allow map loading
			utils::hook::nop(game::game_offset(0x102489A1), 5);
#endif
		}
	};
}

REGISTER_COMPONENT(patches::component)
