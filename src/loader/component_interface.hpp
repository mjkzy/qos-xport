#pragma once

#include <game/game.hpp>
#include <game/structs.IW4.hpp>

class component_interface
{
public:
	virtual ~component_interface()
	{
	}

	virtual void post_start()
	{
	}

	virtual void post_load()
	{
	}

	virtual void pre_destroy()
	{
	}

	virtual void* load_import([[maybe_unused]] const std::string& library, [[maybe_unused]] const std::string& function)
	{
		return nullptr;
	}

	virtual bool is_supported()
	{
		return true;
	}

	virtual game::qos::XAssetType get_type()
	{
		return game::qos::ASSET_TYPE_INVALID;
	}

	virtual game::iw4::XAssetHeader convert_asset([[maybe_unused]] game::qos::XAssetHeader header)
	{
		game::iw4::XAssetHeader default_header{};
		default_header.data = nullptr;
		return default_header;
	}
};
