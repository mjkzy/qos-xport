#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "gsc.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace gsc
{
	/*
	namespace
	{
		std::unordered_map<std::string, game::scr_function_t> scr_functions;
		std::unordered_map<std::string, game::scr_method_t> scr_methods;

		std::unordered_map<std::string, std::uint32_t> main_handles;
		std::unordered_map<std::string, std::uint32_t> init_handles;

		std::unordered_map<const char*, const char*> replaced_functions;
		const char* replaced_pos = nullptr;

		game::method_t player_get_method_stub(const char** name)
		{
			const auto got = scr_methods.find(*name);
			if (got != scr_methods.end())
			{
				return got->second.call;
			}

			return utils::hook::invoke<game::method_t>(0x52E050, name);
		}

		game::function_t scr_get_function_stub(const char** name, int* type)
		{
			const auto got = scr_functions.find(*name);
			if (got != scr_functions.end())
			{
				*type = got->second.developer;
				return got->second.call;
			}

			return utils::hook::invoke<game::function_t>(0x50D280, name, type);
		}

		void load_gametype_script_stub()
		{
			main_handles.clear();
			init_handles.clear();

			game::FS_ForEachFile("scripts", "gsc", [](char* filename)
			{
				std::string file = filename;
				file = "scripts/" + file;

				if (utils::string::ends_with(file, ".gsc"))
				{
					file = file.substr(0, file.size() - 4);
				}

				console::info("Loading '%s.gsc'...\n", file.data());

				if (!game::Scr_LoadScript(file.data()))
				{
					console::info("Could not load '%s.gsc'.\n", file.data());
					return;
				}

				const auto main_handle = game::Scr_GetFunctionHandle(file.data(), "main");
				const auto init_handle = game::Scr_GetFunctionHandle(file.data(), "init");

				if (main_handle)
				{
					console::info("Loaded '%s::main'\n", file.data());
					main_handles[file] = main_handle;
				}

				if (init_handle)
				{
					console::info("Loaded '%s::init'\n", file.data());
					init_handles[file] = init_handle;
				}

				console::info("Loaded script '%s.gsc'\n", file.data());
			});

			game::GScr_LoadGameTypeScript();
		}

		void g_load_structs_stub()
		{
			for (auto& function_handle : main_handles)
			{
				console::debug("Executing '%s::main'\n", function_handle.first.data());
				game::RemoveRefToObject(game::Scr_ExecThread(function_handle.second, 0));
			}

			utils::hook::invoke<void>(0x5118A0);
		}

		void save_reigstered_weapons_stub()
		{
			for (auto& function_handle : init_handles)
			{
				console::debug("Executing '%s::init'\n", function_handle.first.data());
				game::RemoveRefToObject(game::Scr_ExecThread(function_handle.second, 0));
			}

			utils::hook::invoke<void>(0x51F880);
		}

		const char* get_code_pos_for_param(int index)
		{
			if (static_cast<unsigned int>(index) >= game::scrVmPub->outparamcount)
			{
				game::Scr_ParamError(static_cast<unsigned int>(index), "get_code_pos_for_param: Index is out of range!\n");
				return "";
			}

			const auto value = &game::scrVmPub->top[-index];

			if (value->type != game::scrParamType_t::VAR_FUNCTION)
			{
				game::Scr_ParamError(static_cast<unsigned int>(index), "get_code_pos_for_param: Invalid type! (Expects a function as parameter)\n");
				return "";
			}

			return value->u.codePosValue;
		}

		void get_replaced_pos(const char* pos)
		{
			if (replaced_functions.contains(pos))
			{
				replaced_pos = replaced_functions[pos];
			}
		}

		void set_replaced_pos(const char* what, const char* with)
		{
			if (what[0] == '\0' || with[0] == '\0')
			{
				console::warn("Invalid parameters passed to set_replaced_pos\n");
				return;
			}

			if (replaced_functions.contains(what))
			{
				console::warn("replaced_functions already contains codePosValue for a function\n");
			}

			replaced_functions[what] = with;
		}

		__declspec(naked) void vm_execute_stub()
		{
			__asm
			{
				pushad

				push esi
				call get_replaced_pos
				pop esi
				popad

				cmp replaced_pos, 0
				jne set_pos

				movzx eax, byte ptr[esi]
				inc esi

				jmp loc_1
			loc_1:
				cmp eax, 0x86

				mov [ebp + 0x64 - 0x44], eax
				mov [ebp + 0x64 + 0x8], esi

				push 0x47DB1F
				retn
			set_pos:
				mov esi, replaced_pos
				mov replaced_pos, 0

				movzx eax, byte ptr[esi]
				inc esi

				jmp loc_1
			}
		}
	}

	void add_function(const char* name, game::function_t func, int type)
	{
		game::scr_function_t function;
		function.call = func;
		function.developer = type;

		scr_functions[utils::string::to_lower(name)] = function;
	}

	void add_method(const char* name, game::method_t func, int type)
	{
		game::scr_method_t method;
		method.call = func;
		method.developer = type;

		scr_methods[utils::string::to_lower(name)] = method;
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			// custom gsc functions/methods
			utils::hook::call(0x46E9CB, player_get_method_stub);
			utils::hook::call(0x46E7BF, scr_get_function_stub);

			// load custom scripts
			utils::hook::call(0x5043FA, load_gametype_script_stub); // load our custom gsc

			// execute handles
			utils::hook::call(0x4FC75F, g_load_structs_stub); // execute main handles (Scr_LoadGameType is inlined)
			utils::hook::call(0x4FC7CE, save_reigstered_weapons_stub); // execute init handles (Scr_StartupGameType is inlined)

			// hook vm_execute to redirect function calls
			utils::hook::jump(0x47DB10, vm_execute_stub);

			add_function("replacefunc", []()
			{
				if (game::Scr_GetNumParam() != 2)
				{
					game::Scr_Error("replacefunc: two parameters are required.\n");
				}

				const auto what = get_code_pos_for_param(0);
				const auto with = get_code_pos_for_param(1);

				set_replaced_pos(what, with);
			});

			// reset replaced functions on game shutdown
			scheduler::on_shutdown([]
			{
				replaced_functions.clear();
			});
		}
	};
	*/
}

//REGISTER_COMPONENT(gsc::component)
 