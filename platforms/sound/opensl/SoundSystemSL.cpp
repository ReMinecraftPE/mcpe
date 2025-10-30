/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#include "CustomSoundSystem.hpp"
#include "common/Logger.hpp"

#define C_MAX_SOUNDS 4

// TODO: Use other types of mutexes later.
SLObjectItf SoundSystemSL::objEngine;
std::vector<SLObjectItf> SoundSystemSL::toRemove;
pthread_mutex_t SoundSystemSL::toRemoveMutex;

SoundSystemSL::SoundSystemSL()
{
	m_listenerPos = Vec3::ZERO;
	m_soundCount = 0;
	m_3dLocationItf = nullptr;
	m_bAvailable = true;
	init();
}

SoundSystemSL::~SoundSystemSL()
{
	pthread_mutex_unlock(&toRemoveMutex);

	for (SLSoundList::iterator it = m_playingSounds.begin();
		 it != m_playingSounds.end();
		 ++it)
	{
		(**it)->Destroy(*it);
	}

	(*m_slOutputMix)->Destroy(m_slOutputMix);

	if (objEngine)
	{
		(*objEngine)->Destroy(objEngine);
		objEngine = nullptr;
	}
}

void SoundSystemSL::init()
{
	toRemove.clear();
	// some weird stuff

	if (objEngine)
		(*objEngine)->Destroy(objEngine);

	SLEngineOption option;
	option.feature = SL_ENGINEOPTION_THREADSAFE;
	option.data = true;
	checkErr(slCreateEngine(&objEngine, 1, &option, 0, nullptr, nullptr));

	if (checkErr((*objEngine)->Realize(objEngine, false)))
	{
		m_bAvailable = false;
		return;
	}

	checkErr((*objEngine)->GetInterface(objEngine, SL_IID_ENGINE, &m_slEngine));
	checkErr((*m_slEngine)->CreateOutputMix(m_slEngine, &m_slOutputMix, 0, nullptr, nullptr));
	checkErr((*m_slOutputMix)->Realize(m_slOutputMix, false));
}

bool SoundSystemSL::checkErr(SLresult res)
{
	if (res == SL_RESULT_SUCCESS)
		return false;

	LOG_E("OpenSL error: %d\n", res);
	return true;
}

void SoundSystemSL::removePlayer(SLAndroidSimpleBufferQueueItf caller, void* context)
{
	pthread_mutex_lock(&toRemoveMutex);
	toRemove.push_back((SLObjectItf) context);
	pthread_mutex_unlock(&toRemoveMutex);
}

void SoundSystemSL::removeStoppedSounds()
{
	pthread_mutex_lock(&toRemoveMutex);
	m_tempToRemove = toRemove;
	toRemove.clear();
	pthread_mutex_unlock(&toRemoveMutex);

	for (int i = 0; i < m_tempToRemove.size(); i++)
	{

		for (SLSoundList::iterator it = m_playingSounds.begin();
			 it != m_playingSounds.end();
			 ++it)
		{
			if (*it == m_tempToRemove[i])
			{
				m_playingSounds.erase(it);
				break;
			}
		}

		(*m_tempToRemove[i])->Destroy(m_tempToRemove[i]);
		m_soundCount--;
	}
}


void SoundSystemSL::setListenerPos(const Vec3& pos)
{
	if (!m_3dLocationItf)
	{
		m_listenerPos = pos;
		return;
	}

	SLVec3D vec;
	vec.x = int(1000.0f * pos.x);
	vec.y = int(1000.0f * pos.y);
	vec.z = int(1000.0f * pos.z);
	checkErr((*m_3dLocationItf)->SetLocationCartesian(
		m_3dLocationItf,
		&vec
	));
}

void SoundSystemSL::setListenerAngle(const Vec2& rot)
{
	if (!m_3dLocationItf)
		return;

	checkErr((*m_3dLocationItf)->SetOrientationAngles(
		m_3dLocationItf,
		int(180000.0f * rot.x),
		0,
		0
	));
}

void SoundSystemSL::playAt(const SoundDesc &sound, const Vec3& pos, float volume, float pitch)
{
	removeStoppedSounds();

	if (m_soundCount >= C_MAX_SOUNDS)
		return;

	SLDataSource dataSource;
	SLDataSink dataSink;
	SLDataLocator_AndroidSimpleBufferQueue dataSourceLocator;
	SLDataFormat_PCM dataSourceFormat;
	SLDataLocator_OutputMix dataSinkLocator;
	SLObjectItf pAudioPlayer;
	SLPlayItf pPlayItf;
	SLVolumeItf pVolumeItf;
	SLmillibel maxVolume;
	SLAndroidSimpleBufferQueueItf pBufferQueueItf;
	static SLInterfaceID static_iid_array[2];
	static SLboolean static_bool_array[2];

	dataSource.pLocator = &dataSourceLocator;
	dataSource.pFormat  = &dataSourceFormat;
	dataSink.pLocator = &dataSinkLocator;
	dataSink.pFormat = nullptr;

	// TODO: Nicer initialization based on initializer lists :)
	dataSourceLocator.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	dataSourceLocator.numBuffers = 2;
	dataSinkLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	dataSinkLocator.outputMix = m_slOutputMix;
	dataSourceFormat.numChannels = sound.m_header.m_channels;
	dataSourceFormat.channelMask = sound.m_header.m_channels == 1 ? 4 : 3;
	dataSourceFormat.samplesPerSec = 1000 * sound.m_header.m_sample_rate;
	dataSourceFormat.formatType = SL_DATAFORMAT_PCM;
	dataSourceFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;
	dataSourceFormat.bitsPerSample = 8 * sound.m_header.m_bytes_per_sample;
	dataSourceFormat.containerSize = 8 * sound.m_header.m_bytes_per_sample;
	static_iid_array[0] = SL_IID_BUFFERQUEUE;
	static_iid_array[1] = SL_IID_VOLUME;
	static_bool_array[0] = true;
	static_bool_array[1] = true;

	checkErr((*m_slEngine)->CreateAudioPlayer(
			m_slEngine,
			&pAudioPlayer,
			&dataSource,
			&dataSink,
			2,
			static_iid_array,
			static_bool_array
	));
	checkErr((*pAudioPlayer)->Realize(pAudioPlayer, false));
	checkErr((*pAudioPlayer)->GetInterface(pAudioPlayer, SL_IID_PLAY, &pPlayItf));
	checkErr((*pAudioPlayer)->GetInterface(pAudioPlayer, SL_IID_VOLUME, &pVolumeItf));
	checkErr((*pVolumeItf)->GetMaxVolumeLevel(pVolumeItf, &maxVolume));
	SLmillibel remappedVolume = int(float(maxVolume) + (1.0f - volume) * -2000.0f);
	LOG_I("min: %d, max: %d, current: %d (%f)\n", SL_MILLIBEL_MIN, maxVolume, remappedVolume, volume);
	checkErr((*pVolumeItf)->SetVolumeLevel(pVolumeItf, remappedVolume));
	checkErr((*pAudioPlayer)->GetInterface(pAudioPlayer, SL_IID_BUFFERQUEUE, &pBufferQueueItf));
	checkErr((*pBufferQueueItf)->RegisterCallback(pBufferQueueItf, removePlayer, (void*) pAudioPlayer));
	checkErr((*pBufferQueueItf)->Enqueue(pBufferQueueItf, sound.m_buffer.m_pData, sound.m_buffer.m_dataSize));
	checkErr((*pPlayItf)->SetPlayState(pPlayItf, SL_PLAYSTATE_PLAYING));

	m_playingSounds.push_back(pAudioPlayer);
	m_soundCount++;
}

// Confirmed Stubs
void SoundSystemSL::destroy()
{

}
