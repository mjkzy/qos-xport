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

#define COPY_VALUE_GFXWORLD(name) iw4_asset->name = world->name
#define COPY_ARR_GFXWORLD(name) memcpy(&iw4_asset->name, world->name, sizeof(iw4_asset->name));

namespace gfxworld
{
	namespace
	{
		game::iw4::GfxWorld* convert(game::qos::GfxWorld* world)
		{
			if (!world) return nullptr;

			game::iw4::GfxSky* sky = utils::memory::allocate<game::iw4::GfxSky>();

			auto iw4_asset = utils::memory::allocate<game::iw4::GfxWorld>();

			iw4_asset->name = world->name;
			iw4_asset->baseName = world->baseName;
			iw4_asset->planeCount = world->planeCount;

			// planes
			iw4_asset->dpvsPlanes.cellCount = world->planes->cellCount;
			memcpy(iw4_asset->dpvsPlanes.planes, world->planes->planes, 4);
			memcpy(iw4_asset->dpvsPlanes.nodes, world->planes->nodes, 4);
			memcpy(iw4_asset->dpvsPlanes.sceneEntCellBits, world->planes->sceneEntCellBits, 4);

			iw4_asset->nodeCount = world->nodeCount;
			// TODO: nodes?

			//iw4_asset->indexCount = world->indexCount;

			iw4_asset->surfaceCount = world->surfaceCount;
			if (world->surfaces)
			{
				iw4_asset->dpvs.surfaces = utils::memory::allocate_array<game::iw4::GfxSurface>(world->surfaceCount);
				iw4_asset->dpvs.surfacesBounds = utils::memory::allocate_array<game::iw4::GfxSurfaceBounds>(world->surfaceCount);

				for (int i_ = 0; i_ < world->surfaceCount; i_++)
				{
					iw4_asset->dpvs.surfaces[i_].tris = world->surfaces[i_].tris;

					// material
					auto iw4_asset_mat = assethandler::convert_asset_header(game::qos::ASSET_TYPE_MATERIAL, { world->surfaces[i_].material }).material;
					iw4_asset->dpvs.surfaces[i_].material = iw4_asset_mat;
					
					iw4_asset->dpvs.surfaces[i_].lightmapIndex = world->surfaces[i_].lightmapIndex;
					iw4_asset->dpvs.surfaces[i_].reflectionProbeIndex = world->surfaces[i_].reflectionProbeIndex;
					iw4_asset->dpvs.surfaces[i_].primaryLightIndex = world->surfaces[i_].primaryLightIndex;
					iw4_asset->dpvs.surfaces[i_].flags = world->surfaces[i_].flags;

					iw4_asset->dpvs.surfacesBounds[i_].bounds.compute(world->surfaces[i_].bounds[0], world->surfaces[i_].bounds[1]);
				}
			}

			iw4_asset->skyCount = 1;
			iw4_asset->skies = sky;

			if (world->skyImage)
			{
				sky->skyImage = assethandler::convert_asset_header(game::qos::ASSET_TYPE_IMAGE, { world->skyImage }).image;
			}

			sky->skySamplerState = world->skySamplerState; // 0xFf
			sky->skyStartSurfs = world->skyStartSurfs;
			sky->skySurfCount = world->skySurfCount;

			iw4_asset->lastSunPrimaryLightIndex = world->sunPrimaryLightIndex;
			iw4_asset->primaryLightCount = world->primaryLightCount;

			// dpvsPlanes
			auto dpvsPlanes = *world->planes;
			iw4_asset->dpvsPlanes.cellCount = dpvsPlanes.cellCount;
			memcpy(iw4_asset->dpvsPlanes.planes, dpvsPlanes.planes, 4);
			memcpy(iw4_asset->dpvsPlanes.nodes, dpvsPlanes.nodes, 4);
			memcpy(iw4_asset->dpvsPlanes.sceneEntCellBits, dpvsPlanes.sceneEntCellBits, 4);

			iw4_asset->dpvs.surfaceMaterials = utils::memory::allocate_array<game::iw4::GfxDrawSurf>(world->surfaceCount);
			for (auto i = 0; i < world->surfaceCount; i++)
			{
				iw4_asset->dpvs.surfaceMaterials[i].fields.objectId = world->surfaceMaterials[i].fields.objectId;
				iw4_asset->dpvs.surfaceMaterials[i].fields.reflectionProbeIndex = world->surfaceMaterials[i].fields.reflectionProbeIndex;
				iw4_asset->dpvs.surfaceMaterials[i].fields.customIndex = world->surfaceMaterials[i].fields.customIndex;
				iw4_asset->dpvs.surfaceMaterials[i].fields.materialSortedIndex = world->surfaceMaterials[i].fields.materialSortedIndex;
				iw4_asset->dpvs.surfaceMaterials[i].fields.prepass = world->surfaceMaterials[i].fields.prepass;
				iw4_asset->dpvs.surfaceMaterials[i].fields.sceneLightIndex = world->surfaceMaterials[i].fields.primaryLightIndex;
				iw4_asset->dpvs.surfaceMaterials[i].fields.surfType = world->surfaceMaterials[i].fields.surfType;
				iw4_asset->dpvs.surfaceMaterials[i].fields.primarySortKey = world->surfaceMaterials[i].fields.primarySortKey;
				iw4_asset->dpvs.surfaceMaterials[i].fields.unused = world->surfaceMaterials[i].fields.unused;
			}

			iw4_asset->dpvs.surfaceCastsSunShadow = world->surfaceCastsSunShadow;
			iw4_asset->dpvs.usageCount = 0; // ?? (world->usageCount)

			// dpvsDyn
			//iw4_asset->dpvsDyn.dynEntClientCount = world->dynEntClientCount;
			//iw4_asset->dpvsDyn.dynEntClientWordCount = world->dynEntClientWordCount;

			// iw4_credits has 0x3
			// Probably do not add Game::IW4::FogTypes::FOG_DFOG here, cod4 doesn't support it !
			iw4_asset->fogTypesAllowed = game::iw4::FOG_NORMAL;

			// AABBTree data is stored as part of the cells.
			// however, in IW4 it's not, so we have to extract the data
			if (world->cells)
			{
				iw4_asset->aabbTreeCounts = utils::memory::allocate_array<game::iw4::GfxCellTreeCount>(world->planes->cellCount);
				iw4_asset->aabbTrees = utils::memory::allocate_array<game::iw4::GfxCellTree>(world->planes->cellCount);
				iw4_asset->cells = utils::memory::allocate_array<game::iw4::GfxCell>(world->planes->cellCount);

				for (int i_ = 0; i_ < world->planes->cellCount; ++i_)
				{
					iw4_asset->aabbTreeCounts[i_].aabbTreeCount = 0; // cant find?

					iw4_asset->cells[i_].bounds.compute(world->cells[i_].mins, world->cells[i_].maxs); // Verified
					iw4_asset->cells[i_].portalCount = world->cells[i_].portalCount;
					iw4_asset->cells[i_].reflectionProbeCount = world->cells[i_].reflectionProbeCount;
					iw4_asset->cells[i_].reflectionProbes = world->cells[i_].reflectionProbes;

					if (world->cells[i_].aabbTree)
					{
						iw4_asset->aabbTrees[i_].aabbTree = utils::memory::allocate_array<game::iw4::GfxAabbTree>(1); // hardcoded on QoS
						std::memcpy(iw4_asset->aabbTrees[i_].aabbTree, world->cells[i_].aabbTree, sizeof(game::iw4::GfxAabbTree));

						//static_assert(sizeof game::iw4::GfxAabbTree == sizeof game::qos::GfxAabbTree, "Size mismatch");
						game::iw4::GfxAabbTree* iw4_cell = iw4_asset->aabbTrees[i_].aabbTree;
						game::qos::GfxAabbTree* qos_cell = world->cells[i_].aabbTree;

						iw4_cell->bounds.compute(qos_cell->mins, qos_cell->maxs); // Verified
					}

					if (world->cells[i_].portals)
					{
						iw4_asset->cells[i_].portals = utils::memory::allocate_array<game::iw4::GfxPortal>(world->cells[i_].portalCount);

						// Map all portals, so we have them ready for the next loop (might be unnecessary, as they are mapped at runtime)
						std::unordered_map<game::qos::GfxPortal*, game::iw4::GfxPortal*> portal_map = { { nullptr, nullptr } };

						for (int j = 0; j < world->cells[i_].portalCount; ++j)
						{
							portal_map[&world->cells[i_].portals[j]] = &iw4_asset->cells[i_].portals[j];
						}

						for (int j = 0; j < world->cells[i_].portalCount; ++j)
						{
							game::qos::GfxPortal* portal = &world->cells[i_].portals[j];
							game::iw4::GfxPortal* destPortal = &iw4_asset->cells[i_].portals[j];

							destPortal->cellIndex = static_cast<unsigned short>(portal->cell - world->cells);
							if (destPortal->cellIndex >= static_cast<unsigned short>(world->planes->cellCount))
							{
								console::error("Unable to calculate cell index. This should not happen!\n");
								destPortal->cellIndex = 0;
							}

							destPortal->vertices = portal->vertices;
							destPortal->vertexCount = portal->vertexCount;

							destPortal->writable.isQueued = portal->writable.isQueued;
							destPortal->writable.isAncestor = portal->writable.isAncestor;
							destPortal->writable.recursionDepth = portal->writable.recursionDepth;
							destPortal->writable.hullPointCount = portal->writable.hullPointCount;
							destPortal->writable.hullPoints = portal->writable.hullPoints;

							if (portal_map.find(portal->writable.queuedParent) != portal_map.end())
							{
								destPortal->writable.queuedParent = portal_map[portal->writable.queuedParent];
							}
							else
							{
								if (portal->writable.queuedParent) console::error("Unmapped portal. This shouldn't happen. Nulling it...\n");
								destPortal->writable.queuedParent = nullptr;
							}

							std::memcpy(destPortal->plane.coeffs, portal->plane.coeffs, sizeof(destPortal->plane.coeffs));
							std::memcpy(destPortal->hullAxis, portal->hullAxis, sizeof(destPortal->hullAxis));
						}
					}
				}
			}

			iw4_asset->draw.reflectionProbeCount = world->reflectionProbeCount;
			iw4_asset->draw.reflectionProbeTextures = utils::memory::allocate_array<game::iw4::GfxTexture>(iw4_asset->draw.reflectionProbeCount);
			for (size_t i = 0; i < iw4_asset->draw.reflectionProbeCount; i++)
			{
				/*
				if (world->reflectionProbeTextures[i].loadDef)
				{
					iw4_asset->draw.reflectionProbeTextures[i].loadDef = IGfxImage::ConvertLoadDef(world->reflectionProbeTextures[i].loadDef);
				}
				*/
			}

			iw4_asset->draw.lightmapCount = world->lightmapCount;

			return { iw4_asset };
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
