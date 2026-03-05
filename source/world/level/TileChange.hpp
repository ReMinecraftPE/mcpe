#pragma once

class TileChange
{
public:
	enum UpdateFlags
	{
		UPDATE_NONE                  = 0,

		UPDATE_NEIGHBORS             = 1 << 0,
		UPDATE_LISTENERS             = 1 << 1,
		UPDATE_LISTENERS_SERVER_ONLY = 1 << 2,
		UPDATE_UNK3                  = 1 << 3,
		UPDATE_UNK4                  = 1 << 4,

		UPDATE_ALL = UPDATE_NEIGHBORS | UPDATE_LISTENERS,
		UPDATE_SERVER_ONLY = UPDATE_LISTENERS_SERVER_ONLY
	};

public:
	TileChange(UpdateFlags updateFlags = UPDATE_NONE) : m_updateFlags(updateFlags) {}
	TileChange(int updateFlags) : m_updateFlags(static_cast<UpdateFlags>(updateFlags)) {}

	bool isUpdateNeighbors() const {
		return (m_updateFlags & UPDATE_NEIGHBORS) != UPDATE_NONE;
	}

	bool isUpdateListeners() const {
		return (m_updateFlags & UPDATE_LISTENERS) != UPDATE_NONE;
	}

	bool isUpdateListenersServerOnly() const {
		return (m_updateFlags & UPDATE_LISTENERS_SERVER_ONLY) != UPDATE_NONE;
	}

private:
	UpdateFlags m_updateFlags;
};
