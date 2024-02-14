#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"
#include "gsc.hpp"
#include "scheduler.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace gsc_utils
{
	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			/*
			gsc::add_function("printf", []()
			{
				const auto message = game::Scr_GetString(0);
				console::info("%s\n", message);
			});
			*/
		}
	};
}

//REGISTER_COMPONENT(gsc_utils::component)
