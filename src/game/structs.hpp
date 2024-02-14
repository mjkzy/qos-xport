#pragma once

#include <std_include.hpp>
#include <stdint.h>

namespace game
{
	typedef float vec_t;
	typedef vec_t vec3_t[3];
	typedef vec_t vec2_t[2];
	typedef vec_t vec4_t[4];
	typedef vec_t vec5_t[5];

	namespace qos
	{
		enum XAssetType
		{
			ASSET_TYPE_XMODELPIECES = 0x0,
			ASSET_TYPE_PHYSPRESET = 0x1,

			// these are assets from T4 & do not exist in IW4
			ASSET_TYPE_PHYSCONSTRAINTS = 0x2,
			ASSET_TYPE_DESTRUCTIBLE_DEF = 0x3,
			//

			ASSET_TYPE_XANIMPARTS = 0x4,
			ASSET_TYPE_XMODEL = 0x5,
			ASSET_TYPE_MATERIAL = 0x6,
			ASSET_TYPE_TECHNIQUE_SET = 0x7,
			ASSET_TYPE_IMAGE = 0x8,
			ASSET_TYPE_SOUND = 0x9,
			ASSET_TYPE_SOUND_CURVE = 0xA,
			//ASSET_TYPE_LOADED_SOUND = 11, // no loaded sounds?
			ASSET_TYPE_CLIPMAP = 11,
			ASSET_TYPE_CLIPMAP_PVS = 12,
			ASSET_TYPE_COMWORLD = 13,
			ASSET_TYPE_GAMEWORLD_SP = 14,
			ASSET_TYPE_GAMEWORLD_MP = 15,
			ASSET_TYPE_MAP_ENTS = 16,
			ASSET_TYPE_GFXWORLD = 17,
			ASSET_TYPE_LIGHT_DEF = 18,
			ASSET_TYPE_UI_MAP = 19, // i think?
			ASSET_TYPE_FONT = 20,
			ASSET_TYPE_MENULIST = 21,
			ASSET_TYPE_MENU = 22,
			ASSET_TYPE_LOCALIZE_ENTRY = 23,
			ASSET_TYPE_WEAPON = 24,
			ASSET_TYPE_SNDDRIVER_GLOBALS = 25, // i think?
			ASSET_TYPE_FX = 26,
			ASSET_TYPE_IMPACT_FX = 27,

			// luckily, this is the same as IW3 at the very least
			ASSET_TYPE_AITYPE = 28,
			ASSET_TYPE_MPTYPE = 29,
			ASSET_TYPE_CHARACTER = 30,
			ASSET_TYPE_XMODELALIAS = 31,
			//

			ASSET_TYPE_RAWFILE = 32,
			ASSET_TYPE_STRINGTABLE = 33,

			// QoS exclusive assets for Singleplayer
			ASSET_TYPE_XML_TREE = 34,
			ASSET_TYPE_SCENE_ANIM_RESOURCE = 35,
			ASSET_TYPE_CUTSCENE_RESOURCE = 36,
			ASSET_TYPE_CUSTOM_CAMERA_LIST = 37,

			// just guessing here but double check later
			ASSET_TYPE_COUNT = 38,
			ASSET_TYPE_STRING = 38,
			ASSET_TYPE_ASSETLIST = 39,
		};
	}
}
