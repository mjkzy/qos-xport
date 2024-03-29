#include <std_include.hpp>

#include "component/console.hpp"

#include "game/game.hpp"
#include "game/structs.IW4.hpp"

#include <utils/hook.hpp>
#include "Structs.IW4.hpp"


namespace game
{
	HMODULE mp_dll = nullptr;

	uintptr_t game_offset(uintptr_t ida_address)
	{
		if (mp_dll == nullptr)
		{
			mp_dll = GetModuleHandleA("jb_mp_s.dll");
			assert(mp_dll != nullptr);
		}

		return ida_address - 0x10000000 + reinterpret_cast<uintptr_t>(mp_dll);
	}

	void Cbuf_AddText(int controller, const char* text)
	{
		int func_loc = game_offset(0x103F5180);

		__asm
		{
			mov eax, text
			mov ecx, controller
			call func_loc
		}
	}

	// half of this is inlined on QoS, so just re-writing it all since its little work
	qos::cmd_function_s* Cmd_FindCommand(const char* name)
	{
		qos::cmd_function_s* command;

		for (command = *cmd_functions; command; command = command->next)
		{
			if (!strcmp(name, command->name))
			{
				return command;
			}
		}

		return 0;
	}

	void Cmd_AddCommandInternal(const char* name, void(__cdecl* function)(), qos::cmd_function_s* cmd)
	{
		if (Cmd_FindCommand(name))
		{
			if (function)
			{
				console::error("Cmd_AddCommand: %s already defined\n", name);
			}
		}
		else
		{
			cmd->name = name;
			cmd->function = function;
			cmd->next = *cmd_functions;
			*cmd_functions = cmd;
			console::debug("registered cmd '%s'\n", name);
		}
	}

	bool DB_IsXAssetDefault(qos::XAssetType type, const char* name)
	{
		int func_loc = game_offset(0x103DFC00);
		bool answer = false;
		int type_ = static_cast<int>(type);

		__asm
		{
			push ebx
			mov edi, type_
			call func_loc
			add esp, 4
			mov answer, al
		}

		return answer;
	}

	void DB_EnumXAssetEntries(qos::XAssetType type, std::function<void(qos::XAssetEntryPoolEntry*)> callback, bool overrides)
	{
		volatile long* lock = reinterpret_cast<volatile long*>(game_offset(0x1056250A));
		InterlockedIncrement(lock);

		while (*reinterpret_cast<volatile long*>(game_offset(0x105624F8))) std::this_thread::sleep_for(1ms);

		unsigned int index = 0;
		do
		{
			unsigned short hashIndex = game::db_hashTable[index];
			if (hashIndex)
			{
				do
				{
					qos::XAssetEntryPoolEntry* asset = &g_assetEntryPool[hashIndex];
					hashIndex = asset->entry.nextHash;
					if (asset->entry.asset.type == type)
					{
						callback(asset);
						if (overrides)
						{
							unsigned short overrideIndex = asset->entry.nextOverride;
							if (asset->entry.nextOverride)
							{
								do
								{
									asset = &g_assetEntryPool[overrideIndex];
									callback(asset);
									overrideIndex = asset->entry.nextOverride;
								} while (overrideIndex);
							}
						}
					}
				} while (hashIndex);
			}
			++index;
		} while (index < 0x9C40);
		InterlockedDecrement(lock);
	}

	void ConvertBounds(game::iw4::Bounds* bounds, game::qos::vec3_t mins, game::qos::vec3_t maxs)
	{
		for (int i = 0; i < 3; ++i)
		{
			bounds->halfSize[i] = (maxs[i] - mins[i]) / 2;
			bounds->midPoint[i] = bounds->halfSize[i] + mins[i];
		}
	}

	namespace iw4
	{
		void VectorSubtract(const vec3_t& a, const vec3_t& b, vec3_t& out)
		{
			out[0] = a[0] - b[0];
			out[1] = a[1] - b[1];
			out[2] = a[2] - b[2];
		}

		void Bounds::compute(vec3_t mins, vec3_t maxs)
		{
			ConvertBounds(this, mins, maxs);
		}
	}

	unsigned int Scr_GetFunctionHandle(const char* filename, const char* funcHandle)
	{
		int func_loc = game::game_offset(0x1022E410);
		unsigned int answer = 0;

		__asm
		{
			push funcHandle;
			mov eax, filename;
			call func_loc;
			add esp, 4;
			mov answer, eax;
		}

		return answer;
	}

	void RemoveRefToObject(unsigned int obj)
	{
		int func_loc = game::game_offset(0x10230DB0);

		__asm
		{
			mov edi, obj;
			call func_loc;
		}
	}

	__int16 Scr_ExecThread(int handle)
	{
		int func_loc = game::game_offset(0x1023B960);
		__int16 answer = 0;

		__asm
		{
			mov eax, handle;
			call func_loc;
			mov answer, ax;
		}

		return answer;
	}

	int Scr_LoadScript(const char* name)
	{
		int func_loc = game::game_offset(0x1022E7C0);
		int answer = 0;

		__asm
		{
			mov ecx, name;
			call func_loc;
			mov answer, eax;
		}

		return answer;
	}

	qos::dvar_s* Dvar_RegisterBool(const char* name, bool value, int flags, const char* desc)
	{
		int func_loc = game::game_offset(0x10278E60);
		qos::dvar_s* answer = nullptr;

		__asm
		{
			push flags
			push value
			push name
			mov edx, desc
			call func_loc;
			mov answer, eax;
			add esp, 0xC;
			mov answer, eax
		}

		return answer;
	}

	unsigned int Scr_GetNumParam()
	{
		return *scr_numParams;
	}

	char* Scr_ReadFile_FastFile(const char* file_name, int code_pos)
	{
		int func_loc = game::game_offset(0x1022DF13);
		char* answer = nullptr;

		__asm
		{
			push code_pos
			mov eax, file_name
			call func_loc;
			add esp, 0x4;
			mov answer, eax
		}

		return answer;
	}
}
