#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "component/assethandler.hpp"
#include "component/command.hpp"
#include "component/console.hpp"
#include "component/map_dumper.hpp"
#include "component/scheduler.hpp"

//#include "component/assets/gfximage.hpp";

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
			auto qos_planes = reinterpret_cast<game::iw4::GfxWorldDpvsPlanes*>(world->planes);
			iw4_asset->dpvsPlanes = *qos_planes;

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
				const auto load_def = world->reflectionProbes[i].reflectionImage->texture.loadDef;
				if (load_def != nullptr)
				{
					//iw4_asset->draw.reflectionProbeTextures[i].loadDef = gfximage::ConvertLoadDef(load_def);
				}
			}

			iw4_asset->draw.lightmapCount = world->lightmapCount;
			iw4_asset->draw.lightmaps = utils::memory::allocate_array<game::iw4::GfxLightmapArray>(iw4_asset->draw.lightmapCount);
			for (int i = 0; i < iw4_asset->draw.lightmapCount; i++)
			{
				auto iw4_lightmap = &iw4_asset->draw.lightmaps[i];
				auto qos_lightmap = &world->lightmaps[i];

				if (qos_lightmap->primary)
				{
					iw4_lightmap->primary = assethandler::convert_asset_header(game::qos::ASSET_TYPE_IMAGE, { qos_lightmap->primary }).image;
				}

				if (qos_lightmap->secondary)
				{
					iw4_lightmap->secondary = assethandler::convert_asset_header(game::qos::ASSET_TYPE_IMAGE, { qos_lightmap->secondary }).image;
				}
			}

			// can't find it in QoS atm, wtf?
			/*
			if (world->lightmapPrimaryTextures->data)
			{
				iw4_asset->draw.lightmapPrimaryTextures = IGfxImage::ConvertTexture(world->lightmapPrimaryTextures);
			}
			else
			{
				iw4_asset->draw.lightmapPrimaryTextures = LocalAllocator.Allocate<game::iw4::GfxTexture>();
			}
			*/
			iw4_asset->draw.lightmapPrimaryTextures = utils::memory::allocate<game::iw4::GfxTexture>();
			iw4_asset->draw.lightmapSecondaryTextures = utils::memory::allocate<game::iw4::GfxTexture>();

			if (world->outdoorImage)
			{
				iw4_asset->draw.outdoorImage = assethandler::convert_asset_header(game::qos::ASSET_TYPE_IMAGE, { world->outdoorImage }).image;
			}

			iw4_asset->draw.vertexCount = world->vertexCount;
			memcpy(&iw4_asset->draw.vd, &world->vd, sizeof(game::iw4::GfxWorldVertexData));
			iw4_asset->draw.vertexLayerDataSize = world->vertexLayerDataSize;
			memcpy(&iw4_asset->draw.vld, &world->vld, sizeof(game::iw4::GfxWorldVertexLayerData));
			iw4_asset->draw.indexCount = world->indexCount;
			iw4_asset->draw.indices = world->indices;

			// Split reflection images and probes
			if (world->reflectionProbes)
			{
				iw4_asset->draw.reflectionImages = utils::memory::allocate_array<game::iw4::GfxImage*>(world->reflectionProbeCount);
				iw4_asset->draw.reflectionProbes = utils::memory::allocate_array<game::iw4::GfxReflectionProbe>(world->reflectionProbeCount);
				iw4_asset->draw.reflectionProbeCount = world->reflectionProbeCount;

				for (unsigned int i = 0; i < world->reflectionProbeCount; ++i)
				{
					iw4_asset->draw.reflectionImages[i] = assethandler::convert_asset_header(game::qos::ASSET_TYPE_IMAGE, { world->reflectionProbes[i].reflectionImage }).image;

					if (world->reflectionProbes[i].reflectionImage)
					{
						assert(iw4_asset->draw.reflectionImages[i]);
					}

					std::memcpy(iw4_asset->draw.reflectionProbes[i].origin, world->reflectionProbes[i].origin, sizeof(iw4_asset->draw.reflectionProbes[i].origin));
				}
			}

			// lightGrid
			iw4_asset->lightGrid.hasLightRegions;
			memcpy(&iw4_asset->lightGrid + 4, &world->lightGrid, sizeof(game::qos::GfxLightGrid));

			iw4_asset->modelCount = world->modelCount;

			const auto should_add_carepackages = true;
			if (world->models)
			{
				// We're about to add two brushmodels here, which are identical : one for the airdrop package and one for the 4-streak care package
				iw4_asset->models = utils::memory::allocate_array<game::iw4::GfxBrushModel>(world->modelCount + (should_add_carepackages ? 2 : 0));

				for (int i = 0; i < world->modelCount; ++i)
				{
					iw4_asset->models[i].writable.bounds.compute(world->models[i].writable.mins, world->models[i].writable.maxs); // Irrelevant, runtime data
					iw4_asset->models[i].bounds.compute(world->models[i].bounds[0], world->models[i].bounds[1]); // Verified

					float* halfSize = iw4_asset->models[i].bounds.halfSize;
					iw4_asset->models[i].radius = static_cast<float>(std::sqrt(std::pow(halfSize[0], 2) + std::pow(halfSize[1], 2) + std::pow(halfSize[2], 2)));

					iw4_asset->models[i].surfaceCount = world->models[i].surfaceCount;
					iw4_asset->models[i].startSurfIndex = world->models[i].startSurfIndex;
					iw4_asset->models[i].surfaceCountNoDecal = world->models[i].surfaceCountNoDecal;
				}

				if (should_add_carepackages)
				{
					auto index = world->modelCount;

					// Create the care packages (TODO);
					game::iw4::GfxBrushModel care_package{};
					//game::iw4::Bounds packageBounds = IclipMap_t::MakeCarePackageBounds();

					//care_package.bounds = packageBounds;

					care_package.radius = 47.f;
					care_package.surfaceCount = 0;
					care_package.surfaceCountNoDecal = 0;
					care_package.startSurfIndex = std::numeric_limits<unsigned short>().max();

					// Airdrop package
					iw4_asset->models[index++] = care_package;

					// K4 care package
					iw4_asset->models[index++] = care_package;

					// Add 2 to modelcount because we just added two brushmodels
					iw4_asset->modelCount += 2;

					// and that should be it?
				}
			}

			iw4_asset->bounds.compute(world->mins, world->maxs);

			iw4_asset->checksum = world->checksum;
			iw4_asset->materialMemoryCount = world->materialMemoryCount;

			iw4_asset->materialMemory = utils::memory::allocate_array<game::iw4::MaterialMemory>(world->materialMemoryCount);
			for (int i = 0; i < world->materialMemoryCount; i++)
			{
				iw4_asset->materialMemory[i].memory = world->materialMemory[i].memory;
				iw4_asset->materialMemory[i].material = assethandler::convert_asset_header(game::qos::ASSET_TYPE_MATERIAL, { world->materialMemory[i].material }).material;
			}


			static_assert(sizeof iw4_asset->sun == sizeof world->sun);
			std::memcpy(&iw4_asset->sun, &world->sun, sizeof iw4_asset->sun);
			if (iw4_asset->sun.flareMaterial)
			{
				iw4_asset->sun.flareMaterial = assethandler::convert_asset_header(game::qos::ASSET_TYPE_MATERIAL, { world->sun.flareMaterial }).material;
			}

			if (iw4_asset->sun.spriteMaterial)
			{
				iw4_asset->sun.spriteMaterial = assethandler::convert_asset_header(game::qos::ASSET_TYPE_MATERIAL, { world->sun.spriteMaterial }).material;
			}

			std::memcpy(iw4_asset->outdoorLookupMatrix, world->outdoorLookupMatrix, sizeof(iw4_asset->outdoorLookupMatrix));
			iw4_asset->outdoorImage = assethandler::convert_asset_header(game::qos::ASSET_TYPE_IMAGE, { world->outdoorImage }).image;

			iw4_asset->cellCasterBits = world->cellCasterBits;
			iw4_asset->cellHasSunLitSurfsBits = reinterpret_cast<unsigned int*>(1); // This mustn't be null!

			iw4_asset->sceneDynModel = world->sceneDynModel;
			iw4_asset->sceneDynBrush = world->sceneDynBrush;

			iw4_asset->primaryLightEntityShadowVis = world->primaryLightEntityShadowVis;
			iw4_asset->primaryLightDynEntShadowVis[0] = world->primaryLightDynEntShadowVis[0];
			iw4_asset->primaryLightDynEntShadowVis[1] = world->primaryLightDynEntShadowVis[1];
			iw4_asset->nonSunPrimaryLightForModelDynEnt = world->nonSunPrimaryLightForModelDynEnt;

			iw4_asset->shadowGeom = world->shadowGeom;
			iw4_asset->lightRegion = world->lightRegion;

			iw4_asset->dpvs.smodelCount = world->smodelCount;
			iw4_asset->dpvs.staticSurfaceCount = 0; //world->staticSurfaceCount;
			iw4_asset->dpvs.staticSurfaceCountNoDecal = 0; //world->staticSurfaceCountNoDecal;

			// can't find these at all in QoS :/
			iw4_asset->dpvs.litOpaqueSurfsBegin = 0; //world->litSurfsBegin;
			iw4_asset->dpvs.litOpaqueSurfsEnd = 0; //world->decalSurfsEnd;

			// these don't exist in iw3/qos so skip
			iw4_asset->dpvs.litTransSurfsBegin = 0; //world->decalSurfsEnd;
			iw4_asset->dpvs.litTransSurfsEnd = 0; //world->decalSurfsEnd;

			// Skip those as well
			iw4_asset->dpvs.shadowCasterSurfsBegin = 0; //world->decalSurfsEnd;
			iw4_asset->dpvs.shadowCasterSurfsEnd = 0; //world->decalSurfsEnd;

			iw4_asset->dpvs.emissiveSurfsBegin = 0; //world->emissiveSurfsBegin;
			iw4_asset->dpvs.emissiveSurfsEnd = 0; //world->emissiveSurfsEnd;
			iw4_asset->dpvs.smodelVisDataCount = 0; //world->smodelVisDataCount;
			iw4_asset->dpvs.surfaceVisDataCount = 0; //world->surfaceVisDataCount;

			std::memcpy(iw4_asset->dpvs.smodelVisData, world->smodelVisData, sizeof(iw4_asset->dpvs.smodelVisData));	// 16 -> 12
			std::memcpy(iw4_asset->dpvs.surfaceVisData, world->surfaceVisData, sizeof(iw4_asset->dpvs.surfaceVisData)); // 16 -> 12

			iw4_asset->dpvs.sortedSurfIndex = world->shadowGeom->sortedSurfIndex;

			if (world->smodelInsts)
			{
				iw4_asset->dpvs.smodelInsts = utils::memory::allocate_array<game::iw4::GfxStaticModelInst>(world->smodelCount);

				for (unsigned int i = 0; i < world->smodelCount; ++i)
				{
					memcpy(&iw4_asset->dpvs.smodelInsts[i].bounds, &world->smodelInsts[i].bounds, sizeof(iw4_asset->dpvs.smodelInsts[i].bounds)); // Verified

					// this is CORRECT! Lighting origin is the place where the light grid gets sampled, and it must be the object's position!
					// This is how iw3 does it!
					// Check out 0x62EFF0 (iw3) 
					// and 0x524C80 (iw4)
					// (R_SetStaticModelLighting)
					iw4_asset->dpvs.smodelInsts[i].lightingOrigin[0] = iw4_asset->dpvs.smodelInsts[i].bounds.midPoint[0];
					iw4_asset->dpvs.smodelInsts[i].lightingOrigin[1] = iw4_asset->dpvs.smodelInsts[i].bounds.midPoint[1];
					iw4_asset->dpvs.smodelInsts[i].lightingOrigin[2] = iw4_asset->dpvs.smodelInsts[i].bounds.midPoint[2];
				}
			}

			if (world->surfaces)
			{
				iw4_asset->dpvs.surfaces = utils::memory::allocate_array<game::iw4::GfxSurface>(world->surfaceCount);
				iw4_asset->dpvs.surfacesBounds = utils::memory::allocate_array<game::iw4::GfxSurfaceBounds>(world->surfaceCount);

				for (int i = 0; i < world->surfaceCount; ++i)
				{
					iw4_asset->dpvs.surfaces[i].tris = world->surfaces[i].tris;
					iw4_asset->dpvs.surfaces[i].material = assethandler::convert_asset_header(game::qos::ASSET_TYPE_MATERIAL, {world->surfaces[i].material}).material;

					iw4_asset->dpvs.surfaces[i].lightmapIndex = world->surfaces[i].lightmapIndex;
					iw4_asset->dpvs.surfaces[i].reflectionProbeIndex = world->surfaces[i].reflectionProbeIndex;
					iw4_asset->dpvs.surfaces[i].primaryLightIndex = world->surfaces[i].primaryLightIndex;
					iw4_asset->dpvs.surfaces[i].flags = world->surfaces[i].flags;

					iw4_asset->dpvs.surfacesBounds[i].bounds.compute(world->surfaces[i].bounds[0], world->surfaces[i].bounds[1]); // Verified
				}
			}

			if (world->smodelDrawInsts)
			{
				iw4_asset->dpvs.smodelDrawInsts = utils::memory::allocate_array<game::iw4::GfxStaticModelDrawInst>(world->smodelCount);

				for (unsigned int i = 0; i < world->smodelCount; ++i)
				{
					std::memcpy(iw4_asset->dpvs.smodelDrawInsts[i].placement.origin, world->smodelDrawInsts[i].placement.origin, sizeof(iw4_asset->dpvs.smodelDrawInsts[i].placement.origin));
					std::memcpy(iw4_asset->dpvs.smodelDrawInsts[i].placement.axis, world->smodelDrawInsts[i].placement.axis, sizeof(iw4_asset->dpvs.smodelDrawInsts[i].placement.axis));

					iw4_asset->dpvs.smodelDrawInsts[i].cacheId[0] = 0;
					iw4_asset->dpvs.smodelDrawInsts[i].cacheId[1] = 0;
					iw4_asset->dpvs.smodelDrawInsts[i].cacheId[2] = 0;
					iw4_asset->dpvs.smodelDrawInsts[i].cacheId[3] = 0;

					iw4_asset->dpvs.smodelDrawInsts[i].placement.scale = world->smodelDrawInsts[i].placement.scale;
					iw4_asset->dpvs.smodelDrawInsts[i].model = assethandler::convert_asset_header(game::qos::ASSET_TYPE_XMODEL, { world->smodelDrawInsts[i].model }).model;


					float qos_cull_dist = world->smodelDrawInsts[i].cullDist;

#if EXTEND_CULLING
					// Double cull distance so it looks nicer in iw4
					qos_cull_dist *= 2;
#endif

					unsigned short iw4_cull_dist = 0;

					if (qos_cull_dist > std::numeric_limits<unsigned short>().max())
					{
						iw4_cull_dist = std::numeric_limits<unsigned short>().max();
					}
					else
					{
						iw4_cull_dist = static_cast<unsigned short>(qos_cull_dist);
					}

					iw4_asset->dpvs.smodelDrawInsts[i].cullDist = iw4_cull_dist;

					iw4_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = world->smodelDrawInsts[i].reflectionProbeIndex;
					iw4_asset->dpvs.smodelDrawInsts[i].primaryLightIndex = world->smodelDrawInsts[i].primaryLightIndex;
					iw4_asset->dpvs.smodelDrawInsts[i].lightingHandle = world->smodelDrawInsts[i].lightingHandle;
					iw4_asset->dpvs.smodelDrawInsts[i].flags = 0;

					if (world->smodelDrawInsts[i].flags & game::qos::STATIC_MODEL_FLAG_NO_SHADOW)
					{
						// Confirmed to be the same in the rendering functions
						// Check R_AddAllStaticModelSurfacesSpotShadow in both iw3 and iw4

						iw4_asset->dpvs.smodelDrawInsts[i].flags |= game::iw4::STATIC_MODEL_FLAG_NO_CAST_SHADOW;

						// aaaaand NO it's not !
						// For some reason while being used in the same place for the same thing AFAIK,
						// setting this to the "correct value" in iw4 results in blocky smodel shadows!
						// Unless we keep the iw3 flag in (which should be non existent in iw4!)
						iw4_asset->dpvs.smodelDrawInsts[i].flags |= game::qos::STATIC_MODEL_FLAG_NO_SHADOW;
					}

					if (world->smodelInsts)
					{
						iw4_asset->dpvs.smodelDrawInsts[i].groundLighting = world->smodelInsts[i].groundLighting;

						//// Grass needs 0x20 otherwise it doesn't read data from the lightmap and it's full bright !
						//// Whenever a model needs ground lighting in iw4, it has to specify it
						if (iw4_asset->dpvs.smodelDrawInsts[i].groundLighting.packed > 0)
						{
							iw4_asset->dpvs.smodelDrawInsts[i].flags |= game::iw4::STATIC_MODEL_FLAG_GROUND_LIGHTING;
						}
					}
				}
			}

#if USE_IW3_SORTKEYS
			// IW3 values
			iw4_asset->sortKeyLitDecal = 9;
			iw4_asset->sortKeyEffectDecal = 29;
			iw4_asset->sortKeyEffectAuto = 48;
			iw4_asset->sortKeyDistortion = 0;
#else
			iw4_asset->sortKeyLitDecal = 0x6;
			iw4_asset->sortKeyEffectDecal = 0x27;
			iw4_asset->sortKeyEffectAuto = 0x30;
			iw4_asset->sortKeyDistortion = 0x2b;
#endif

			int base_index = 0;
			iw4_asset->draw.indices = utils::memory::allocate_array<unsigned short>(iw4_asset->draw.indexCount);
			for (unsigned int i = 0; i < iw4_asset->surfaceCount; ++i)
			{
				std::memcpy(&iw4_asset->draw.indices[base_index], &world->indices[iw4_asset->dpvs.surfaces[i].tris.baseIndex], iw4_asset->dpvs.surfaces[i].tris.triCount * 6);
				iw4_asset->dpvs.surfaces[i].tris.baseIndex = base_index;
				base_index += iw4_asset->dpvs.surfaces[i].tris.triCount * 3;
			}

			if (base_index != iw4_asset->draw.indexCount)
			{
				console::warn("Warning: Didn't sort all indicies for worldDraw\n");
			}

			// Specify that it's a custom map
			iw4_asset->checksum = 0xC0D40001;

			/*
			auto smodelsFixMethod = Game::Dvar_FindVar("iw3x_smodels_fix_method");
			if (smodelsFixMethod)
			{
				auto method = std::stoi(smodelsFixMethod->current.string);
				IGfxWorld::RemoveIncompatibleModelsForIW4(&map, method);
			}
			*/

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
					if (!header.gfxWorld)
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
