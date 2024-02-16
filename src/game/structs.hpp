#pragma once

namespace game::qos
{
	typedef float vec_t;
	typedef vec_t vec3_t[3];
	typedef vec_t vec2_t[2];
	typedef vec_t vec4_t[4];
	typedef vec_t vec5_t[5];

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
		ASSET_TYPE_CLIPMAP_SP = 11,
		ASSET_TYPE_CLIPMAP_MP = 12, // ASSET_TYPE_CLIPMAP_PVS
		ASSET_TYPE_COMWORLD = 13,
		ASSET_TYPE_gameWORLD_SP = 14,
		ASSET_TYPE_gameWORLD_MP = 15,
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
		ASSET_TYPE_AITYPE = 28,
		ASSET_TYPE_MPTYPE = 29,
		ASSET_TYPE_CHARACTER = 30,
		ASSET_TYPE_XMODELALIAS = 31,
		ASSET_TYPE_RAWFILE = 32,
		ASSET_TYPE_STRINGTABLE = 33,

		// these assets only exist in QoS
		ASSET_TYPE_XML_TREE = 34,
		ASSET_TYPE_SCENE_ANIM_RESOURCE = 35,
		ASSET_TYPE_CUTSCENE_RESOURCE = 36,
		ASSET_TYPE_CUSTOM_CAMERA_LIST = 37,

		// just guessing here but double check later
		ASSET_TYPE_COUNT = 38,
		ASSET_TYPE_STRING = 38,
		ASSET_TYPE_ASSETLIST = 39,
		ASSET_TYPE_INVALID = -1
	};

#pragma pack(push, 4)
	struct PhysPreset
	{
		const char* name; // 0
		int type; // 4
		float mass; // 8
		float bounce; // 12
		float friction; // 16
		float bulletForceScale; // 20
		float explosiveForceScale; // 24
		const char* sndAliasPrefix; // 28

		// something past here is incorrect
		float piecesSpreadFraction; // 32
		float piecesUpwardVelocity; // 36
		bool tempDefaultToCylinder; // 40
		bool perSurfaceSndAlias; // 44
	}; //static_assert(sizeof(PhysPreset) == 48)
#pragma pack(pop)

	struct RawFile
	{
		const char* name; // 0
		unsigned int len; // 4
		char* buffer; // 8
	}; static_assert(sizeof(RawFile) == 12);

	struct ComPrimaryLight
	{
		char type;
		char canUseShadowMap;
		char exponent;
		char unused;
		float color[3];
		float dir[3];
		float origin[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		float cosHalfFovExpanded;
		float rotationLimit;
		float translationLimit;
		const char* defName;
	};

	struct ComWorld
	{
		const char* name; // 0
		int isInUse; // 4
		unsigned int unk; // 8 (?)
		unsigned int primaryLightCount; // 12
		ComPrimaryLight* primaryLights; // 16
	}; static_assert(sizeof(ComWorld) == 20);

	struct gameWorldMp
	{
		const char* name; // 0
		//void* g_glassData; // 4
	}; static_assert(sizeof(gameWorldMp) == 4);

	struct GfxImage
	{
		// TODO
	};

	// slightly different than IW3
	struct GfxWorld
	{
		const char* name;			// 0
		const char* baseName;		// 4
		int planeCount;				// 8
		void* planes;				// 12 (pointer is next to count now)
		int nodeCount;				// 16
		void* nodes;				// 20 ^
		int indexCount;				// 24
		unsigned __int16* indices;	// 28 ^
		int surfaceCount;			// 32
		void* surfaces;				// 36 ^
		char __pad0[20];			// 40 (unresearched)
		int skySurfCount;			// 60
		int* skyStartSurfs;			// 64
		GfxImage* skyImage;			// 68
		char __pad0[4];				// 72 (unresearched)
		const char* unk;			// 76 (varXString used)

		// TODO
	}; static_assert(sizeof(GfxWorld) == 728); // 732 -> 728 (4 byte difference from COD4..)

	union XAssetHeader
	{
		void* data;
		//void* xmodelPieces;
		PhysPreset* physPreset; // not done
		//void* physConstraints;
		//void* destructibleDef;
		ComWorld* comWorld;
		//gameWorldSp* gameWorldSp;
		gameWorldMp* gameWorldMp;

		GfxWorld* gfxWorld;

		RawFile* rawfile;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct XAssetEntry
	{
		XAsset asset;
		char zoneIndex;
		char inuse;
		unsigned __int16 nextHash;
		unsigned __int16 nextOverride;
		unsigned __int16 usageFrame;
	};

	union XAssetEntryPoolEntry
	{
		XAssetEntry entry;
		XAssetEntryPoolEntry* next;
	};

	struct CmdArgs
	{
		int nesting;
		int localClientNum[8];
		int controllerIndex[8];
		int argc[8];
		const char** argv[8];
	};

	struct cmd_function_s
	{
		cmd_function_s* next;
		const char* name;
		const char* autoCompleteDir;
		const char* autoCompleteExt;
		void(__cdecl* function)();
	};

	struct scrMemTreePub_t
	{
		char* mt_buffer;
	};
}
