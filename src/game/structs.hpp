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
	typedef int scr_entref_t;

	typedef int fileHandle_t;

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

	struct VariableStackBuffer
	{
		const char* pos;
		uint16_t size;
		uint16_t bufLen;
		uint16_t localId;
		char time;
		char buf[1];
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

	union ObjectInfo_u
	{
		uint16_t size;
		uint16_t entnum;
		uint16_t nextEntId;
		uint16_t self;
	};

	struct ObjectInfo
	{
		uint16_t refCount;
		union ObjectInfo_u u;
	};

	union VariableValueInternal_u
	{
		uint16_t next;
		union VariableUnion u;
		struct ObjectInfo o;
	};

	union VariableValueInternal_w
	{
		unsigned int status;
		unsigned int type;
		unsigned int name;
		unsigned int classnum;
		unsigned int notifyName;
		unsigned int waitTime;
		unsigned int parentLocalId;
	};

	union VariableValueInternal_v
	{
		uint16_t next;
		uint16_t index;
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

	union Variable_u
	{
		uint16_t prev;
		uint16_t prevSibling;
	};

	struct Variable
	{
		uint16_t id;
		union Variable_u u;
	};

	struct VariableValueInternal
	{
		struct Variable hash;
		union VariableValueInternal_u u;
		union VariableValueInternal_w w;
		union VariableValueInternal_v v;
		uint16_t nextSibling;
	};


	struct function_frame_t
	{
		struct function_stack_t fs;
		int topType;
	};

	struct scrVmPub_t
	{
		unsigned int* localVars;
		VariableValue* maxstack;
		int function_count;
		struct function_frame_t* function_frame;
		VariableValue* top;
		byte debugCode;
		byte abort_on_error;
		byte terminal_error;
		byte pad;
		unsigned int inparamcount;
		unsigned int outparamcount;
		struct function_frame_t function_frame_start[32];
		VariableValue stack[2048];
	};

	enum scrParamType_t
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
		VAR_PRE_ANIMATION = 0xF,
		VAR_THREAD = 0x10,
		VAR_NOTIFY_THREAD = 0x11,
		VAR_TIME_THREAD = 0x12,
		VAR_CHILD_THREAD = 0x13,
		VAR_OBJECT = 0x14,
		VAR_DEAD_ENTITY = 0x15,
		VAR_ENTITY = 0x16,
		VAR_ARRAY = 0x17,
		VAR_DEAD_THREAD = 0x18,
		VAR_COUNT = 0x19,
		VAR_THREAD_LIST = 0x1A,
		VAR_ENDON_LIST = 0x1B,
	};

	struct scrVarPub_t
	{
		const char* fieldBuffer;
		struct HunkUser* programHunkUser;
		uint16_t canonicalStrCount;
		byte developer;
		byte developer_script;
		byte evaluate;
		byte pad[3];
		const char* error_message;
		int error_index;
		unsigned int time;
		unsigned int timeArrayId;
		unsigned int pauseArrayId;
		unsigned int levelId;
		unsigned int gameId;
		unsigned int animId;
		unsigned int freeEntList;
		unsigned int tempVariable;
		byte bInited;
		byte pad2;
		uint16_t savecount;
		unsigned int checksum;
		unsigned int entId;
		unsigned int entFieldName;
		const char* programBuffer;
		const char* endScriptBuffer;
	};

	typedef enum button_mask : unsigned int
	{
		KEY_FIRE = 1 << 0,
		KEY_UNK = 1 << 1,
		KEY_MELEE = 1 << 2,
		KEY_USE = 1 << 3,
		KEY_RELOAD = 1 << 4,
		KEY_USERELOAD = 1 << 5,
		KEY_LEANLEFT = 1 << 6,
		KEY_LEANRIGHT = 1 << 7,
		KEY_PRONE = 1 << 8,
		KEY_CROUCH = 1 << 9,
		KEY_GOSTAND = 1 << 10,
		KEY_UNK2 = 1 << 11,
		KEY_ADSMODE = 1 << 12,
		KEY_UNK3 = 1 << 13,
		KEY_UNK4 = 1 << 14,
		KEY_HOLDBREATH = 1 << 15,
		KEY_FRAG = 1 << 16,
		KEY_SMOKE = 1 << 17,
		KEY_UNK5 = 1 << 18,
		KEY_ADS = 1 << 19,
		KEY_UNK7 = 1 << 20,
		KEY_UNK8 = 1 << 21,
		KEY_MENU = 1 << 22,
		KEY_UNK10 = 1 << 23,
		KEY_UNK11 = 1 << 24,
		KEY_UNK12 = 1 << 25,
		KEY_UNK13 = 1 << 25,
		KEY_UNK14 = 1 << 26,
		KEY_UNK15 = 1 << 27,
		KEY_UNK16 = 1 << 28,
		KEY_UNK17 = 1 << 29,
		KEY_UNK9 = 1 << 30,
		KEY_UNK6 = 2147483648
	} button_mask;

	struct botaction_t
	{
		const char* action;
		unsigned int key;
	};

	struct bot_movements
	{
		unsigned int buttons;
		int ping;
		char weapon;
		char forward;
		char right;
	};

	typedef enum
	{
		CS_FREE = 0x0,
		CS_ZOMBIE = 0x1,
		CS_CONNECTED = 0x2,
		CS_PRIMED = 0x3,
		CS_ACTIVE = 0x4,
	} clientState_t;

	struct reliableCommands_t
	{
		char command[1024];
		int cmdTime;
		int cmdType;
	};

	typedef struct usercmd_s
	{
		int serverTime;
		int buttons;
		byte weapon;
		byte offHandIndex;
		int angles[3];
		char forwardmove;
		char rightmove;
	} usercmd_t;

	typedef enum
	{
		TR_STATIONARY = 0x0,
		TR_INTERPOLATE = 0x1,
		TR_LINEAR = 0x2,
		TR_LINEAR_STOP = 0x3,
		TR_SINE = 0x4,
		TR_GRAVITY = 0x5,
		TR_GRAVITY_PAUSED = 0x6,
		TR_ACCELERATE = 0x7,
		TR_DECCELERATE = 0x8,
	} trType_t;

	typedef struct
	{
		trType_t trType;
		int trTime;
		int trDuration;
		vec3_t trBase;
		vec3_t trDelta;
	} trajectory_t;

	typedef struct entityState_s
	{
		int number;
		int eType;
		int eFlags;
		trajectory_t pos;
		trajectory_t apos;
		int time;
		int time2;
		vec3_t origin;
		vec3_t angles;
		int otherEntityNum;
		int attackerEntityNum;
		int groundEntityNum;
		int constantLight;
		int loopSound;
		int surfaceFlags;
		int modelindex;
		int clientNum;
		int iHeadIcon;
		int iHeadIconTeam;
		int solid;
		int eventParm;
		int eventSequence;
		vec4_t events;
		vec4_t eventParms;
		int weapon;
		int legsAnim;
		int torsoAnim;
		int stage;
		int loopfxid;
		int hintstring;
		int animMovetype;
		vec3_t unkAngles;
	} entityState_t;

	typedef struct
	{
		byte linked;
		byte bmodel;
		byte svFlags;
		byte pad1;
		int clientMask[2];
		byte inuse;
		byte pad2[3];
		int broadcastTime;
		vec3_t mins;
		vec3_t maxs;
		int contents;
		vec3_t absmin;
		vec3_t absmax;
		vec3_t currentOrigin;
		vec3_t currentAngles;
		unsigned __int16 ownerNum;
		unsigned __int16 pad3;
		int eventTime;
	} entityShared_t;

	struct turretInfo_s
	{
		int inuse;
		int flags;
		int fireTime;
		vec2_t arcmin;
		vec2_t arcmax;
		float dropPitch;
		int stance;
		int prevStance;
		int fireSndDelay;
		vec3_t userOrigin;
		float playerSpread;
		int triggerDown;
		char fireSnd;
		char fireSndPlayer;
		char stopSnd;
		char stopSndPlayer;
	};

	typedef struct gentity_s
	{
		entityState_t s;
		entityShared_t r;
		struct gclient_s* client;
		turretInfo_s* pTurretInfo;
		byte physicsObject;
		byte takedamage;
		byte active;
		byte nopickup;
		byte model;
		byte dobjbits;
		byte handler;
		byte team;
		unsigned __int16 classname;
		unsigned __int16 target;
		unsigned __int16 targetname;
		unsigned __int16 padding;
		int spawnflags;
		int flags;
		int eventTime;
		int freeAfterEvent;
		int unlinkAfterEvent;
		int clipmask;
		int framenum;
		struct gentity_s* parent;
		int nextthink;
		int healthPoints;
		int reservedHealth;
		int damage;
		int splashDamage;
		int splashRadius;
		float pfDecelTimeMove;
		float pfDecelTimeRotate;
		float pfSpeedMove;
		float pfSpeedRotate;
		float pfMidTimeMove;
		float pfMidTimeRotate;
		vec3_t vPos1Move;
		vec3_t vPos2Move;
		vec3_t vPos3Move;
		vec3_t vPos1Rotate;
		vec3_t vPos2Rotate;
		vec3_t vPos3Rotate;
		int moverState;
		struct gentity_s** linkedEntities;
		byte attachedModels[6];
		unsigned __int16 attachedModelsIndexes;
		unsigned __int16 numAttachedModels;
		int animTree;
		vec4_t color;
	} gentity_t;

	typedef struct
	{
		char r;
		char g;
		char b;
		char a;
	} hudelem_colorsplit_t;

	typedef union
	{
		hudelem_colorsplit_t split;
		int rgba;
	} hudelem_color_t;

	enum he_type_t
	{
		HE_TYPE_FREE = 0x0,
		HE_TYPE_TEXT = 0x1,
		HE_TYPE_VALUE = 0x2,
		HE_TYPE_PLAYERNAME = 0x3,
		HE_TYPE_MAPNAME = 0x4,
		HE_TYPE_GAMETYPE = 0x5,
		HE_TYPE_MATERIAL = 0x6,
		HE_TYPE_TIMER_DOWN = 0x7,
		HE_TYPE_TIMER_UP = 0x8,
		HE_TYPE_TENTHS_TIMER_DOWN = 0x9,
		HE_TYPE_TENTHS_TIMER_UP = 0xA,
		HE_TYPE_CLOCK_DOWN = 0xB,
		HE_TYPE_CLOCK_UP = 0xC,
		HE_TYPE_WAYPOINT = 0xD,
		HE_TYPE_COUNT = 0xE,
	};

	typedef struct hudelem_s
	{
		he_type_t type;
		float x;
		float y;
		float z;
		float fontScale;
		int font;
		int alignOrg;
		int alignScreen;
		hudelem_color_t color;
		hudelem_color_t fromColor;
		int fadeStartTime;
		int fadeTime;
		int label;
		int width;
		int height;
		int materialIndex;
		int fromWidth;
		int fromHeight;
		int scaleStartTime;
		int scaleTime;
		float fromX;
		float fromY;
		int fromAlignOrg;
		int fromAlignScreen;
		int moveStartTime;
		int moveTime;
		int time;
		int duration;
		float value;
		int text;
		float sort;
		hudelem_color_t glowColor;
	} hudelem_t;

	typedef struct hudElemState_s
	{
		hudelem_t current[31];
		hudelem_t archival[31];
	} hudElemState_t;

	enum objectiveState_t
	{
		OBJST_EMPTY = 0x0,
		OBJST_ACTIVE = 0x1,
		OBJST_INVISIBLE = 0x2,
		OBJST_DONE = 0x3,
		OBJST_CURRENT = 0x4,
		OBJST_FAILED = 0x5,
		OBJST_NUMSTATES = 0x6,
	};

	typedef struct objective_s
	{
		objectiveState_t state;
		vec3_t origin;
		int entNum;
		int teamNum;
		int icon;
	} objective_t;

	typedef struct
	{
		float yaw;
		int timer;
		int transIndex;
		int flags;
	} mantleState_t;

	enum ViewLockTypes_t
	{
		PLAYERVIEWLOCK_NONE = 0x0,
		PLAYERVIEWLOCK_FULL = 0x1,
		PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
		PLAYERVIEWLOCKCOUNT = 0x3,
	};

	typedef struct playerState_s
	{
		int commandTime;
		int pm_type;
		int bobCycle;
		int pm_flags;
		int pm_time;
		vec3_t origin;
		vec3_t velocity;
		vec2_t oldVelocity;
		int weaponTime;
		int weaponDelay;
		int grenadeTimeLeft;
		int throwBackGrenadeOwner;
		int throwBackGrenadeTimeLeft;
		int gravity;
		float leanf;
		int speed;
		vec3_t delta_angles;
		int groundEntityNum;
		vec3_t vLadderVec;
		int jumpTime;
		float jumpOriginZ;
		int legsTime;
		int legsAnim;
		int torsoTime;
		int torsoAnim;
		int legsAnimDuration;
		int torsoAnimDuration;
		int damageTimer;
		int damageDuration;
		int flinchYawAnim;
		int movementDir;
		int eFlags;
		int eventSequence;
		int events[4];
		unsigned int eventParms[4];
		int oldEventSequence;
		int clientNum;
		int offHandIndex;
		unsigned int weapon;
		int weaponstate;
		float fWeaponPosFrac;
		int adsDelayTime;
		int viewmodelIndex;
		vec3_t viewangles;
		int viewHeightTarget;
		float viewHeightCurrent;
		int viewHeightLerpTime;
		int viewHeightLerpTarget;
		int viewHeightLerpDown;
		int unknown[5];
		int damageEvent;
		int damageYaw;
		int damagePitch;
		int damageCount;
		int stats[6];
		int ammo[128];
		int ammoclip[128];
		int weapFlags;
		int weapFlags2;
		int unknown2[2];
		byte slot_none;
		byte slot_primary;
		byte slot_primaryb;
		int unknown3[5];
		vec3_t mins;
		vec3_t maxs;
		float proneDirection;
		float proneDirectionPitch;
		float proneTorsoPitch;
		ViewLockTypes_t viewlocked;
		int viewlocked_entNum;
		int cursorHint;
		int cursorHintString;
		int cursorHintEntIndex;
		int unknown1;
		vec3_t unkAngles;
		float holdBreathScale;
		int holdBreathTimer;
		mantleState_t mantleState;
		int entityEventSequence;
		int weapAnim;
		float aimSpreadScale;
		int shellshockIndex;
		int shellshockTime;
		int shellshockDuration;
		objective_t objective[16];
		int archiveTime;
		hudElemState_t hud;
	} playerState_t;

	typedef struct
	{
		playerState_t ps;
		int num_entities;
		int num_clients;
		int first_entity;
		int first_client;
		unsigned int messageSent;
		unsigned int messageAcked;
		int messageSize;
	} clientSnapshot_t;

#pragma pack(push, 1)
	typedef struct
	{
		char num;
		char data[256];
		int dataLen;
	} voices_t;
#pragma pack(pop)

	typedef void netProfileInfo_t;

	enum netadrtype_t
	{
		NA_BOT = 0x0,
		NA_BAD = 0x0,
		NA_LOOPBACK = 0x2,
		NA_BROADCAST = 0x3,
		NA_IP = 0x4,
		NA_IPX = 0x5,
		NA_BROADCAST_IPX = 0x6,
	};

	struct netadr_t
	{
		netadrtype_t type;
		byte ip[4];
		unsigned __int16 port;
		byte ipx[10];
	};

	enum netsrc_t
	{
		NS_CLIENT = 0x0,
		NS_SERVER = 0x1,
	};

	typedef struct
	{
		int outgoingSequence;
		netsrc_t sock;
		int dropped;
		int incomingSequence;
		netadr_t remoteAddress;
		int qport;
		int fragmentSequence;
		int fragmentLength;
		byte fragmentBuffer[131072];
		int unsentFragments;
		int unsentFragmentStart;
		int unsentLength;
		byte unsentBuffer[131072];
		netProfileInfo_t* netProfile;
	} netchan_t;

	typedef struct client_s
	{
		clientState_t state;
		int unksnapshotvar;
		int unksnapshotvar2;
		char userinfo[1024];
		reliableCommands_t reliableCommands[128];
		int reliableSequence;
		int reliableAcknowledge;
		int reliableSent;
		int messageAcknowledge;
		int gamestateMessageNum;
		int challenge;
		usercmd_t lastUsercmd;
		int lastClientCommand;
		char lastClientCommandString[1024];
		gentity_t* gentity;
		char name[32];
		char downloadName[64];
		fileHandle_t download;
		int downloadSize;
		int downloadCount;
		int downloadClientBlock;
		int downloadCurrentBlock;
		int downloadXmitBlock;
		unsigned __int8* downloadBlocks[8];
		int downloadBlockSize[8];
		int downloadEOF;
		int downloadSendTime;
		char wwwDownloadURL[256];
		int wwwDownload;
		int wwwDownloadStarted;
		int wwwDlAck;
		int wwwDl_failed;
		int deltaMessage;
		int floodprotect;
		int lastPacketTime;
		int lastConnectTime;
		int nextSnapshotTime;
		int rateDelayed;
		int timeoutCount;
		clientSnapshot_t frames[32];
		int ping;
		int rate;
		int snapshotMsec;
		int pureAuthentic;
		netchan_t netchan;
		int guid;
		__int16 clscriptid;
		int bot;
		int serverId;
		voices_t voicedata[40];
		int unsentVoiceData;
		byte mutedClients[64];
		byte hasVoip;
		char pbguid[64];
	} client_t;

	typedef struct archivedSnapshot_s
	{
		int start;
		int size;
	} archivedSnapshot_t;

	typedef struct cachedClient_s
	{
		int playerStateExists;
		clientState_t* cs;
		playerState_t* ps;
	} cachedClient_t;

	typedef struct
	{
		netadr_t adr;
		int challenge;
		int time;
		int pingTime;
		int firstTime;
		int firstPing;
		int connected;
		int guid;
		char pbguid[64];
		int ipAuthorize;
	} challenge_t;

	typedef struct
	{
		int initialized;
		int time;
		int snapFlagServerBit;
		client_t* clients;
		int numSnapshotEntities;
		int numSnapshotClients;
		int nextSnapshotEntities;
		int nextSnapshotClients;
		entityState_t* snapshotEntities;
		clientState_t* snapshotClients;
		int archivedSnapshotEnabled;
		int nextArchivedSnapshotFrames;
		archivedSnapshot_t* archivedSnapshotFrames;
		int* archivedSnapshotBuffer;
		int nextArchivedSnapshotBuffer;
		int nextCachedSnapshotEntities;
		int nextCachedSnapshotClients;
		int nextCachedSnapshotFrames;
		cachedClient_t cachedSnapshotClients;
		int nextHeartbeatTime;
		int nextStatusResponseTime;
		challenge_t challenges[1024];
		netadr_t redirectAddress;
		netadr_t authorizeAddress;
		char netProfilingBuf[1504];
	} serverStatic_t;

	// namespace for 007 QoS, hybrid of IW3 & T4
	namespace qos
	{
		enum XAssetType
		{
			ASSET_TYPE_XMODELPIECES = 0x0,
			ASSET_TYPE_PHYSPRESET = 0x1,

			// same as T4/T5
			ASSET_TYPE_PHYSCONSTRAINTS = 0x2,
			ASSET_TYPE_DESTRUCTIBLE_DEF = 0x3,

			ASSET_TYPE_XANIMPARTS = 0x4,
			ASSET_TYPE_XMODEL = 0x5,
			ASSET_TYPE_MATERIAL = 0x6,
			ASSET_TYPE_TECHNIQUE_SET = 0x7,
			ASSET_TYPE_IMAGE = 0x8,
			ASSET_TYPE_SOUND = 0x9,
			ASSET_TYPE_SOUND_CURVE = 0xA,
			//ASSET_TYPE_LOADED_SOUND = 11,
			ASSET_TYPE_CLIPMAP = 11,
			ASSET_TYPE_CLIPMAP_PVS = 12,
			ASSET_TYPE_COMWORLD = 13, // confirmed
			ASSET_TYPE_GAMEWORLD_SP = 14,
			ASSET_TYPE_GAMEWORLD_MP = 15,
			ASSET_TYPE_MAP_ENTS = 16,
			ASSET_TYPE_GFXWORLD = 17,
			ASSET_TYPE_LIGHT_DEF = 18,
			ASSET_TYPE_UI_MAP = 19, // probably
			ASSET_TYPE_FONT = 20,
			ASSET_TYPE_MENULIST = 21,
			ASSET_TYPE_MENU = 22,
			ASSET_TYPE_LOCALIZE_ENTRY = 23,
			ASSET_TYPE_WEAPON = 24,
			ASSET_TYPE_SNDDRIVER_GLOBALS = 25, // probably
			ASSET_TYPE_FX = 26,
			ASSET_TYPE_IMPACT_FX = 27,

			// same as IW3
			ASSET_TYPE_AITYPE = 28,
			ASSET_TYPE_MPTYPE = 29,
			ASSET_TYPE_CHARACTER = 30,
			ASSET_TYPE_XMODELALIAS = 31,

			ASSET_TYPE_RAWFILE = 32,
			ASSET_TYPE_STRINGTABLE = 33,

			// not on IW3 or T4/T5, seems to be QoS exclusive
			ASSET_TYPE_XML_TREE = 34,
			ASSET_TYPE_SCENE_ANIM_RESOURCE = 35,
			ASSET_TYPE_CUTSCENE_RESOURCE = 36,
			ASSET_TYPE_CUSTOM_CAMERA_LIST = 37,

			// just guessing
			ASSET_TYPE_COUNT = 38,
			ASSET_TYPE_STRING = 38,
			ASSET_TYPE_ASSETLIST = 39,
		};

		enum dvar_type : __int8
		{
			boolean = 0x0,
			value = 0x1,
			vec2 = 0x2,
			vec3 = 0x3,
			vec4 = 0x4,
			integer = 0x5,
			enumeration = 0x6,
			string = 0x7,
			color = 0x8
		};

		union DvarValue
		{
			bool enabled;
			int integer;
			//unsigned int unsignedInt;
			//float value;
			//float vector[4];
			//const char* string;
			//char color[4];
		};

		union DvarLimits
		{
			struct
			{
				int stringCount;
				const char** strings;
			} enumeration;
			struct
			{
				int min;
				int max;
			} integer;
			struct
			{
				float min;
				float max;
			} value;
			struct
			{
				float min;
				float max;
			} vector;
		};
	}
}
