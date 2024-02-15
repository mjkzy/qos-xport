#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "component/command.hpp"
#include "component/console.hpp"
#include "component/scheduler.hpp"

#include "game/game.hpp"
#include "game/structs.IW4.hpp"

#include <utils/memory.hpp>

namespace rawfile
{
	namespace
	{
		game::iw4::RawFile* convert(game::qos::RawFile* rawfile)
		{
			if (!rawfile || !rawfile->buffer) return nullptr;

			auto iw4_asset = utils::memory::allocate<game::iw4::RawFile>();
			iw4_asset->name = rawfile->name;
			iw4_asset->compressedLen = 0;
			iw4_asset->len = 0;
			iw4_asset->buffer = rawfile->buffer;

			return iw4_asset;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			scheduler::once([]()
			{
				command::add("dumprawfile", [](const command::params& params)
				{
					// TODO
				});
			}, scheduler::main);
		}

		game::qos::XAssetType get_type() override
		{
			return game::qos::ASSET_TYPE_RAWFILE;
		}

		game::iw4::XAssetHeader convert_asset(game::qos::XAssetHeader header) override
		{
			return { convert(header.rawfile) };
		}
	};
}

REGISTER_COMPONENT(rawfile::component)
