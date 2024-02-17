#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "component/assethandler.hpp"
#include "component/command.hpp"
#include "component/console.hpp"
#include "component/map_dumper.hpp"
#include "component/scheduler.hpp"

#include "game/game.hpp"
#include "game/structs.IW4.hpp"

#include <utils/memory.hpp>

namespace gfxworld
{
	namespace
	{
		game::iw4::GfxWorld* convert(game::qos::GfxWorld* world)
		{
			if (!world) return nullptr;

			game::iw4::GfxSky* sky = utils::memory::allocate<game::iw4::GfxSky>();

			game::iw4::GfxWorld iw4_asset;
			ZeroMemory(&iw4_asset, sizeof(iw4_asset));

			iw4_asset.name = world->name;
			iw4_asset.baseName = world->baseName;
			iw4_asset.planeCount = world->planeCount;
			iw4_asset.nodeCount = world->nodeCount;
			iw4_asset.surfaceCount = world->surfaceCount;

			iw4_asset.skyCount = 1;
			iw4_asset.skies = sky;

			if (world->skyImage)
			{
				sky->skyImage = assethandler::convert_asset_header(game::qos::ASSET_TYPE_IMAGE, { world->skyImage }).image;
			}

			sky->skySamplerState = world->skySamplerState/* & 0xFF*/;
			sky->skyStartSurfs = world->skyStartSurfs;
			sky->skySurfCount = world->skySurfCount;

			iw4_asset.lastSunPrimaryLightIndex = world->sunPrimaryLightIndex;
			iw4_asset.primaryLightCount = world->primaryLightCount;

			// dpvsPlanes
			auto dpvsPlanes = *world->planes;
			iw4_asset.dpvsPlanes.cellCount = dpvsPlanes.cellCount;
			memcpy(iw4_asset.dpvsPlanes.planes, dpvsPlanes.planes, 4);
			memcpy(iw4_asset.dpvsPlanes.nodes, dpvsPlanes.nodes, 4);
			memcpy(iw4_asset.dpvsPlanes.sceneEntCellBits, dpvsPlanes.sceneEntCellBits, 4);

			iw4_asset.dpvs.surfaceMaterials = utils::memory::allocate_array<game::iw4::GfxDrawSurf>(world->surfaceCount);
			for (auto i = 0; i < world->surfaceCount; i++)
			{
				iw4_asset.dpvs.surfaceMaterials[i].fields.objectId = world->surfaceMaterials[i].fields.objectId;
				iw4_asset.dpvs.surfaceMaterials[i].fields.reflectionProbeIndex = world->surfaceMaterials[i].fields.reflectionProbeIndex;
				iw4_asset.dpvs.surfaceMaterials[i].fields.customIndex = world->surfaceMaterials[i].fields.customIndex;
				iw4_asset.dpvs.surfaceMaterials[i].fields.materialSortedIndex = world->surfaceMaterials[i].fields.materialSortedIndex;
				iw4_asset.dpvs.surfaceMaterials[i].fields.prepass = world->surfaceMaterials[i].fields.prepass;
				iw4_asset.dpvs.surfaceMaterials[i].fields.sceneLightIndex = world->surfaceMaterials[i].fields.primaryLightIndex;
				iw4_asset.dpvs.surfaceMaterials[i].fields.surfType = world->surfaceMaterials[i].fields.surfType;
				iw4_asset.dpvs.surfaceMaterials[i].fields.primarySortKey = world->surfaceMaterials[i].fields.primarySortKey;
				iw4_asset.dpvs.surfaceMaterials[i].fields.unused = world->surfaceMaterials[i].fields.unused;
			}

			iw4_asset.dpvs.surfaceCastsSunShadow = world->surfaceCastsSunShadow;
			//iw4_asset.dpvs.usageCount = world->usageCount;

			// dpvsDyn
			//iw4_asset.dpvsDyn

			auto iw4_asset_output = utils::memory::allocate<game::iw4::GfxWorld>();
			*iw4_asset_output = iw4_asset;
			return { iw4_asset_output };
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			scheduler::once([&]()
			{
				command::add("dumpgfxworld", [](const command::params& params)
				{
					if (params.size() < 2)
					{
						console::info("USAGE: dumpgfxworld <name>\n");
						return;
					}

					const auto name = params[1];

					auto header = game::DB_FindXAssetHeader(game::qos::ASSET_TYPE_GFXWORLD, name);
					if (!header.comWorld)
					{
						console::error("dumpgfxworld failed on '%s'\n", name);
						return;
					}

					auto converted = gfxworld::convert(header.gfxWorld);
					map_dumper::api->write(game::iw4::ASSET_TYPE_GFXWORLD, converted);

					console::info("dumped '%s' for IW4\n", name);
				});
			}, scheduler::main);
		}

		game::qos::XAssetType get_type() override
		{
			return game::qos::ASSET_TYPE_GFXWORLD;
		}

		game::iw4::XAssetHeader convert_asset(game::qos::XAssetHeader header) override
		{
			return { convert(header.gfxWorld) };
		}
	};
}

REGISTER_COMPONENT(gfxworld::component)
