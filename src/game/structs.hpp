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
		//ASSET_TYPE_PIXELSHADER = 7,			// pixelshader on Xbox, rest from here is + 1 :)
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
		ASSET_TYPE_SCENE_ANIMATION = 35,
		ASSET_TYPE_CUTSCENE_RESOURCE = 36,
		ASSET_TYPE_CUSTOM_CAMERA_LIST = 37,

		// just guessing here but double check later
		ASSET_TYPE_COUNT = 38,
		ASSET_TYPE_STRING = 38,
		ASSET_TYPE_ASSETLIST = 39,
		ASSET_TYPE_INVALID = -1
	};

	struct Bounds
	{
		vec3_t midPoint;
		vec3_t halfSize;
	};

#pragma region clipmap
#pragma pack(push, 4)
	struct cLeaf_t
	{
		unsigned __int16 firstCollAabbIndex;
		unsigned __int16 collAabbCount;
		int brushContents;
		int terrainContents;
		float mins[3];
		float maxs[3];
		int leafBrushNode;
		__int16 cluster;
	};
#pragma pack(pop)

	struct cmodel_t
	{
		float mins[3];
		float maxs[3];
		float radius;
		cLeaf_t leaf;
	};

	struct cplane_s;
	struct cbrushside_t;

	struct MapEnts
	{
		const char* name;
		char* entityString;
		int numEntityChars; // The structure actually ends here...
	}; static_assert(sizeof(MapEnts) == 0xC);

	struct clipMap_t
	{
		const char* name;
		int isInUse;
		int planeCount;
		cplane_s* planes;
		unsigned int numStaticModels;
		void* staticModelList;
		unsigned int numMaterials;
		void* materials;
		unsigned int numBrushSides;
		cbrushside_t* brushsides;
		char pad_0028[108]; //0x0028
		unsigned int numSubModels; //0x0094
		cmodel_t* cmodels; //0x0098
		char pad_009C[24]; //0x009C
		MapEnts* mapEnts; //0xB4
		char pad_00B8[140]; //0x00B8
	}; static_assert(sizeof(clipMap_t) == 0x144);
#pragma endregion

	struct GfxImageFileHeader
	{
		char tag[3];
		char version;
		char format;
		char flags;
		short dimensions[3];
		int fileSizeForPicmip[4];
	};

	struct GfxImageLoadDef
	{
		char levelCount;
		char flags;
		__int16 dimensions[3];
		int format;
		int resourceSize;
		char data[1];
	};

	union GfxTexture
	{
		GfxImageLoadDef* loadDef;
		void* data;
	};

	enum file_image_flags_t
	{
		IMG_FLAG_NOPICMIP = 0x1,
		IMG_FLAG_NOMIPMAPS = 0x2,
		IMG_FLAG_CUBEMAP = 0x4,
		IMG_FLAG_VOLMAP = 0x8,
		IMG_FLAG_STREAMING = 0x10,
		IMG_FLAG_LEGACY_NORMALS = 0x20,
		IMG_FLAG_CLAMP_U = 0x40,
		IMG_FLAG_CLAMP_V = 0x80,
		IMG_FLAG_DYNAMIC = 0x10000,
		IMG_FLAG_RENDER_TARGET = 0x20000,
		IMG_FLAG_SYSTEMMEM = 0x40000,
	};

	struct Picmip
	{
		char platform[2];
	};

	struct CardMemory
	{
		int platform[2];
	};

	struct GfxImage
	{
		char mapType;
		GfxTexture texture;
		Picmip picmip;
		bool noPicmip;
		char semantic;
		char track;
		CardMemory cardMemory;
		unsigned __int16 width;
		unsigned __int16 height;
		unsigned __int16 depth;
		char category;
		bool delayLoadPixels;
		const char* name;
	}; static_assert(sizeof(GfxImage) == 0x24);

	struct DObjAnimMat
	{
		float quat[4];
		float trans[3];
		float transWeight;
	};

	struct XSurfaceVertexInfo
	{
		short vertCount[4]; // 0
		unsigned short* vertsBlend; // 8
		float idk; // 12 (not sure what this is here)
	}; static_assert(sizeof(XSurfaceVertexInfo) == 16); // PLF confirms this

	union PackedTexCoords
	{
		unsigned int packed;
	};

	union GfxColor
	{
		unsigned int packed;
		unsigned char array[4];
	};

	union PackedUnitVec
	{
		unsigned int packed;
		char array[4];
	};

	struct GfxPackedVertex
	{
		float xyz[3];
		float binormalSign;
		GfxColor color;
		PackedTexCoords texCoord;
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	struct XSurfaceCollisionAabb
	{
		unsigned short mins[3];
		unsigned short maxs[3];
	};

	struct XSurfaceCollisionNode
	{
		XSurfaceCollisionAabb aabb;
		unsigned short childBeginIndex;
		unsigned short childCount;
	};

	struct XSurfaceCollisionLeaf
	{
		unsigned short triangleBeginIndex;
	};

	struct XSurfaceCollisionTree
	{
		float trans[3];
		float scale[3];
		unsigned int nodeCount;
		XSurfaceCollisionNode* nodes;
		unsigned int leafCount;
		XSurfaceCollisionLeaf* leafs;
	};

	struct XRigidVertList
	{
		unsigned short boneOffset; // 0
		unsigned short vertCount; // 2
		unsigned short triOffset; // 4
		unsigned short triCount; // 6
		XSurfaceCollisionTree* collisionTree; // 8
		void* unk; // 12 (not sure if its even here, but idk?)
	}; static_assert(sizeof(XRigidVertList) == 16);

	static_assert(sizeof(unsigned char) == 1);
	static_assert(sizeof(unsigned short) == 2);

	struct XSurface
	{
		unsigned char tileMode; // 0
		bool deformed; // 1
		unsigned short vertCount; // 2
		unsigned short triCount; // 4
		//char zoneHandle; // 6
		unsigned short baseTriIndex; // 6
		//unsigned short baseVertIndex; // 7
		unsigned short* triIndices; // 8

		XSurfaceVertexInfo vertInfo; // 12
		GfxPackedVertex* verts0; // 28
		unsigned int vertListCount; // 32
		XRigidVertList* vertList; // 36 (varGfxPackedVertex3)
		int partBits[4];	// 40
		char __pad0[24];	// 56

		/*
		int vertexBufferCount;	// 44 (guessed names)
		void* vertexBuffer;		// 48 ^
		void* indexBuffer;	// 52
		*/
	}; 
	static_assert(sizeof(XSurface) == 80); // 0x50
	static_assert(offsetof(XSurface, partBits) == 40);

	struct XModelCollTri_s
	{
		float plane[4];
		float svec[4];
		float tvec[4];
	};

	struct XModelCollSurf_s
	{
		XModelCollTri_s* collTris;
		int numCollTris;
		float mins[3];
		float maxs[3];
		int boneIdx;
		int contents;
		int surfFlags;
	};

	static_assert(sizeof(XModelCollSurf_s) == 44);

	struct XBoneInfo
	{
		float bounds[2][3];
		float offset[3];
		float radiusSquared;
	};

	static_assert(sizeof(XBoneInfo) == 40);

	struct PhysPreset;

	struct cplane_s
	{
		float normal[3];
		float dist;
		char type;
		char signbits;
		char pad[2];
	};

#pragma pack(push, 2)
	struct cbrushside_t
	{
		cplane_s* plane;
		unsigned int materialNum;
		__int16 firstAdjacentSideOffset;
		char edgeCount;
	};
#pragma pack(pop)

	struct BrushWrapper
	{
		float mins[3];
		int contents;
		float maxs[3];
		unsigned int numsides;
		cbrushside_t* sides;
		__int16 axialMaterialNum[2][3];
		char* baseAdjacentSide;
		__int16 firstAdjacentSideOffsets[2][3];
		char edgeCount[2][3];
		int totalEdgeCount;
		cplane_s* planes;
	};

	struct PhysMass
	{
		float centerOfMass[3];
		float momentsOfInertia[3];
		float productsOfInertia[3];
	};

	struct PhysGeomInfo
	{
		BrushWrapper* brush;
		int type;
		float orientation[3][3];
		float offset[3];
		float halfLengths[3];
	};

	struct PhysGeomList
	{
		unsigned int count;
		PhysGeomInfo* geoms;
		PhysMass mass;
	};

	struct XModelLodInfo
	{
		float dist;
		unsigned __int16 numsurfs;
		unsigned __int16 surfIndex;
		int partBits[4];
		char lod;
		char smcIndexPlusOne;
		char smcAllocBits;
		char unused;
		char crap[4];
	};

	struct Material;

	struct XModel
	{
		const char* name; // 0
		char numBones; // 4
		char numRootBones; // 5
		unsigned char numsurfs; //  6
		char lodRampType; // 7
		unsigned __int16* boneNames; // 8
		unsigned char* parentList; // 12
		__int16* quats; // 16
		float* trans; // 20
		unsigned char* partClassification; // 24
		DObjAnimMat* baseMat; // 28
		XSurface* surfs; // 32
		Material** materialHandles; // 36
		XModelLodInfo lodInfo[4]; // 40
		XModelCollSurf_s* collSurfs; // 168
		int numCollSurfs; // 172
		int contents; // 176
		XBoneInfo* boneInfo; // 180
		float radius; // 184

		// idk
		float mins[3]; // 188
		float maxs[3]; // 200
		short numLods; // 212
		short collLod; // 214
		void* streamInfo; // 216
		int memUsage; // 220
		char flags; // 224
		bool bad; // 225
		short __pad0; // 226
		PhysPreset* physPreset; // 228
		PhysGeomList* physGeoms; // 232
		void* physConstraints; // 236
	}; static_assert(sizeof(XModel) == 240);

	struct GfxDrawSurfFields
	{
		unsigned __int64 objectId : 16;
		unsigned __int64 reflectionProbeIndex : 8;
		unsigned __int64 customIndex : 5;
		unsigned __int64 materialSortedIndex : 11;
		unsigned __int64 prepass : 2;
		unsigned __int64 primaryLightIndex : 8;
		unsigned __int64 surfType : 4;
		unsigned __int64 primarySortKey : 6;
		unsigned __int64 unused : 4;
	};

	union GfxDrawSurf
	{
		GfxDrawSurfFields fields;
		unsigned __int64 packed;
	};

	struct MaterialArgumentCodeConst
	{
		unsigned __int16 index;
		char firstRow;
		char rowCount;
	};

	union MaterialArgumentDef
	{
		const float* literalConst;
		MaterialArgumentCodeConst codeConst;
		unsigned int codeSampler;
		unsigned int nameHash;
	};

	struct MaterialShaderArgument
	{
		unsigned __int16 type;
		unsigned __int16 dest;
		MaterialArgumentDef u;
	};

	struct MaterialPass
	{
		void* vertexDecl;
		void* vertexShader;
		void* pixelShader;
		char perPrimArgCount;
		char perObjArgCount;
		char stableArgCount;
		char customSamplerFlags;
		MaterialShaderArgument* args;
	};

	struct MaterialTechnique
	{
		const char* name;
		unsigned __int16 flags;
		unsigned __int16 numPasses;
		MaterialPass pass[1];
	};

	struct MaterialTechniqueSet
	{
		const char* name;
		char worldVertFormat;
		char unused[2];
		MaterialTechniqueSet* remappedTechniqueSet;
		MaterialTechnique* techniques[43];
	};

	struct MaterialTextureDef
	{
		unsigned int typeHash; // 0
		char firstCharacter; // 4
		char secondLastCharacter; // 5
		char sampleState; // 6
		char semantic; // 7
		GfxImage* image; // 8
	}; static_assert(sizeof(MaterialTextureDef) == 12);

	struct MaterialConstantDef
	{
		unsigned int nameHash;
		char name[12];
		vec4_t literal;
	};

	enum GfxSurfaceStatebitOp0 : unsigned int
	{
		GFXS0_SRCBLEND_RGB_SHIFT = 0x0,
		GFXS0_SRCBLEND_RGB_MASK = 0xF,
		GFXS0_DSTBLEND_RGB_SHIFT = 0x4,
		GFXS0_DSTBLEND_RGB_MASK = 0xF0,
		GFXS0_BLENDOP_RGB_SHIFT = 0x8,
		GFXS0_BLENDOP_RGB_MASK = 0x700,
		GFXS0_BLEND_RGB_MASK = 0x7FF,
		GFXS0_ATEST_DISABLE = 0x800,
		GFXS0_ATEST_GT_0 = 0x1000,
		GFXS0_ATEST_LT_128 = 0x2000,
		GFXS0_ATEST_GE_128 = 0x3000,
		GFXS0_ATEST_MASK = 0x3000,
		GFXS0_CULL_SHIFT = 0xE,
		GFXS0_CULL_NONE = 0x4000,
		GFXS0_CULL_BACK = 0x8000,
		GFXS0_CULL_FRONT = 0xC000,
		GFXS0_CULL_MASK = 0xC000,
		GFXS0_SRCBLEND_ALPHA_SHIFT = 0x10,
		GFXS0_SRCBLEND_ALPHA_MASK = 0xF0000,
		GFXS0_DSTBLEND_ALPHA_SHIFT = 0x14,
		GFXS0_DSTBLEND_ALPHA_MASK = 0xF00000,
		GFXS0_BLENDOP_ALPHA_SHIFT = 0x18,
		GFXS0_BLENDOP_ALPHA_MASK = 0x7000000,
		GFXS0_BLEND_ALPHA_MASK = 0x7FF0000,
		GFXS0_COLORWRITE_RGB = 0x8000000,
		GFXS0_COLORWRITE_ALPHA = 0x10000000,
		GFXS0_COLORWRITE_MASK = 0x18000000,
		GFXS0_POLYMODE_LINE = 0x80000000
	};

	enum GfxSurfaceStatebitOp1 : unsigned int
	{
		GFXS1_DEPTHWRITE = 0x1,
		GFXS1_DEPTHTEST_DISABLE = 0x2,
		GFXS1_DEPTHTEST_SHIFT = 0x2,
		GFXS1_DEPTHTEST_ALWAYS = 0x0,
		GFXS1_DEPTHTEST_LESS = 0x4,
		GFXS1_DEPTHTEST_EQUAL = 0x8,
		GFXS1_DEPTHTEST_LESSEQUAL = 0xC,
		GFXS1_DEPTHTEST_MASK = 0xC,
		GFXS1_POLYGON_OFFSET_SHIFT = 0x4,
		GFXS1_POLYGON_OFFSET_0 = 0x0,
		GFXS1_POLYGON_OFFSET_1 = 0x10,
		GFXS1_POLYGON_OFFSET_2 = 0x20,
		GFXS1_POLYGON_OFFSET_SHADOWMAP = 0x30,
		GFXS1_POLYGON_OFFSET_MASK = 0x30,
		GFXS1_STENCIL_FRONT_ENABLE = 0x40,
		GFXS1_STENCIL_BACK_ENABLE = 0x80,
		GFXS1_STENCIL_MASK = 0xC0,
		GFXS1_STENCIL_FRONT_PASS_SHIFT = 0x8,
		GFXS1_STENCIL_FRONT_FAIL_SHIFT = 0xB,
		GFXS1_STENCIL_FRONT_ZFAIL_SHIFT = 0xE,
		GFXS1_STENCIL_FRONT_FUNC_SHIFT = 0x11,
		GFXS1_STENCIL_FRONT_MASK = 0xFFF00,
		GFXS1_STENCIL_BACK_PASS_SHIFT = 0x14,
		GFXS1_STENCIL_BACK_FAIL_SHIFT = 0x17,
		GFXS1_STENCIL_BACK_ZFAIL_SHIFT = 0x1A,
		GFXS1_STENCIL_BACK_FUNC_SHIFT = 0x1D,
		GFXS1_STENCIL_BACK_MASK = 0xFFF00000,
		GFXS1_STENCILFUNC_FRONTBACK_MASK = 0xE00E0000,
		GFXS1_STENCILOP_FRONTBACK_MASK = 0x1FF1FF00,
	};

	struct GfxStatebitsFlags {
		GfxSurfaceStatebitOp0 loadbit0;
		GfxSurfaceStatebitOp1 loadbit1;
	};

	union GfxStateBits
	{
		GfxStatebitsFlags flags;
		int loadBits[2];
	};

	struct MaterialGameFlagsFields
	{
		unsigned char unk1 : 1; // 0x1
		unsigned char addShadowToPrimaryLight : 1; // 0x2
		unsigned char isFoliageRequiresGroundLighting : 1; // 0x4
		unsigned char unk4 : 1; // 0x8
		unsigned char unk5 : 1; // 0x10
		unsigned char unk6 : 1; // 0x20
		unsigned char MTL_GAMEFLAG_CASTS_SHADOW : 1; // 0x40
		unsigned char unkNeededForSModelDisplay : 1; // 0x80
	};

	union MaterialGameFlags
	{
		MaterialGameFlagsFields fields;
		unsigned char packed;
	};

	enum StateFlags : unsigned char 
	{
		STATE_FLAG_CULL_BACK = 0x1,
		STATE_FLAG_CULL_FRONT = 0x2,
		STATE_FLAG_DECAL = 0x4,
		STATE_FLAG_WRITES_DEPTH = 0x8,
		STATE_FLAG_USES_DEPTH_BUFFER = 0x10,
		STATE_FLAG_USES_STENCIL_BUFFER = 0x20,
		STATE_FLAG_CULL_BACK_SHADOW = 0x40,
		STATE_FLAG_CULL_FRONT_SHADOW = 0x80
	};

	struct Material
	{
		const char* name; // 0
		MaterialGameFlags gameFlags; // 4
		char sortKey; // 5
		char textureAtlasRowCount; // 6
		char textureAtlasColumnCount; // 7
		GfxDrawSurf drawSurf; // 8
		unsigned int surfaceTypeBits; // 16
		unsigned __int16 hashIndex; // 20
		char __pad0[8]; // 24 (rest of MaterialInfo?)
		char stateBitsEntry[35]; // 32
		char textureCount; // 67
		char constantCount; // 68
		char stateBitsCount; // 69
		StateFlags stateFlags; // 70
		char cameraRegion; // 71
		char __pad1[12]; // 72
		MaterialTechniqueSet* techniqueSet; // 84
		MaterialTextureDef* textureTable; // 88
		qos::MaterialConstantDef* constantTable; // 92
		qos::GfxStateBits* stateBitTable; // 96
	}; static_assert(sizeof(Material) == 104);

	struct FxSpawnDefLooping
	{
		int intervalMsec;
		int count;
	};

	struct FxIntRange
	{
		int base;
		int amplitude;
	};

	struct FxSpawnDefOneShot
	{
		FxIntRange count;
	};

	union FxSpawnDef
	{
		FxSpawnDefLooping looping;
		FxSpawnDefOneShot oneShot;
	};

	struct FxFloatRange
	{
		float base;
		float amplitude;
	};

	struct FxElemAtlas
	{
		char behavior;
		char index;
		char fps;
		char loopCount;
		char colIndexBits;
		char rowIndexBits;
		__int16 entryCount;
	};

	struct FxElemVec3Range
	{
		float base[3];
		float amplitude[3];
	};

	struct FxElemVelStateInFrame
	{
		FxElemVec3Range velocity;
		FxElemVec3Range totalDelta;
	};

	const struct FxElemVelStateSample
	{
		FxElemVelStateInFrame local;
		FxElemVelStateInFrame world;
	};

	struct FxElemVisualState
	{
		char color[4];
		float rotationDelta;
		float rotationTotal;
		float size[2];
		float scale;
	};

	const struct FxElemVisStateSample
	{
		FxElemVisualState base;
		FxElemVisualState amplitude;
	};

	struct FxEffectDef;

	union FxEffectDefRef
	{
		FxEffectDef* handle;
		const char* name;
	};

	union FxElemVisuals
	{
		const void* anonymous;
		Material* material;
		XModel* model;
		FxEffectDefRef effectDef;
		const char* soundName;
	};

	struct FxElemMarkVisuals
	{
		Material* materials[2];
	};

	union FxElemDefVisuals
	{
		FxElemMarkVisuals* markArray;
		FxElemVisuals* array;
		FxElemVisuals instance;
	};

	struct FxTrailVertex
	{
		float pos[2];
		float normal[2];
		float texCoord;
	};

	struct FxTrailDef
	{
		int scrollTimeMsec;
		int repeatDist;
		int splitDist;
		int vertCount;
		FxTrailVertex* verts;
		int indCount;
		unsigned __int16* inds;
	};

	const struct FxElemDef
	{
		int flags;
		FxSpawnDef spawn;
		FxFloatRange spawnRange;
		FxFloatRange fadeInRange;
		FxFloatRange fadeOutRange;
		float spawnFrustumCullRadius;
		FxIntRange spawnDelayMsec;
		FxIntRange lifeSpanMsec;
		FxFloatRange spawnOrigin[3];
		FxFloatRange spawnOffsetRadius;
		FxFloatRange spawnOffsetHeight;
		FxFloatRange spawnAngles[3];
		FxFloatRange angularVelocity[3];
		FxFloatRange initialRotation;
		FxFloatRange gravity;
		FxFloatRange reflectionFactor;
		FxElemAtlas atlas;
		char elemType;
		char visualCount;
		char velIntervalCount;
		char visStateIntervalCount;
		FxElemVelStateSample* velSamples;
		FxElemVisStateSample* visSamples;
		FxElemDefVisuals visuals;
		Bounds collBounds;
		FxEffectDefRef effectOnImpact;
		FxEffectDefRef effectOnDeath;
		FxEffectDefRef effectEmitted;
		FxFloatRange emitDist;
		FxFloatRange emitDistVariance;
		FxTrailDef* trailDef;
		char sortOrder;
		char lightingFrac;
		char useItemClip;
		char unused[1];
	};

	static_assert(sizeof(FxElemDef) == 0xFC);

	struct FxEffectDef
	{
		const char* name;
		int flags;
		int totalSize;
		int msecLoopingLife;
		int elemDefCountLooping;
		int elemDefCountOneShot;
		int elemDefCountEmission;
		FxElemDef* elemDefs;
	};

	static_assert(sizeof(FxEffectDef) == 0x20);

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
		char type;				// 0
		char canUseShadowMap;	// 1
		char exponent;			// 2
		char unused;			// 3
		float color[3];			// 4
		float dir[3];			// 16
		float origin[3];		// 28
		float radius;			// 40
		float cosHalfFovOuter;	// 44
		float cosHalfFovInner;	// 48
		float cosHalfFovExpanded; // 52
		float rotationLimit;	// 56
		float translationLimit;	// 60
		const char* defName;	// 64
	}; static_assert(sizeof(ComPrimaryLight) == 68);

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

	struct GfxWorldStreamInfo
	{
		int aabbTreeCount; // 0
		// 			GfxStreamingAabbTree *aabbTrees;
		// 			int leafRefCount;
		// 			int *leafRefs;
	};

	struct GfxWorldVertex
	{
		float xyz[3];
		float binormalSign;
		GfxColor color;
		float texCoord[2];
		float lmapCoord[2];
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	struct GfxWorldVertexData
	{
		GfxWorldVertex* vertices;
		void/*IDirect3DVertexBuffer9*/* worldVb;
	};

	struct GfxWorldVertexLayerData
	{
		char* data;
		void/*IDirect3DVertexBuffer9*/* layerVb;
	};

#pragma pack(push, 4)
	struct sunflare_t
	{
		char hasValidData; // 0
		Material* spriteMaterial; // 4
		Material* flareMaterial; // 8
		float spriteSize; // 12
		float flareMinSize;
		float flareMinDot;
		float flareMaxSize;
		float flareMaxDot;
		float flareMaxAlpha;
		int flareFadeInTime;
		int flareFadeOutTime;
		float blindMinDot;
		float blindMaxDot;
		float blindMaxDarken;
		int blindFadeInTime;
		int blindFadeOutTime;
		float glareMinDot;
		float glareMaxDot;
		float glareMaxLighten;
		int glareFadeInTime;
		int glareFadeOutTime;
		float sunFxPosition[3];
	};
#pragma pack(pop)

	struct __declspec(align(4)) GfxLightImage
	{
		GfxImage* image;
		char samplerState;
	};

	struct GfxLightDef
	{
		const char* name;
		GfxLightImage attenuation;
		int lmapLookupStart;
	};

	struct GfxLight
	{
		char type; // 0
		char canUseShadowMap; // 1
		char unused[2]; // 2
		float color[3]; // 4
		float dir[3]; // 16
		float origin[3]; // 28
		float radius; // 40
		float cosHalfFovOuter; // 44
		float cosHalfFovInner; // 48
		int exponent; // 52
		unsigned int spotShadowIndex; // 56
		int unk; // 60
		GfxLightDef* def; // 64
	}; static_assert(sizeof(GfxLight) == 68);

	struct GfxLightmapArray
	{
		GfxImage* primary; // 0
		GfxImage* secondary; // 4
	}; static_assert(sizeof(GfxLightmapArray) == 8);

	struct GfxLightGrid
	{
		//char hasLightRegions; // 0
		unsigned int sunPrimaryLightIndex; // 0
		unsigned __int16 mins[3]; // 4
		unsigned __int16 maxs[3]; // 10
		unsigned int rowAxis; // 16
		unsigned int colAxis; // 20
		unsigned __int16* rowDataStart; // 24
		unsigned int rawRowDataSize; // 28
		char* rawRowData; // 32
		unsigned int entryCount; // 36
		void* entries; // 40
		unsigned int colorCount; // 44
		void* colors; // 48
	}; static_assert(sizeof(GfxLightGrid) == 52); // PLF claims it's 56, but that could literally just be Wii version

	struct GfxBrushModelWritable
	{
		float mins[3];
		float maxs[3];
	};

	struct __declspec(align(4)) GfxBrushModel
	{
		GfxBrushModelWritable writable; // 0
		float bounds[2][3]; // 24
		unsigned __int16 surfaceCount; // 48
		unsigned __int16 startSurfIndex;
		unsigned __int16 surfaceCountNoDecal;
	};

	struct GfxReflectionProbe
	{
		float origin[3]; // 0
		GfxImage* reflectionImage; // 12
	}; static_assert(sizeof(GfxReflectionProbe) == 16);

	struct GfxPackedPlacement
	{
		float origin[3]; // 0
		vec3_t axis[3]; // 12
		float scale; // 48
	}; static_assert(sizeof(GfxPackedPlacement) == 52);

	static_assert(sizeof(__int16) == 2);

	struct __declspec(align(4)) GfxStaticModelDrawInst
	{
		float cullDist; // 0
		GfxPackedPlacement placement; // 4
		XModel* model;	// 56
		char __pad0[4];	// 60
		/*
		unsigned __int16 smodelCacheIndex[4]; // 60
		unsigned char reflectionProbeIndex; // 62
		unsigned char primaryLightIndex; // 63
		unsigned __int16 lightingHandle; // 64
		unsigned char flags; // 66
		*/
	}; static_assert(sizeof(GfxStaticModelDrawInst) == 64);

	struct MaterialMemory
	{
		Material* material;
		int memory;
	};

	struct GfxWorldDpvsPlanes
	{
		int cellCount; // 0
		cplane_s* planes; // 4
		unsigned __int16* nodes; // 8 
		unsigned int* sceneEntCellBits; // 12
	}; static_assert(sizeof(GfxWorldDpvsPlanes) == 16);

	struct GfxShadowGeometry
	{
		unsigned __int16 surfaceCount; // 0
		unsigned __int16 smodelCount; // 2
		unsigned __int16* sortedSurfIndex; // 4
		unsigned __int16* smodelIndex; // 8
	}; static_assert(sizeof(GfxShadowGeometry) == 12);

	struct GfxLightRegionAxis
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};

	struct GfxLightRegionHull
	{
		float kdopMidPoint[9]; // 0
		float kdopHalfSize[9]; // 36
		unsigned int axisCount; // 72
		GfxLightRegionAxis* axis; // 76
	}; static_assert(sizeof(GfxLightRegionHull) == 80);

	struct GfxLightRegion
	{
		unsigned int hullCount; // 0
		GfxLightRegionHull* hulls; // 4
	}; static_assert(sizeof(GfxLightRegion) == 8);

	struct XModelDrawInfo
	{
		unsigned __int16 lod;
		unsigned __int16 surfId;
	};

	struct GfxSceneDynModel
	{
		XModelDrawInfo info; // 0
		unsigned __int16 dynEntId; // 4
	}; static_assert(sizeof(GfxSceneDynModel) == 6);

	struct BModelDrawInfo
	{
		unsigned __int16 surfId;
	};

	struct GfxSceneDynBrush
	{
		BModelDrawInfo info;
		unsigned __int16 dynEntId;
	};

	struct srfTriangles_t
	{
		int vertexLayerData;
		int firstVertex;
		unsigned __int16 vertexCount;
		unsigned __int16 triCount;
		int baseIndex;
	};

	struct GfxSurface_
	{
		srfTriangles_t tris;
		Material* material;
		unsigned char lightmapIndex;
		unsigned char reflectionProbeIndex;
		unsigned char primaryLightIndex;
		unsigned char flags;
		float bounds[2][3];
	};

	struct GfxAabbTreeChildren;

	struct GfxAabbTree
	{
		float mins[3]; // 0
		float maxs[3]; // 12
		unsigned __int16 childCount; // 24
		unsigned __int16 surfaceCount; // 26
		unsigned __int16 startSurfIndex; // 28
		unsigned __int16 surfaceCountNoDecal; // 30
		unsigned __int16 startSurfIndexNoDecal; // 32
		unsigned __int16 smodelIndexCount; // 34
		unsigned __int16* smodelIndexes; // 36
		int childrenOffset; // 40 (aabbTreeChildrenCount)
		GfxAabbTreeChildren* gfxAabbTreeChildren; // 44
	}; static_assert(sizeof(GfxAabbTree) == 48);

	struct GfxAabbTreeChildren
	{
		GfxAabbTree child;
	};

	struct GfxPortal;

	struct GfxPortalWritable
	{
		char isQueued;
		char isAncestor;
		char recursionDepth;
		char hullPointCount;
		float(*hullPoints)[2];
		GfxPortal* queuedParent;
	};

	struct DpvsPlane
	{
		float coeffs[4];
		char side[3];
		char pad;
	};

	struct GfxCell;

	struct GfxPortal
	{
		GfxPortalWritable writable;
		DpvsPlane plane;
		GfxCell* cell;
		float(*vertices)[3];
		char vertexCount;
		float hullAxis[2][3];
	};

#pragma pack(push, 4)
	struct GfxCell
	{
		float mins[3]; // 0
		float maxs[3]; // 12
		GfxAabbTree* aabbTree; // 24 (just one)
		int portalCount; // 28
		GfxPortal* portals; // 32
		int cullGroupCount; // 36
		int* cullGroups; // 40
		unsigned char reflectionProbeCount; // 44
		unsigned char* reflectionProbes; // 48

		/*
		int aabbTreeCount; // 24
		GfxAabbTree* aabbTree; // 28
		int portalCount; // 32
		GfxPortal* portals; // 36
		int cullGroupCount; // 40
		int* cullGroups; // 44
		unsigned char reflectionProbeCount; // 48
		unsigned char* reflectionProbes; // 49
		*/
	}; static_assert(sizeof(GfxCell) == 52); // 56 in COD4
#pragma pack(pop)

	struct GfxSurface
	{
		// TODO
	}; 

#pragma pack(push, 4)
	struct GfxWorld // slightly different than IW3
	{
		const char* name;			// 0
		const char* baseName;		// 4
		int planeCount;				// 8
		GfxWorldDpvsPlanes* planes;	// 12 (pointer is next to count now)
		int nodeCount;				// 16
		void* nodes;				// 20 ^
		int indexCount;				// 24
		unsigned __int16* indices;	// 28 ^ (is this indices?? idk...)
		int surfaceCount;			// 32
		GfxSurface_* surfaces;		// 36 ^ (GfxWorldDpvsStatic?)
		char __pad0[20];			// 40 (unknown between here and 60)
		int skySurfCount;			// 60 (104 on Xbox?)
		int* skyStartSurfs;			// 64 (108 on Xbox?)
		GfxImage* skyImage;			// 68
		char skySamplerState;		// 72 (i think?)
		const char* skyDomeImageName;		// 76 (varXString used, name madeup)
		unsigned int vertexCount;			// 80
		GfxWorldVertexData vd;				// 84
		unsigned int vertexLayerDataSize;	// 92
		GfxWorldVertexLayerData vld;		// 96
		char __pad2[128];					// 104
		GfxLight* sunLight;					// 232
		float sunColorFromBsp[3];			// 236
		unsigned int sunPrimaryLightIndex;	// 248
		unsigned int primaryLightCount;		// 252
		int __pad3;							// 256
		int cullGroupCount;						// 260 (shows 0 debugging)
		unsigned char reflectionProbeCount;		// 264
		GfxReflectionProbe* reflectionProbes;	// 268
		void* cullGroups;							// 272 (from GfxWorldDpvsStatic?)
		int smodelCount;							// 276 ^ (cellCount?)
		GfxStaticModelDrawInst* smodelDrawInsts;	// 280 ^
		void* smodelInsts;							// 284 ?
		int cellBitsCount;			// 288
		int unk_2;					// 292 (some sort of count variable)
		GfxCell* cells;				// 296
		int lightmapCount;			// 300
		GfxLightmapArray* lightmaps;// 304
		GfxLightGrid lightGrid;		// 308
		int modelCount;				// 360
		GfxBrushModel* models;		// 364
		float mins[3];				// 368
		float maxs[3];				// 380
		unsigned int checksum;		// 392
		int materialMemoryCount;	// 396
		MaterialMemory* materialMemory;		// 400
		sunflare_t sun;						// 404
		float outdoorLookupMatrix[4][4];	// 500
		GfxImage* outdoorImage;				// 564

		// dpvsDyn?
		unsigned int dynEntClientCount[2];		// 568 (572)
		unsigned int dynEntClientWordCount[2];	// 576 (580)
		unsigned char* smodelVisData[4];		// 584 (588) (592) (596)
		unsigned char* surfaceVisData[4];		// 600 (604) (608) (612)

		GfxDrawSurf* surfaceMaterials;			// 616 (confirmed)
		unsigned int* surfaceCastsSunShadow;	// 620 ^
		unsigned int* cellCasterBits;			// 624
		unsigned int* cellHasSunLitSurfsBits;	// 628 (var_ubyte? ugh)
		char __pad6[48];					// 632
		void* gfxDynEntCellRef;				// 680 (?)
		void* gfxDynEntCellRef2;			// 684 ^
		GfxSceneDynModel* sceneDynModel;	// 688
		GfxSceneDynBrush* sceneDynBrush;	// 692
		short primaryLightForModelDynEnt;				// 696
		short idk;										// 698
		unsigned int* primaryLightEntityShadowVis;		// 700
		unsigned int* primaryLightDynEntShadowVis[2];	// 704 (708)
		GfxShadowGeometry* shadowGeom;	// 712
		int lightRegionCount;			// 716 (maybe, primaryLightCount used for it in IW4???)
		GfxLightRegion* lightRegion;	// 720 ^ idk what this is
		Material* material_idk;			// 724
	}; static_assert(sizeof(GfxWorld) == 728); // 732 -> 728 (4 byte difference from COD4 but structed very differently)
#pragma pack(pop)

	union XAssetHeader
	{
		void* data;
		//void* xmodelPieces;
		PhysPreset* physPreset; // not done
		//void* physConstraints;
		//void* destructibleDef;
		Material* material;
		GfxImage* image;
		XModel* xmodel;
		ComWorld* comWorld;
		//gameWorldSp* gameWorldSp;
		gameWorldMp* gameWorldMp;
		clipMap_t* clipMap;
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

	enum DvarType
	{
		DVAR_TYPE_BOOL = 0x0,
		DVAR_TYPE_FLOAT = 0x1,
		DVAR_TYPE_FLOAT_2 = 0x2,
		DVAR_TYPE_FLOAT_3 = 0x3,
		DVAR_TYPE_FLOAT_4 = 0x4,
		DVAR_TYPE_INT = 0x5,
		DVAR_TYPE_ENUM = 0x6,
		DVAR_TYPE_STRING = 0x7,
		DVAR_TYPE_COLOR = 0x8,
		DVAR_TYPE_COUNT = 0x9,
	};

	enum class dvar_type : std::int8_t
	{
		boolean = 0,
		value = 1,
		vec2 = 2,
		vec3 = 3,
		vec4 = 4,
		integer = 5,
		enumeration = 6,
		string = 7,
		color = 8,
		rgb = 9 // Color without alpha
	};

	struct DvarValueStringBuf
	{
		const char* pad;
		char string[12];
	};

	union DvarValue
	{
		bool enabled;
		int integer;
		unsigned int unsignedInt;
		float value;
		float vector[4];
		const char* string;
		DvarValueStringBuf stringBuf;
		char color[4];
	};

	union DvarLimits
	{
		struct enumeration
		{
			int stringCount;
			const char** strings;
		};
		struct integer
		{
			int min;
			int max;
		};
		struct value
		{
			float min;
			float max;
		};
		struct vector
		{
			float min;
			float max;
		};
	};

	enum dvar_flags : std::uint16_t
	{
		none = 0x0,
		saved = 0x1,
		user_info = 0x2, // sent to server on connect or change
		server_info = 0x4, // sent in response to front end requests
		replicated = 0x8,
		write_protected = 0x10,
		latched = 0x20,
		read_only = 0x40,
		cheat_protected = 0x80,
		temp = 0x100,
		no_restart = 0x400, // do not clear when a cvar_restart is issued
		user_created = 0x4000, // created by a set command
	};

	struct dvar_s
	{
		const char* name;
		const char* description;
		dvar_flags flags;
		dvar_type type;
		bool modified;
		DvarValue current;
		DvarValue latched;
		DvarValue reset;
		DvarLimits domain;
		dvar_s* next;
		dvar_s* hashNext;
	};

	typedef int scr_entref_t;

	typedef void(__stdcall* function_t)();

	struct scr_function_t
	{
		const char* name;
		function_t call;
		int developer;
	};

	typedef void(__stdcall* method_t)(scr_entref_t);

	struct scr_method_t
	{
		const char* name;
		method_t call;
		int developer;
	};

	union VariableUnion
	{
		int intValue;
		float floatValue;
		unsigned int stringValue;
		const float* vectorValue;
		const char* codePosValue;
		unsigned int pointerValue;
		struct VariableStackBuffer* stackValue;
		unsigned int entityOffset;
	};

	typedef struct
	{
		union VariableUnion u;
		int type;
	} VariableValue;

	struct function_stack_t
	{
		const char* pos;
		unsigned int localId;
		unsigned int localVarCount;
		VariableValue* top;
		VariableValue* startTop;
	};

	struct function_frame_t
	{
		function_stack_t fs;
		int topType;
	};

	struct scrVmPub_t
	{
		unsigned int* localVars;
		VariableValue* maxstack;
		int function_count;
		function_frame_t* function_frame;
		VariableValue* top;
		unsigned int inparamcount;
		unsigned int outparamcount;
		function_frame_t function_frame_start[32];
		VariableValue stack[2048];
	};

	enum VariableType
	{
		VAR_UNDEFINED = 0x0,
		VAR_BEGIN_REF = 0x1,
		VAR_POINTER = 0x1,
		VAR_STRING = 0x2,
		VAR_ISTRING = 0x3,
		VAR_VECTOR = 0x4,
		VAR_END_REF = 0x5,
		VAR_FLOAT = 0x5,
		VAR_INTEGER = 0x6,
		VAR_CODEPOS = 0x7,
		VAR_PRECODEPOS = 0x8,
		VAR_FUNCTION = 0x9,
		VAR_BUILTIN_FUNCTION = 0xA,
		VAR_BUILTIN_METHOD = 0xB,
		VAR_STACK = 0xC,
		VAR_ANIMATION = 0xD,
		VAR_DEVELOPER_CODEPOS = 0xE,
		VAR_INCLUDE_CODEPOS = 0xF,
		VAR_THREAD_LIST = 0x10,
		VAR_THREAD = 0x11,
		VAR_NOTIFY_THREAD = 0x12,
		VAR_TIME_THREAD = 0x13,
		VAR_CHILD_THREAD = 0x14,
		VAR_OBJECT = 0x15,
		VAR_DEAD_ENTITY = 0x16,
		VAR_ENTITY = 0x17,
		VAR_ARRAY = 0x18,
		VAR_DEAD_THREAD = 0x19,
		VAR_COUNT = 0x1A,
		VAR_FREE = 0x1A,
		VAR_ENDON_LIST = 0x1B,
	};
}
