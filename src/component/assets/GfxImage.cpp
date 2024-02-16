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
#include <utils/hook.hpp>

#define IW4X_IMG_VERSION "0"

static_assert(sizeof game::iw4::GfxImageFileHeader::tag == sizeof game::qos::GfxImageFileHeader::tag, "tag size mismatch!");
static_assert(sizeof game::iw4::GfxImageFileHeader::dimensions == sizeof game::qos::GfxImageFileHeader::dimensions, "dimensions size mismatch!");
static_assert(sizeof game::iw4::GfxImageFileHeader::fileSizeForPicmip == sizeof game::qos::GfxImageFileHeader::fileSizeForPicmip, "fileSizeForPicmip size mismatch!");

namespace gfximage
{
	namespace
	{
		static const std::unordered_set<std::string> imagesToRename =
		{
			"xpec_electrical_transformer_col", // Used between IW3 and IW4 but very different texture
			"xpec_electrical_transformer_nml",
			"refrigerator2_col", // Blue fridge in IW4, red in IW3
			"refrigerator2_nml",
			"refrigerator_col", // Different fridge texture
			"refrigerator_nml",
		};

		static std::unordered_map<std::string, std::string> imagesRenamed{};
		static std::unordered_map<std::string, std::string> imagesRenamedReverse{};

		const std::string imageSuffix = "_3x";

		int TranslateFlags(int iw3Flags)
		{
			int iw4Flags = 0;

			const auto translateFlags = [&](game::qos::file_image_flags_t sourceFlag, game::iw4::file_image_flags_t targetFlag)
				{
					if (iw3Flags & sourceFlag)
					{
						iw4Flags |= targetFlag;
					}
				};

			translateFlags(game::qos::IMG_FLAG_NOPICMIP, game::iw4::IMG_FLAG_NOPICMIP);
			translateFlags(game::qos::IMG_FLAG_NOMIPMAPS, game::iw4::IMG_FLAG_NOMIPMAPS);
			translateFlags(game::qos::IMG_FLAG_CUBEMAP, game::iw4::IMG_FLAG_MAPTYPE_CUBE);
			translateFlags(game::qos::IMG_FLAG_VOLMAP, game::iw4::IMG_FLAG_MAPTYPE_3D);
			translateFlags(game::qos::IMG_FLAG_STREAMING, game::iw4::IMG_FLAG_STREAMING);
			translateFlags(game::qos::IMG_FLAG_LEGACY_NORMALS, game::iw4::IMG_FLAG_LEGACY_NORMALS);
			translateFlags(game::qos::IMG_FLAG_CLAMP_U, game::iw4::IMG_FLAG_CLAMP_U);
			translateFlags(game::qos::IMG_FLAG_CLAMP_V, game::iw4::IMG_FLAG_CLAMP_V);

			// Not sure about these
			//translateFlags(game::qos::IMG_FLAG_DYNAMIC,        game::iw4::IMG_FLAG_DYNAMIC);
			//translateFlags(game::qos::IMG_FLAG_RENDER_TARGET,  game::iw4::IMG_FLAG_RENDER_TARGET);
			//translateFlags(game::qos::IMG_FLAG_SYSTEMMEM,      game::iw4::IMG_FLAG_SYSTEMMEM);

			//iw4Flags |= game::iw4::IMG_FLAG_ALPHA_WEIGHTED_COLORS;
			//iw4Flags |= game::iw4::IMG_FLAG_GAMMA_SRGB;

			return iw4Flags;
		}

		game::iw4::GfxImageLoadDef* ConvertLoadDef(game::qos::GfxImageLoadDef* qosLoadDef)
		{
			auto iw4Def = reinterpret_cast<game::iw4::GfxImageLoadDef*>(utils::memory::allocate(sizeof(game::iw4::GfxImageLoadDef) + qosLoadDef->resourceSize - 4));

			std::memcpy(iw4Def->data, qosLoadDef->data, qosLoadDef->resourceSize);
			iw4Def->flags = TranslateFlags(qosLoadDef->flags);
			iw4Def->format = qosLoadDef->format;
			iw4Def->levelCount = qosLoadDef->levelCount;
			iw4Def->resourceSize = qosLoadDef->resourceSize;

			return iw4Def;
		}

		game::iw4::GfxImage* convert(game::qos::GfxImage* image)
		{
			if (!image) return nullptr;

			if (image == nullptr || &image == NULL) return nullptr;

			std::string name = image->name;
			if (name[0] == '*') name.erase(name.begin());

			auto iw4Image = utils::memory::allocate<game::iw4::GfxImage>();
			
			iw4Image->texture.loadDef = ConvertLoadDef(image->texture.loadDef);

			iw4Image->mapType = static_cast<unsigned char>(image->mapType);
			iw4Image->semantic = static_cast<unsigned char>(image->semantic);
			iw4Image->category = image->category;
			iw4Image->picmip = image->picmip;
			iw4Image->noPicmip = image->noPicmip;
			iw4Image->track = image->track;
			iw4Image->cardMemory = image->cardMemory;
			iw4Image->width = image->width;
			iw4Image->height = image->height;
			iw4Image->depth = image->depth;
			iw4Image->delayLoadPixels = image->delayLoadPixels;
			iw4Image->name = image->name;

			return { iw4Image };
		}

		utils::hook::detour StoreTextureDetour;
		void StoreTextureHook()
		{
			game::qos::GfxImageLoadDef** loadDef = *reinterpret_cast<game::qos::GfxImageLoadDef***>(game::game_offset(0x10AB8F58));
			game::qos::GfxImage* image = *reinterpret_cast<game::qos::GfxImage**>(game::game_offset(0x10AB8E00));

			size_t size = 16 + (*loadDef)->resourceSize;
			void* data = utils::memory::get_allocator()->allocate(size);
			std::memcpy(data, *loadDef, size);

			image->texture.loadDef = reinterpret_cast<game::qos::GfxImageLoadDef*>(data);
		}

		utils::hook::detour ReleaseTextureDetour;
		void ReleaseTextureHook(game::qos::XAssetHeader header)
		{
			if (header.image && header.image->texture.loadDef)
			{
				utils::memory::get_allocator()->free(header.image->texture.loadDef);
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			scheduler::once([&]()
			{
				command::add("dumpGfxImage", [](const command::params& params)
				{
					if (params.size() < 2)
					{
						console::info("USAGE: dumpGfxImage <name>\n");
						return;
					}

					const auto name = params[1];

					auto header = game::DB_FindXAssetHeader(game::qos::ASSET_TYPE_IMAGE, name);
					if (!header.image)
					{
						console::error("dumpGfxImage failed on '%s'\n", name);
						return;
					}

					auto converted = gfximage::convert(header.image);
					map_dumper::api->write(game::iw4::ASSET_TYPE_IMAGE, converted);

					console::info("dumped '%s' for IW4\n", name);
				});
				//StoreTextureDetour.create(game::game_offset(0x103ADD20), StoreTextureHook);
				//ReleaseTextureDetour.create(game::game_offset(0x103ADD20), ReleaseTextureHook);
			}, scheduler::main);
		}

		game::qos::XAssetType get_type() override
		{
			return game::qos::ASSET_TYPE_IMAGE;
		}

		game::iw4::XAssetHeader convert_asset(game::qos::XAssetHeader header) override
		{
			return { convert(header.image) };
		}
	};
}

REGISTER_COMPONENT(gfximage::component)
