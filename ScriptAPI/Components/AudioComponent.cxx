#include "AudioComponent.hxx"
#include "../TypeConversion.hxx"
#include "../EngineInterface.hxx"

namespace ScriptAPI
{
	AudioComponent::AudioComponent(TDS::EntityID ID) : entityID(ID), transform(TransformComponent(ID))
	{
		gameObject = EngineInterface::GetGameObject(ID);
		TDS::SoundInfo temp;
	}

	void AudioComponent::SetEntityID(TDS::EntityID id)
	{
		entityID = id;
		transform = TransformComponent(id);
		gameObject = EngineInterface::GetGameObject(id);
	}

	TDS::EntityID AudioComponent::GetEntityID()
	{
		return entityID;
	}

	void AudioComponent::set3DCoords(float x, float y, float z, System::String^ name)
	{
		TDS::proxy_audio_system::ScriptSetPosition(TDS::floatsToVec3(x, y, z), toStdString(name));
	}

	void AudioComponent::set3DCoords(Vector3 in_pos, System::String^ name)
	{
		//pos = in_pos;
		TDS::proxy_audio_system::ScriptSetPosition(TDS::floatsToVec3(in_pos.X, in_pos.Y, in_pos.Z), toStdString(name));
	}

	void AudioComponent::setPlayerCoords(Vector3 in_pos, Vector3 in_for, Vector3 in_up)
	{
		TDS::proxy_audio_system::ScriptSetListenerPos(TDS::floatsToVec3(in_pos.X, in_pos.Y, in_pos.Z),
			TDS::floatsToVec3(in_for.X, in_for.Y, in_for.Z),
			TDS::floatsToVec3(in_up.X, in_up.Y, in_up.Z));
	}

	bool AudioComponent::isLoaded()
	{
		return (whatState == TDS::SOUND_LOADED);
	}

	bool AudioComponent::isit3D()
	{
		return is3D;
	}

	bool AudioComponent::isitLoop(System::String^ pathing)
	{
		return TDS::proxy_audio_system::ScriptGetLoop(toStdString(pathing));
	}

	bool AudioComponent::isitMuted()
	{
		return isMuted;
	}

	bool AudioComponent::finished(System::String^ str_path)
	{
		return TDS::proxy_audio_system::checkifdone(toStdString(str_path));
	}

	bool AudioComponent::anyPlaying()
	{
		return TDS::proxy_audio_system::ScriptAnySoundPlaying();
	}

	bool AudioComponent::checkPlaying(System::String^ str_path)
	{
		return TDS::proxy_audio_system::CheckPlaying(toStdString(str_path));
	}

	bool AudioComponent::checkPaused(System::String^ str_path)
	{
		return TDS::proxy_audio_system::CheckPause(toStdString(str_path));
	}

	Vector3 AudioComponent::get3DCoords()
	{
		return pos;
	}

	snd AudioComponent::getState()
	{
		return whatState;
	}

	unsigned int AudioComponent::getUniqueID()
	{
		return uniqueID;
	}

	unsigned int AudioComponent::getMSLength()
	{
		return MSLength;
	}

	std::string AudioComponent::getFilePath()
	{
		return filePath;
	}

	Vector3 AudioComponent::getListenerPos()
	{
		return Vector3(TDS::proxy_audio_system::getListenerPos());
	}

	void AudioComponent::setFilePath(System::String^ str_path)
	{
		filePath = toStdString(str_path);
	}

	float AudioComponent::getX()
	{
		return pos.X;
	}

	float AudioComponent::getY()
	{
		return pos.Y;
	}

	float AudioComponent::getZ()
	{
		return pos.Z;
	}

	float AudioComponent::getReverbAmount()
	{
		return ReverbAmount;
	}

	float AudioComponent::getVolume(System::String^ pathing)
	{
		return TDS::proxy_audio_system::getVolume(toStdString(pathing));
	}
	
	float AudioComponent::getMasterVol()
	{
		return TDS::proxy_audio_system::getMasterVolume();
	}

	float AudioComponent::getBGMVol()
	{
		return TDS::proxy_audio_system::getBGMVolume();
	}

	float AudioComponent::getSFXVol()
	{
		return TDS::proxy_audio_system::getSFXVolume();
	}

	void AudioComponent::setVolume(float vol, System::String^ pathing)
	{
		//volume = vol;
		TDS::proxy_audio_system::SetVolume(vol, toStdString(pathing));
	}

	void AudioComponent::setMasterVol(float vol)
	{
		TDS::proxy_audio_system::SetMasterVolume(vol);
	}

	void AudioComponent::setBGMVol(float vol)
	{
		TDS::proxy_audio_system::SetBGMVolume(vol);
	}

	void AudioComponent::setSFXVol(float vol)
	{
		TDS::proxy_audio_system::SetSFXVolume(vol);
	}

	void AudioComponent::setMSLength(unsigned int len)
	{
		MSLength = len;
	}

	void AudioComponent::setState(snd setting)
	{
		whatState = setting;
	}

	void AudioComponent::setLoop(bool condition, System::String^ pathing)
	{
		TDS::proxy_audio_system::ScriptSetLoop(condition, toStdString(pathing));
	}

	void AudioComponent::set3D(bool condition)
	{
		is3D = condition;
	}

	void AudioComponent::setMute(bool condition)
	{
		isMuted = condition;
	}

	void AudioComponent::play(System::String^ pathing)
	{
		TDS::proxy_audio_system::ScriptPlay(toStdString(pathing));
	}

	void AudioComponent::playAllPaused()
	{
		TDS::proxy_audio_system::ScriptPlayAllPaused();
	}

	void AudioComponent::playQueue()
	{
		TDS::proxy_audio_system::Play_queue();
	}

	void AudioComponent::pause(System::String^ pathing)
	{
		TDS::proxy_audio_system::ScriptPause(toStdString(pathing));
	}

	void AudioComponent::pauseAll()
	{
		TDS::proxy_audio_system::ScriptPauseAll();
	}

	void AudioComponent::stop(System::String^ pathing)
	{
		TDS::proxy_audio_system::ScriptStop(toStdString(pathing));
	}

	void AudioComponent::stopAll()
	{
		TDS::proxy_audio_system::ScriptStopAll();
	}

	void AudioComponent::FadeOut(unsigned int duration, System::String^ pathing)
	{
		TDS::proxy_audio_system::ScriptFadeOut(duration, toStdString(pathing));
	}

	void AudioComponent::FadeIn(unsigned int duration, System::String^ pathing)
	{
		TDS::proxy_audio_system::ScriptFadeIn(duration, toStdString(pathing));
	}

	void AudioComponent::Queue(System::String^ str)
	{
		TDS::proxy_audio_system::Add_to_Queue(toStdString(str));
	}

	void AudioComponent::clearQueue()
	{
		TDS::proxy_audio_system::Clear_queue();
	}

	void AudioComponent::SetEnabled(bool enabled)
	{
		TDS::setComponentIsEnable("Audio", GetEntityID(), enabled);
	}
	bool AudioComponent::GetEnabled()
	{
		return TDS::getComponentIsEnable("Audio", GetEntityID());
	}

	//unique ID
	unsigned int AudioComponent::uniqueID::get()
	{
		return TDS::GetSoundInfo(entityID)->getUniqueID();
	}

	//MS Length
	unsigned int AudioComponent::MSLength::get()
	{
		return TDS::GetSoundInfo(entityID)->MSLength;
	}
	void AudioComponent::MSLength::set(unsigned int value)
	{
		TDS::GetSoundInfo(entityID)->MSLength = value;
	}

	//file path
	std::string AudioComponent::filePath::get()
	{
		return TDS::GetSoundInfo(entityID)->getFilePath();
	}
	void AudioComponent::filePath::set(std::string value)
	{
		TDS::GetSoundInfo(entityID)->setFilePath(value);
	}

	//loop
	/*bool AudioComponent::isLoop::get()
	{
		return TDS::GetSoundInfo(entityID)->isLoop;
	}
	void AudioComponent::isLoop::set(bool value)
	{
		TDS::GetSoundInfo(entityID)->isLoop = value;
	}*/

	//3D boolean
	bool AudioComponent::is3D::get()
	{
		return TDS::GetSoundInfo(entityID)->is3D;
	}
	void AudioComponent::is3D::set(bool value)
	{
		TDS::GetSoundInfo(entityID)->is3D = value;
	}

	//muted
	bool AudioComponent::isMuted::get()
	{
		return TDS::GetSoundInfo(entityID)->isMuted;
	}
	void AudioComponent::isMuted::set(bool value)
	{
		TDS::GetSoundInfo(entityID)->isMuted = value;
	}

	//state of sound info
	snd AudioComponent::whatState::get()
	{
		return TDS::GetSoundInfo(entityID)->getState();
	}
	void AudioComponent::whatState::set(snd value)
	{
		TDS::GetSoundInfo(entityID)->whatState = value;
	}

	//3D position
	Vector3 AudioComponent::pos::get()
	{
		return Vector3(TDS::GetSoundInfo(entityID)->get3DCoords());
	}
	void AudioComponent::pos::set(Vector3 value)
	{
		TDS::GetSoundInfo(entityID)->set3DCoords(value.X, value.Y, value.Z);
	}

	//volume
	float AudioComponent::volume::get()
	{
		return TDS::GetSoundInfo(entityID)->getVolume();
	}
	void AudioComponent::volume::set(float value)
	{
		TDS::GetSoundInfo(entityID)->setVol(value);
	}

	//Reverb
	float AudioComponent::ReverbAmount::get()
	{
		return TDS::GetSoundInfo(entityID)->getReverbAmount();
	}
	void AudioComponent::ReverbAmount::set(float value)
	{
		TDS::GetSoundInfo(entityID)->ReverbAmount = value;
	}
}