#pragma once

class TileChange
{
public:
	enum UpdateFlags
	{
		UPDATE_NONE      = 0 << 0,             //  0 0 0  //  

		UPDATE_NEIGHBORS = 1 << 0,	           //  0 0 1  //  
		UPDATE_LISTENERS = 1 << 1,             //  0 1 0  //  
		UPDATE_SILENT  = 1 << 2,		       //  1 0 0  //

		UPDATE_ALL = UPDATE_NEIGHBORS | UPDATE_LISTENERS, // the default behavior
	};

public:
	TileChange(UpdateFlags updateFlags = UPDATE_NONE) : m_updateFlags(updateFlags) {}

	bool isUpdateNeighbors() const {
		return (m_updateFlags & UPDATE_NEIGHBORS) != UPDATE_NONE;
	}

	bool isUpdateListeners() const {
		return (m_updateFlags & UPDATE_LISTENERS) != UPDATE_NONE;
	}

	bool isUpdateSilent() const {
		return (m_updateFlags & UPDATE_SILENT) != UPDATE_NONE;
	}

	bool isUpdateAll() const {
		return (m_updateFlags & UPDATE_ALL) != UPDATE_NONE;
	}

private:
	UpdateFlags m_updateFlags;
};