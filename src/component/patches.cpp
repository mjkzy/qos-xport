#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"

#include "game/game.hpp"

#include <mmeapi.h>

#include <utils/hook.hpp>
#include <utils/string.hpp>

#define FORCE_BORDERLESS
#define XLIVELESS

namespace patches
{
	namespace
	{
		int ret_zero()
		{
			return 0;
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

		utils::hook::detour link_xasset_entry_hook;
		game::qos::XAssetEntry* link_xasset_entry_stub(game::qos::XAssetEntry* entry, int override)
		{
			if (entry->asset.type == game::qos::ASSET_TYPE_PHYSPRESET)
			{
				const auto troll = entry->asset.header.physPreset;
				printf("");
			}

			return link_xasset_entry_hook.invoke<game::qos::XAssetEntry*>(entry, override);
		}

		int PlayListBypass(DWORD* unk, int a2)
		{
			return 1;
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
			utils::hook::call(game::game_offset(0x103BE2AD), ret_zero); // vid_xpos
			utils::hook::call(game::game_offset(0x103BE2DF), ret_zero); // vid_ypos

			// intercept import for CreateWindowExA to change window stuff
			utils::hook::set(game::game_offset(0x1047627C), create_window_ex_stub);
#endif

			// un-cap fps
			utils::hook::set<uint8_t>(game::game_offset(0x103F696A), 0x00);

#ifdef DEBUG
			// hook linkxassetentry to debug stuff
			//link_xasset_entry_hook.create(game::game_offset(0x103E0640), link_xasset_entry_stub);
#endif

#ifdef XLIVELESS
			// stop disconnect error when starting match
			// Bypass playlist + stats
			utils::hook::jump(game::game_offset(0x10240B30), PlayListBypass);
			utils::hook::jump(game::game_offset(0x10240A30), PlayListBypass);

			// Allow map Loading
			utils::hook::nop(game::game_offset(0x102489A1), 5);
#endif
		}
	};
}

REGISTER_COMPONENT(patches::component)
