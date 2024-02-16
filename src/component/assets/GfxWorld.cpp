#include <std_include.hpp>
//#include "loader/component_loader.hpp"
//
//#include "component/assethandler.hpp"
//#include "component/command.hpp"
//#include "component/console.hpp"
//#include "component/map_dumper.hpp"
//#include "component/scheduler.hpp"
//
//#include "game/game.hpp"
//#include "game/structs.IW4.hpp"
//
//#include <utils/memory.hpp>
//
//namespace gfxworld
//{
//	namespace
//	{
//		game::iw4::GfxWorld* convert(game::qos::GfxWorld* world)
//		{
//			if (!world) return nullptr;
//
//			game::iw4::GfxSky* sky = utils::memory::allocate<game::iw4::GfxSky>();
//
//			game::iw4::GfxWorld map;
//			ZeroMemory(&map, sizeof(map));
//
//			map.name = world->name;
//			map.baseName = world->baseName;
//
//			return { iw4_asset };
//		}
//	}
//
//	class component final : public component_interface
//	{
//	public:
//		void post_load() override
//		{
//			scheduler::once([&]()
//			{
//				command::add("dumpgfxworld", [](const command::params& params)
//				{
//					if (params.size() < 2)
//					{
//						console::info("USAGE: dumpgfxworld <name>\n");
//						return;
//					}
//
//					const auto name = params[1];
//
//					auto header = game::DB_FindXAssetHeader(game::qos::ASSET_TYPE_GFXWORLD, name);
//					if (!header.comWorld)
//					{
//						console::error("dumpgfxworld failed on '%s'\n", name);
//						return;
//					}
//
//					auto converted = gfxworld::convert(header.comWorld);
//					map_dumper::api->write(game::iw4::ASSET_TYPE_GFXWORLD, converted);
//
//					console::info("dumped '%s' for IW4\n", name);
//				});
//			}, scheduler::main);
//		}
//
//		game::qos::XAssetType get_type() override
//		{
//			return game::qos::ASSET_TYPE_GFXWORLD;
//		}
//
//		game::iw4::XAssetHeader convert_asset(game::qos::XAssetHeader header) override
//		{
//			return { convert(header.gfxWorld) };
//		}
//	};
//}
//
//REGISTER_COMPONENT(gfxworld::component)
