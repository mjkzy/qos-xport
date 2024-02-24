#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"

#include <mmeapi.h>

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/string.hpp>

#define FORCE_BORDERLESS // still needs a few things fixed
#define XLIVELESS

namespace patches
{
	namespace
	{
		//game::qos::dvar_s* r_borderless = nullptr;

		int ret_one(DWORD*, int)
		{
			return 1;
		}

		/*
		void apply_borderless_stub()
		{
			int func_loc = game::game_offset(0x103BD711);

			__asm
			{
				mov ebp, r_borderless;
				cmp byte ptr[ebp + 0x10], 0;
				mov ebp, 0xC80000;
				jz not_borderless;
				mov ebp, 0x90000000;

			not_borderless:
				push func_loc;
				retn;
			}
		}
		*/
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			// un-cap fps
			utils::hook::set<std::uint8_t>(game::game_offset(0x103F696A), 0x00);

#ifdef FORCE_BORDERLESS
			// force fullscreen to always be false
			utils::hook::nop(game::game_offset(0x103BE1A2), 2);

			// change window style to 0x90000000
			utils::hook::set<std::uint8_t>(game::game_offset(0x103BD70C) + 3, 0x00);
			utils::hook::set<std::uint8_t>(game::game_offset(0x103BD70C) + 4, 0x90);

			/*
			scheduler::once([]()
			{
				r_borderless = game::Dvar_RegisterBool("r_borderless", true, 0, "Remove the windows border when in windowed mode.");
			}, scheduler::main);
			*/
#endif

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
