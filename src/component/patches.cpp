#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"

#include "game/game.hpp"

#include <mmeapi.h>

#include <utils/hook.hpp>
#include <utils/string.hpp>

#define FORCE_BORDERLESS true

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

		utils::hook::detour register_variant_hook;
		void* register_variant_stub(const char* name, int type, int flags, game::qos::DvarValue value, game::qos::DvarLimits domain, const char* desc)
		{
			const auto new_name = utils::string::to_lower(name);

			if (new_name == "cg_drawfps"s || new_name == "cg_drawfpslabels")
			{
				value.enabled = 1;
				value.integer = 1;
				console::debug("setting %s to true\n", name);
			}

			return register_variant_hook.invoke<void*>(name, type, flags, value, domain, desc);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			// TODO: hook Dvar_RegisterVariant
			//register_variant_hook.create(game::game_offset(0x102783C0), register_variant_stub);

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
		}
	};
}

REGISTER_COMPONENT(patches::component)
