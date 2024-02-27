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
#include <utils/string.hpp>
#include <iostream>

namespace menus
{
	namespace
	{
		int maxPadding = 20;
		#define MENU_TABBING "\t"
		#define MENUDEF_TABBING "\t\t"
		#define ITEMDEF_TABBING "\t\t\t"

		#define PADDING(x) std::left << std::setw(maxPadding - x) << std::cout.fill(' ')
		#define WRITE_TEXT(name, value)	\
				if (value) {				\
					write_text(name, value);	\
				}

		#define WRITE_ACTION(name, value)	\
				if(value) {						\
					write_action(name, value);	\
				}

				//write number if not default value
		#define WRITE_NUMBER(name, def, value)	\
				if(value != def) {					\
					write_number(name,value);		\
				}

		#define WRITE_NUMBER_MAP(name, def, value, map)	\
				if(value != def) {							\
					write_number(name, value, map);			\
				}

		typedef const std::unordered_map<unsigned int, std::string> identifierMap;
		typedef std::unordered_map<unsigned int, std::string>::const_iterator identifierMapIt;

		std::ofstream menu_file_;
		const std::string headerText = "//This menu was generated using qos-exp\n\n#include \"ui/menudefinition.h\"\n\n";
		std::string tabbing;
		identifierMapIt find;

		identifierMap horizontalAlignmentMap =
		{
			{0, "HORIZONTAL_ALIGN_SUBLEFT"},
			{1, "HORIZONTAL_ALIGN_LEFT"},
			{2, "HORIZONTAL_ALIGN_CENTER"},
			{3, "HORIZONTAL_ALIGN_RIGHT"},
			{4, "HORIZONTAL_ALIGN_FULLSCREEN"},
			{5, "HORIZONTAL_ALIGN_NOSCALE"},
			{6, "HORIZONTAL_ALIGN_TO640"},
			{7, "HORIZONTAL_ALIGN_CENTER_SAFEAREA"},
		};

		identifierMap verticalAlignmentMap =
		{
			{0, "VERTICAL_ALIGN_SUBTOP"},
			{1, "VERTICAL_ALIGN_TOP"},
			{2, "VERTICAL_ALIGN_CENTER"},
			{3, "VERTICAL_ALIGN_RIGHT"},
			{4, "VERTICAL_ALIGN_FULLSCREEN"},
			{5, "VERTICAL_ALIGN_NOSCALE"},
			{6, "VERTICAL_ALIGN_TO640"},
			{7, "VERTICAL_ALIGN_CENTER_SAFEAREA"},
		};

		identifierMap operandMap =
		{
			{ 0, "NOOP" },
			{ 1, ")" },
			{ 2, "*" },
			{ 3, "/" },
			{ 4, "%" },
			{ 5, "+" },
			{ 6, "-" },
			{ 7, "!" },
			{ 8, "<" },
			{ 9, "<=" },
			{ 10, ">" },
			{ 11, ">=" },
			{ 12, "==" },
			{ 13, "!=" },
			{ 14, "&&" },
			{ 15, "||" },
			{ 16, "(" },
			{ 17, "," },
			{ 18, "sin(" },
			{ 19, "cos(" },
			{ 20, "min(" },
			{ 21, "max(" },
			{ 22, "milliseconds(" },
			{ 23, "dvarint(" },
			{ 24, "dvarbool(" },
			{ 25, "dvarfloat(" },
			{ 26, "dvarstring(" },
			{ 27, "stat(" },
			{ 28, "ui_active(" },
			{ 29, "flashbanged(" },
			{ 30, "scoped(" },
			{ 31, "scoreboard_visible(" },
			{ 32, "inkillcam(" },
			{ 33, "player(" },
			{ 34, "selecting_location(" },
			{ 35, "team(" },
			{ 36, "otherteam(" },
			{ 37, "marinesfield(" },
			{ 38, "opforfield(" },
			{ 39, "menuisopen(" },
			{ 40, "writingdata(" },
			{ 41, "inlobby(" },
			{ 42, "inprivateparty(" },
			{ 43, "privatepartyhost(" },
			{ 44, "privatepartyhostinlobby(" },
			{ 45, "aloneinparty(" },
			{ 46, "adsjavelin(" },
			{ 47, "weaplockblink(" },
			{ 48, "weapattacktop(" },
			{ 49, "weapattackdirect(" },
			{ 50, "secondsastime(" },
			{ 51, "tablelookup(" },
			{ 52, "localvarint(" },
			{ 53, "localvarbool(" },
			{ 54, "localvarfloat(" },
			{ 55, "localvarstring(" },
			{ 56, "friendsonline(" },
			{ 57, "badallocation(" },
		};

		identifierMap borderTypeMap =
		{
			{0, "WINDOW_BORDER_NONE"},
			{1, "WINDOW_BORDER_FULL"},
			{2, "WINDOW_BORDER_HORZ"},
			{3, "WINDOW_BORDER_VERT"},
			{4, "WINDOW_BORDER_KCGRADIENT"},
			{5, "WINDOW_BORDER_RAISED"},
			{6, "WINDOW_BORDER_SUNKEN"},
		};

		identifierMap ownerDrawTypeMap =
		{
			{1, "CG_OWNERDRAW_BASE"},
			{5, "CG_PLAYER_AMMO_VALUE"},
			{6, "CG_PLAYER_AMMO_BACKDROP"},
			{20, "CG_PLAYER_STANCE"},
			{60, "CG_SPECTATORS"},
			{71, "CG_HOLD_BREATH_HINT"},
			{72, "CG_CURSORHINT"},
			{73, "CG_PLAYER_POWERUP"},
			{74, "CG_PLAYER_HOLDABLE"},
			{75, "CG_PLAYER_INVENTORY"},
			{78, "CG_CURSORHINT_STATUS"},
			{79, "CG_PLAYER_BAR_HEALTH"},
			{80, "CG_MANTLE_HINT"},
			{81, "CG_PLAYER_WEAPON_NAME"},
			{82, "CG_PLAYER_WEAPON_NAME_BACK"},
			{90, "CG_CENTER_MESSAGE"},
			{95, "CG_TANK_BODY_DIR"},
			{96, "CG_TANK_BARREL_DIR"},
			{97, "CG_DEADQUOTE"},
			{98, "CG_PLAYER_BAR_HEALTH_BACK"},
			{99, "CG_MISSION_OBJECTIVE_HEADER"},
			{100, "CG_MISSION_OBJECTIVE_LIST"},
			{101, "CG_MISSION_OBJECTIVE_BACKDROP"},
			{102, "CG_PAUSED_MENU_LINE"},
			{103, "CG_OFFHAND_WEAPON_ICON_FRAG"},
			{104, "CG_OFFHAND_WEAPON_ICON_SMOKEFLASH"},
			{105, "CG_OFFHAND_WEAPON_AMMO_FRAG"},
			{106, "CG_OFFHAND_WEAPON_AMMO_SMOKEFLASH"},
			{107, "CG_OFFHAND_WEAPON_NAME_FRAG"},
			{108, "CG_OFFHAND_WEAPON_NAME_SMOKEFLASH"},
			{109, "CG_OFFHAND_WEAPON_SELECT_FRAG"},
			{110, "CG_OFFHAND_WEAPON_SELECT_SMOKEFLASH"},
			{111, "CG_SAVING"},
			{112, "CG_PLAYER_LOW_HEALTH_OVERLAY"},
			{113, "CG_INVALID_CMD_HINT"},
			{114, "CG_PLAYER_SPRINT_METER"},
			{115, "CG_PLAYER_SPRINT_BACK"},
			{116, "CG_PLAYER_WEAPON_BACKGROUND"},
			{117, "CG_PLAYER_WEAPON_AMMO_CLIP_GRAPHIC"},
			{118, "CG_PLAYER_WEAPON_PRIMARY_ICON"},
			{119, "CG_PLAYER_WEAPON_AMMO_STOCK"},
			{120, "CG_PLAYER_WEAPON_LOW_AMMO_WARNING"},
			{145, "CG_PLAYER_COMPASS_TICKERTAPE"},
			{146, "CG_PLAYER_COMPASS_TICKERTAPE_NO_OBJ"},
			{150, "CG_PLAYER_COMPASS_PLAYER"},
			{151, "CG_PLAYER_COMPASS_BACK"},
			{152, "CG_PLAYER_COMPASS_POINTERS"},
			{153, "CG_PLAYER_COMPASS_ACTORS"},
			{154, "CG_PLAYER_COMPASS_TANKS"},
			{155, "CG_PLAYER_COMPASS_HELICOPTERS"},
			{156, "CG_PLAYER_COMPASS_PLANES"},
			{157, "CG_PLAYER_COMPASS_AUTOMOBILES"},
			{158, "CG_PLAYER_COMPASS_FRIENDS"},
			{159, "CG_PLAYER_COMPASS_MAP"},
			{160, "CG_PLAYER_COMPASS_NORTHCOORD"},
			{161, "CG_PLAYER_COMPASS_EASTCOORD"},
			{162, "CG_PLAYER_COMPASS_NCOORD_SCROLL"},
			{163, "CG_PLAYER_COMPASS_ECOORD_SCROLL"},
			{164, "CG_PLAYER_COMPASS_GOALDISTANCE"},
			{165, "CG_PLAYER_ACTIONSLOT_DPAD"},
			{166, "CG_PLAYER_ACTIONSLOT_1"},
			{167, "CG_PLAYER_ACTIONSLOT_2"},
			{168, "CG_PLAYER_ACTIONSLOT_3"},
			{169, "CG_PLAYER_ACTIONSLOT_4"},
			{170, "CG_PLAYER_COMPASS_ENEMIES"},
			{180, "CG_PLAYER_FULLMAP_BACK"},
			{181, "CG_PLAYER_FULLMAP_MAP"},
			{182, "CG_PLAYER_FULLMAP_POINTERS"},
			{183, "CG_PLAYER_FULLMAP_PLAYER"},
			{184, "CG_PLAYER_FULLMAP_ACTORS"},
			{185, "CG_PLAYER_FULLMAP_FRIENDS"},
			{186, "CG_PLAYER_FULLMAP_LOCATION_SELECTOR"},
			{187, "CG_PLAYER_FULLMAP_BORDER"},
			{188, "CG_PLAYER_FULLMAP_ENEMIES"},
			{190, "CG_VEHICLE_RETICLE"},
			{191, "CG_HUD_TARGETS_VEHICLE"},
			{192, "CG_HUD_TARGETS_JAVELIN"},
			{193, "CG_TALKER1"},
			{194, "CG_TALKER2"},
			{195, "CG_TALKER3"},
			{196, "CG_TALKER4"},
			{200, "UI_OWNERDRAW_BASE"},
			//200 is also UI_HANDICAP

			{201, "UI_EFFECTS"},
			{202, "UI_PLAYERMODEL"},
			{205, "UI_GAMETYPE"},
			{207, "UI_SKILL"},
			{220, "UI_NETSOURCE"},
			{222, "UI_NETFILTER"},
			{238, "UI_VOTE_KICK"},
			{245, "UI_NETGAMETYPE"},
			{247, "UI_SERVERREFRESHDATE"},
			{248, "UI_SERVERMOTD"},
			{249, "UI_GLINFO"},
			{250, "UI_KEYBINDSTATUS"},
			{253, "UI_JOINGAMETYPE "},
			{254, "UI_MAPPREVIEW"},
			{257, "UI_MENUMODEL"},
			{258, "UI_SAVEGAME_SHOT"},
			{262, "UI_SAVEGAMENAME"},
			{263, "UI_SAVEGAMEINFO"},
			{264, "UI_LOADPROFILING"},
			{265, "UI_RECORDLEVEL"},
			{266, "UI_AMITALKING"},
			{267, "UI_TALKER1"},
			{268, "UI_TALKER2"},
			{269, "UI_TALKER3"},
			{270, "UI_TALKER4"},
			{271, "UI_PARTYSTATUS"},
			{272, "UI_LOGGEDINUSER"},
			{273, "UI_RESERVEDSLOTS"},
			{274, "UI_PLAYLISTNAME"},
			{275, "UI_PLAYLISTDESCRIPTION"},
			{276, "UI_USERNAME"},
			{277, "UI_CINEMATIC"},
		};

		identifierMap messageMap =
		{
			{0, "MODE_BOTTOMUP_ALIGN_TOP"},
			{1, "MODE_BOTTOMUP_ALIGN_BOTTOM"},
			{2, "MODE_TOPDOWN_ALIGN_TOP"},
			{3, "MODE_TOPDOWN_ALIGN_BOTTOM"}
		};

		identifierMap feederMap =
		{
			{0, "FEEDER_HEADS"},
			{1, "FEEDER_MAPS"},
			{2, "FEEDER_SERVERS"},
			{3, "FEEDER_CLAN_MEMBERS"},
			{4, "FEEDER_ALLMAPS"},
			{5, "FEEDER_REDTEAM_LIST"},
			{6, "FEEDER_BLUETEAM_LIST"},
			{7, "FEEDER_PLAYER_LIST"},
			{8, "FEEDER_TEAM_LIST"},
			{9, "FEEDER_MODS"},
			{10, "FEEDER_DEMOS"},
			{11, "FEEDER_SCOREBOARD"},
			{12, "FEEDER_Q3HEADS"},
			{13, "FEEDER_SERVERSTATUS"},
			{14, "FEEDER_FINDPLAYER"},
			{15, "FEEDER_CINEMATICS"},
			{16, "FEEDER_SAVEGAMES"},
			{17, "FEEDER_PICKSPAWN"},
			{18, "FEEDER_LOBBY_MEMBERS"},
			{19, "FEEDER_LOBBY_MEMBERS_TALK"},
			{20, "FEEDER_MUTELIST"},
			{21, "FEEDER_PLAYERSTALKING"},
			{22, "FEEDER_SPLITSCREENPLAYERS"},
			{23, "FEEDER_LOBBY_MEMBERS_READY"},
			{24, "FEEDER_PLAYER_PROFILES"},
			{25, "FEEDER_PARTY_MEMBERS"},
			{26, "FEEDER_PARTY_MEMBERS_TALK"},
			{27, "FEEDER_PARTY_MEMBERS_READY"},
			{28, "FEEDER_PLAYLISTS"},
			{29, "FEEDER_GAMEMODES"},
			{30, "FEEDER_LEADERBOARD"},
			{32, "FEEDER_MYTEAM_MEMBERS"},
			{33, "FEEDER_MYTEAM_MEMBERS_TALK"},
			{34, "FEEDER_ENEMY_MEMBERS"},
			{35, "FEEDER_ENEMY_MEMBERS_TALK"},
			{36, "FEEDER_LOBBY_MEMBERS_STAT"},
			{37, "FEEDER_MYTEAM_MEMBERS_STAT"},
			{38, "FEEDER_ENEMY_MEMBERS_STAT"},
			{39, "FEEDER_ONLINEFRIENDS"},
			{40, "FEEDER_LOBBY_MEMBERS_RANK"},
			{41, "FEEDER_PARTY_MEMBERS_RANK"},
			{48, "FEEDER_ENEMY_MEMBERS_RANK"},
			{49, "FEEDER_MYTEAM_MEMBERS_RANK"},
		};

		identifierMap staticFlagsMap =
		{
			{1048576, "decoration"},
			{1048576, "horizontalscroll"},
			{1048576, "autowrapped"},
			{1048576, "popup"},
			{1048576, "outOfBoundsClick"},
			{1048576, "legacySplitScreenScale"},
			{1048576, "hiddenDuringFlashbang"},
			{1048576, "hiddenDuringScope"},
			{1048576, "hiddenDuringGUI"},
		};

		const std::string script_actions_[34] =
		{
			"fadein",
			"fadeout",
			"show",
			"hide",
			"hidemenu",
			"open",
			"close",
			"setasset",
			"setbackground",
			"setteamcolor",
			"setitemcolor",
			"setfocus",
			"setplayermodel",
			"setplayerhead",
			"transition",
			"setcvar",
			"exec",
			"play",
			"playlooped",
			"orbit",
			"uiscript",
			"scriptmenuresponse",
			"setlocalvarint",
			"setlocalvarstring",
			"setlocalvarfloat",
			"setlocalvarbool",
			"execnow",
			"execondvarintvalue",
			"execondvarstringvalue",
			"showmenu",
			"focusfirst",
			"setfocusbydvar",
			"setdvar",
			"execnowOndvarstringvalue",
		};
		const int script_action_length_ = 34;

		const enum keyCodes
		{
			KEY_MULTIPLY = 42,
			KEY_Z = 122,
		};

		identifierMap windowStyleMap =
		{
			{0, "WINDOW_STYLE_EMPTY"},
			{1, "WINDOW_STYLE_FILLED"},
			{2, "WINDOW_STYLE_GRADIENT"},
			{3, "WINDOW_STYLE_SHADER"},
			{4, "WINDOW_STYLE_TEAMCOLOR"},
			{5, "WINDOW_STYLE_DVAR_SHADER"},
			{6, "WINDOW_STYLE_LOADBAR"},
		};

		identifierMap itemTypeMap =
		{
			{0, "ITEM_TYPE_TEXT"},
			{1, "ITEM_TYPE_BUTTON"},
			{2, "ITEM_TYPE_RADIOBUTTON"},
			{3, "ITEM_TYPE_CHECKBOX"},
			{4, "ITEM_TYPE_EDITFIELD"},
			{5, "ITEM_TYPE_COMBO"},
			{6, "ITEM_TYPE_LISTBOX"},
			{7, "ITEM_TYPE_MODEL"},
			{8, "ITEM_TYPE_OWNERDRAW"},
			{9, "ITEM_TYPE_NUMERICFIELD"},
			{10, "ITEM_TYPE_SLIDER"},
			{11, "ITEM_TYPE_YESNO"},
			{12, "ITEM_TYPE_MULTI"},
			{13, "ITEM_TYPE_DVARENUM"},
			{14, "ITEM_TYPE_BIND"},
			{15, "ITEM_TYPE_MENUMODEL"},
			{16, "ITEM_TYPE_VALIDFILEFIELD"},
			{17, "ITEM_TYPE_DECIMALFIELD"},
			{18, "ITEM_TYPE_UPREDITFIELD"},
			{19, "ITEM_TYPE_GAME_MESSAGE_WINDOW"},
		};

		identifierMap itemTextStyleMap =
		{
			{0, "ITEM_TEXTSTYLE_NORMAL"},
			{1, "ITEM_TEXTSTYLE_BLINK"},
			{3, "ITEM_TEXTSTYLE_SHADOWED"},
			{5, "ITEM_TEXTSTYLE_SHADOWEDMORE"},
			{128, "ITEM_TEXTSTYLE_MONOSPACE"},
		};

		identifierMap itemTextAlignMap =
		{
			{0, "ITEM_ALIGN_LEFT"},
			{1, "ITEM_ALIGN_CENTER"},
			{2, "ITEM_ALIGN_RIGHT"},
			{3, "ITEM_ALIGN_X_MASK"},
		};

		identifierMap listBoxTypeMap =
		{
			{0, "LISTBOX_TEXT"},
			{1, "LISTBOX_IMAGE"}
		};

		const enum itemType
		{
			ITEM_TYPE_EDITFIELD = 4,
			ITEM_TYPE_LISTBOX = 6,
			ITEM_TYPE_MULTI = 12,
			ITEM_TYPE_DVARENUM = 13,
			ITEM_TYPE_GAME_MESSAGE_WINDOW = 19,
		};

		identifierMap fontTypeMap =
		{
			{0, "UI_FONT_DEFAULT"},
			{1, "UI_FONT_NORMAL"},
			{2, "UI_FONT_BIG"},
			{3, "UI_FONT_SMALL"},
			{4, "UI_FONT_BOLD"},
			{5, "UI_FONT_CONSOLE"},
			{6, "UI_FONT_OBJECTIVE"},
		};

		const enum dvarFlags
		{
			DISABLEDVAR = 2,
			SHOWDVAR = 4,
			HIDEDVAR = 8,
			FOCUSDVAR = 16,
		};

		void write_text(std::string name, const char* text)
		{
			menu_file_ << tabbing << name << PADDING(name.length()) << "\"" << text << "\"\n";
		}

		//write single line
		void write_line(std::string name)
		{
			menu_file_ << tabbing << name << "\n";
		}

		void write_rect(game::qos::rectDef_s rect)
		{
			auto hAlign = std::to_string(rect.horzAlign);
			find = horizontalAlignmentMap.find(rect.horzAlign);

			if (find != horizontalAlignmentMap.end())
			{
				hAlign = find->second;
			}

			auto vAlign = std::to_string(rect.vertAlign);
			find = verticalAlignmentMap.find(rect.vertAlign);

			if (find != verticalAlignmentMap.end())
			{
				vAlign = find->second;
			}

			menu_file_ << tabbing << "rect" << PADDING(4)
				<< rect.x << " "
				<< rect.y << " "
				<< rect.w << " "
				<< rect.h << " "
				<< hAlign << " "
				<< vAlign << "\n";
		}

		void write_statement(std::string name, game::qos::statement_s stmt)
		{
			if (stmt.numEntries == 0)
			{
				return;
			}

			std::string statement;
			menu_file_ << tabbing << "exp " << PADDING(name.length() + 3) << name << " ";

			for (int i = 0; i < stmt.numEntries; i++)
			{
				if (stmt.entries[i]->type == 0)
				{
					find = operandMap.find(stmt.entries[i]->data.op);

					if (find != operandMap.end())
					{
						statement += find->second;
					}
				}
				else if (stmt.entries[i]->type == 1)
				{
					switch (stmt.entries[i]->data.operand.dataType)
					{
					case game::qos::expDataType::VAL_INT:
					{
						statement += std::to_string(stmt.entries[i]->data.operand.internals.intVal);
						break;
					}
					case game::qos::expDataType::VAL_FLOAT:
					{
						statement += std::to_string(stmt.entries[i]->data.operand.internals.floatVal);
						break;
					}
					case game::qos::expDataType::VAL_STRING:
					{
						statement += "\"";
						statement += stmt.entries[i]->data.operand.internals.string;
						statement += "\"";
						break;
					}
					}
				}
			}

			//final close bracket does not get stored and sometimes open brackets do not get stored so we try to replace missing ones
			int open_brackets = 0;
			for (auto i = 0; i < static_cast<int>(statement.length()); i++)
			{
				if (statement.at(i) == '(')
				{
					open_brackets++;
				}
				else if (statement.at(i) == ')')
				{
					open_brackets--;
				}
			}

			if (open_brackets < 0)
			{
				statement += "//More closed brackets ')' then open.";
			}
			else
			{
				for (int i = 0; i < open_brackets; i++)
				{
					statement += ')';
				}
			}

			menu_file_ << statement << "\n";
		}

		void write_visible(game::qos::statement_s stmt)
		{
			if (stmt.numEntries == 0)
			{
				menu_file_ << tabbing << "visible" << PADDING(7) << "1\n";
			}
			else
			{
				write_statement("visible", stmt);
			}
		}

		template <typename T>
		void write_number(std::string name, T num)
		{
			menu_file_ << tabbing << name << PADDING(name.length()) << num << "\n";
		}

		void write_number(std::string name, int num, identifierMap& map)
		{
			auto number = std::to_string(num);
			find = map.find(num);

			if (find != map.end())
			{
				number = find->second;
			}

			menu_file_ << tabbing << name << PADDING(name.length()) << number << "\n";
		}

		void write_vec4(std::string name, float vec4[])
		{
			menu_file_ << tabbing << name << PADDING(name.length())
				<< vec4[0] << " "
				<< vec4[1] << " "
				<< vec4[2] << " "
				<< vec4[3] << "\n";
		}

		void write_flag(int num)
		{
			find = staticFlagsMap.find(num);

			if (find != staticFlagsMap.end())
			{
				menu_file_ << tabbing << find->second << "\n";
			}
		}

		void write_action(std::string name, std::string action)
		{
			bool string_open = false;
			std::string word;

			// I dont think this is the best solution
			// Need to remove quote character around script actions but leave quotes everywhere else
			int start = 0;

			for (auto i = 0; i < static_cast<int>(action.length()); i++)
			{
				if (action.at(i) == '"')
				{
					if (!string_open)
					{
						string_open = true;
						start = i;

						continue;
					}
					else
					{
						string_open = false;

						for (int z = 0; z < script_action_length_; z++)
						{
							utils::string::to_lower(word);

							if (word.compare(script_actions_[z]) == 0)
							{
								action.replace(start, 1, " ");
								action.replace(i, 1, " ");
								z = script_action_length_;
							}
						}

						word = "";
					}
				}

				if (string_open)
				{
					word += action.at(i);
				}
			}

			menu_file_ << tabbing << name << PADDING(name.length()) << "{ " << action << " }" << "\n";
		}

		void write_key_action(int key, std::string action)
		{
			std::string name;

			//range for single character keys
			if (key >= KEY_MULTIPLY && key <= KEY_Z)
			{
				name = "execKey \"";
				name += (char)key;
				name += "\" ";
			}
			else
			{
				name = "execKeyInt " + std::to_string(key) + " ";
			}

			write_action(name, action);
		}



		void write_item_def(game::qos::itemDef_s& itemDef)
		{
			//itemdef open
			menu_file_ << "\n" << tabbing << "itemDef\n";
			menu_file_ << tabbing << "{\n";
			tabbing = ITEMDEF_TABBING;

			WRITE_TEXT("name", itemDef.window.name);

			write_rect(itemDef.window.rectClient);
			write_visible(itemDef.visibleExp);

			if (itemDef.window.ownerDraw)
			{
				write_number("ownerDraw", itemDef.window.ownerDraw, ownerDrawTypeMap);
				write_number("ownerDrawFlag", itemDef.window.ownerDrawFlags);
			}

			write_number("style", itemDef.window.style, windowStyleMap);

			if (itemDef.window.border != 0 && itemDef.window.borderSize != 0)
			{
				write_number("border", itemDef.window.border, borderTypeMap);
				write_number("borderSize", itemDef.window.borderSize);
				write_vec4("borderColor", itemDef.window.borderColor);
			}

			write_vec4("foreColor", itemDef.window.foreColor);
			write_vec4("backColor", itemDef.window.backColor);

			if (itemDef.window.outlineColor[3] != 0)
			{
				write_vec4("outlineColor", itemDef.window.outlineColor);
			}

			WRITE_TEXT("group", itemDef.window.group);

			if (itemDef.window.background)
			{
				write_text("background", itemDef.window.background->name);
			}

			write_number("type", itemDef.type, itemTypeMap);

			WRITE_NUMBER_MAP("textfont", 0, itemDef.fontEnum, fontTypeMap);
			WRITE_NUMBER_MAP("textAlign", 0, itemDef.textAlignMode, itemTextAlignMap);
			WRITE_NUMBER("textAlignX", 0, itemDef.textalignx);
			WRITE_NUMBER("textAlignY", 0, itemDef.textaligny);
			WRITE_NUMBER("textScale", 0.55f, itemDef.textscale);
			WRITE_NUMBER_MAP("textStyle", 0, itemDef.textStyle, itemTextStyleMap);
			WRITE_TEXT("text", itemDef.text);

			if (itemDef.type == ITEM_TYPE_LISTBOX)
			{
				write_number("feeder", itemDef.feeder, feederMap);
				write_number("elementWidth", itemDef.typeData.listBox->elementWidth);
				write_number("elementHeight", itemDef.typeData.listBox->elementHeight);
				write_number("elementType", itemDef.typeData.listBox->elementStyle, listBoxTypeMap);
				WRITE_ACTION("doubleclick", itemDef.typeData.listBox->doubleClick);
				write_vec4("selectBorder", itemDef.typeData.listBox->selectBorder);
				write_vec4("disableColor", itemDef.typeData.listBox->disableColor);

				if (itemDef.typeData.listBox->selectIcon)
				{
					write_text("selectIcon", itemDef.typeData.listBox->selectIcon->name);
				}

				if (itemDef.typeData.listBox->notselectable)
				{
					write_line("notselectable");
				}

				if (itemDef.typeData.listBox->noScrollBars)
				{
					write_line("noScrollBars");
				}

				if (itemDef.typeData.listBox->usePaging)
				{
					write_line("usePaging");
				}

				menu_file_ << tabbing << "columns" << PADDING(7) << itemDef.typeData.listBox->numColumns << "\n";

				for (int i = 0; i < itemDef.typeData.listBox->numColumns; i++)
				{
					menu_file_ << tabbing << "\t\t\t\t" << std::setw(0) << itemDef.typeData.listBox->columnInfo[i].pos
						<< " " << itemDef.typeData.listBox->columnInfo[i].width
						<< " " << itemDef.typeData.listBox->columnInfo[i].maxChars << "\n";
				}
			}
			else if (itemDef.type == ITEM_TYPE_EDITFIELD)
			{
				write_number("maxChars", itemDef.typeData.editField->maxChars);
				write_number("maxPaintChars", itemDef.typeData.editField->maxPaintChars);

				if (itemDef.typeData.editField->maxCharsGotoNext)
				{
					write_line("maxCharsGotoNext");
				}
			}
			else if (itemDef.type == ITEM_TYPE_MULTI)
			{

				if (itemDef.typeData.multi->strDef == 1)
				{
					menu_file_ << tabbing << "dvarStrList" << PADDING(13) << "{ ";
				}
				else
				{
					menu_file_ << tabbing << "dvarFloatList" << PADDING(13) << "{ ";
				}

				for (int i = 0; i < itemDef.typeData.multi->count; i++)
				{
					if (itemDef.typeData.multi->strDef == 1)
					{
						menu_file_ << '"' << itemDef.typeData.multi->dvarList[i] << '"' << ", " << '"' << itemDef.typeData.multi->dvarStr[i] << '"' << "; ";
					}
					else
					{
						menu_file_ << '"' << itemDef.typeData.multi->dvarList[i] << '"' << " " << itemDef.typeData.multi->dvarValue[i] << " ";
					}
				}

				menu_file_ << " }\n";
			}
			else if (itemDef.type == ITEM_TYPE_DVARENUM)
			{
				//idk what this does
			}
			else if (itemDef.type == ITEM_TYPE_GAME_MESSAGE_WINDOW)
			{
				write_number("gameMsgWindowIndex", itemDef.gameMsgWindowIndex);
				write_number("gameMsgWindowMode", itemDef.gameMsgWindowMode, messageMap);
			}

			WRITE_ACTION("mouseEnterText", itemDef.mouseEnterText);
			WRITE_ACTION("mouseExitText", itemDef.mouseExitText);
			WRITE_ACTION("mouseEnter", itemDef.mouseEnter);
			WRITE_ACTION("mouseExit", itemDef.mouseExit);
			WRITE_ACTION("action", itemDef.action);
			WRITE_ACTION("onAccept", itemDef.onAccept);
			WRITE_ACTION("onAccept", itemDef.onAccept);
			WRITE_ACTION("onFocus", itemDef.onFocus);
			WRITE_ACTION("leaveFocus", itemDef.mouseExitText);

			while (itemDef.onKey)
			{
				write_key_action(itemDef.onKey->key, itemDef.onKey->action);
				itemDef.onKey = itemDef.onKey->next;
			}

			write_statement("text", itemDef.textExp);
			write_statement("material", itemDef.materialExp);
			write_statement("rect X", itemDef.rectXExp);
			write_statement("rect Y", itemDef.rectYExp);
			write_statement("rect W", itemDef.rectWExp);
			write_statement("rect H", itemDef.rectHExp);
			//write_statement("foreColor A", itemDef.forecolorAExp);

			WRITE_TEXT("dvar", itemDef.dvar);
			WRITE_TEXT("dvartest", itemDef.dvarTest);

			if (itemDef.dvarFlags != 0)
			{
				std::string dvarFlag = "";

				switch (itemDef.dvarFlags)
				{
				case DISABLEDVAR:
					dvarFlag = "disableDvar";
					break;
				case SHOWDVAR:
					dvarFlag = "showDvar";
					break;
				case HIDEDVAR:
					dvarFlag = "hideDvar";
					break;
				case FOCUSDVAR:
					dvarFlag = "focusDvar";
				}

				write_action(dvarFlag, itemDef.enableDvar);
			}

			if (itemDef.window.staticFlags)
			{
				write_flag(itemDef.window.staticFlags);
			}

			tabbing = MENUDEF_TABBING;

			//close itemDef
			menu_file_ << tabbing << "}\n";
		}

		void write_menu_def(game::qos::menuDef_t* menuDef)
		{
			tabbing = MENU_TABBING;

			//open menudef
			menu_file_ << tabbing << "menuDef" << "\n" << tabbing << "{\n";
			tabbing = MENUDEF_TABBING;

			WRITE_TEXT("name", menuDef->window.name);

			write_rect(menuDef->window.rect);
			write_visible(menuDef->visibleExp);
			write_number("fullScreen", menuDef->fullScreen);

			WRITE_NUMBER("blurWorld", 0, menuDef->blurRadius);
			WRITE_NUMBER("fadeCycle", 0, menuDef->fadeCycle);
			WRITE_NUMBER("fadeClamp", 0, menuDef->fadeClamp);
			WRITE_NUMBER("fadeAmount", 0, menuDef->fadeAmount);
			WRITE_NUMBER("fadeInAmount", 0, menuDef->fadeInAmount);

			if (menuDef->window.border != 0 && menuDef->window.borderSize != 0) {

				write_number("border", menuDef->window.border, borderTypeMap);
				write_number("borderSize", menuDef->window.borderSize);
				write_vec4("borderColor", menuDef->window.borderColor);
			}

			write_vec4("foreColor", menuDef->window.foreColor);
			write_vec4("backColor", menuDef->window.backColor);
			write_vec4("outlineColor", menuDef->window.outlineColor);
			write_vec4("disableColor", menuDef->disableColor);
			write_vec4("focusColor", menuDef->focusColor);

			if (menuDef->window.ownerDraw)
			{
				write_number("ownerDraw", menuDef->window.ownerDraw, ownerDrawTypeMap);
				write_number("ownerDrawFlag", menuDef->window.ownerDrawFlags);
			}

			WRITE_TEXT("allowBinding", menuDef->allowedBinding);
			WRITE_TEXT("soundLoop", menuDef->soundName);

			if (menuDef->window.background)
			{
				write_text("background", menuDef->window.background->name);
			}

			if (menuDef->window.staticFlags)
			{
				write_flag(menuDef->window.staticFlags);
			}

			WRITE_ACTION("onOpen", menuDef->onOpen);
			WRITE_ACTION("onClose", menuDef->onClose);
			WRITE_ACTION("onESC", menuDef->onESC);

			while (menuDef->onKey)
			{
				write_key_action(menuDef->onKey->key, menuDef->onKey->action);
				menuDef->onKey = menuDef->onKey->next;
			}

			write_statement("rect X", menuDef->rectXExp);
			write_statement("rect Y", menuDef->rectYExp);

			for (int i = 0; i < menuDef->itemCount; i++)
			{
				write_item_def(*menuDef->items[i]);
			}

			tabbing = MENU_TABBING;

			//close menudef
			menu_file_ << tabbing << "}\n";
		}

		void dump(game::qos::menuDef_t* menu)
		{
			if (!menu)
			{
				return;
			}

			std::string file_path = "qos-exp\\dump\\menus\\"s;
			std::filesystem::create_directories(file_path);

			std::filesystem::path path = "qos-exp\\dump\\menus\\" + std::string(menu->window.name) + ".menu";
			menu_file_.open(path.c_str());

			menu_file_ << headerText;

			menu_file_ << "{\n";

			write_menu_def(menu);

			menu_file_ << "}\n";
			menu_file_.close();
		}

		game::qos::menuDef_t* convert(game::qos::menuDef_t* menufile)
		{
			if (!menufile) return nullptr;

			//auto iw4_asset = utils::memory::allocate<game::iw4::RawFile>();
			/*iw4_asset->name = rawfile->name;
			iw4_asset->compressedLen = 0;
			iw4_asset->len = rawfile->len;
			iw4_asset->buffer = rawfile->buffer;*/
			//return iw4_asset;
			return nullptr;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			scheduler::once([&]()
			{
				command::add("dumpmenu", [](const command::params& params)
				{
					if (params.size() < 2)
					{
						console::info("USAGE: dumpmenu <name>\n");
						return;
					}

					const auto name = params[1];

					auto header = game::DB_FindXAssetHeader(game::qos::ASSET_TYPE_MENU, name);
					if (!header.menu)
					{
						console::error("dumpmenu failed on '%s'\n", name);
						return;
					}

					/*auto converted = menus::convert(header.menu);
					map_dumper::api->write(game::iw4::ASSET_TYPE_MENU, converted);*/

					menus::dump(header.menu);

					console::info("dumped '%s'\n", name);
				});

				command::add("dumpmenulist", [](const command::params& params)
					{
						if (params.size() < 2)
						{
							console::info("USAGE: dumpmenulist <name>\n");
							return;
						}

						const auto name = params[1];

						auto header = game::DB_FindXAssetHeader(game::qos::ASSET_TYPE_MENULIST, name);
						if (!header.menuList)
						{
							console::error("dumpmenulist failed on '%s'\n", name);
							return;
						}

						/*auto converted = menus::convert(header.menu);
						map_dumper::api->write(game::iw4::ASSET_TYPE_MENU, converted);*/

						for (auto i = 0; i < header.menuList->menuCount; i++)
						{
							menus::dump(header.menuList->menus[i]);
						}

						console::info("dumped '%s'\n", name);
					});
			}, scheduler::main);
		}

		game::qos::XAssetType get_type() override
		{
			return game::qos::ASSET_TYPE_RAWFILE;
		}

		game::iw4::XAssetHeader convert_asset(game::qos::XAssetHeader header) override
		{
			return { convert(header.menu) };
		}
	};
}

REGISTER_COMPONENT(menus::component)
