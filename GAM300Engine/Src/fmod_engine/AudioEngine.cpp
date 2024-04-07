/*!*************************************************************************
****
\file AudioEngine.cpp
\author w.chongsheng    
\par DP email: w.chongsheng@digipen.edu
\par Course: CSD3400
\par Section: a
\brief  Audio Engine
****************************************************************************
***/

#include <fmod/fmod_errors.h>
#include <iostream>
#include <fmod_engine/AudioEngine.h>
#include "../camera/camerasystem/CameraSystem.h"

namespace TDS
{
    namespace AudioWerks
    {
        AudioEngine* AudioEngine::audioE_instance = NULL;

        AudioEngine::AudioEngine() : sounds(), channels(), soundBanks(),
            eventDescriptions(), eventInstances(), reverb()
        {
            //std::cout << "AudioEngine Constructor" << '\n';
        }

        AudioEngine::~AudioEngine()
        {
            deactivate();
            delete audioE_instance;
            //std::cout << "AudioEngine Destructor" << '\n';
        }

        void AudioEngine::init()
        {
            //std::cout << "FMOD Engine Init" << '\n';
            ERRCHECK(FMOD::System_Create(&lowLevelSystem));
            ERRCHECK(lowLevelSystem->init(MAX_AUDIO_CHANNELS, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0));
            ERRCHECK(FMOD::Studio::System::create(&studioSystem));
            ERRCHECK(studioSystem->getCoreSystem(&lowLevelSystem));
            ERRCHECK(lowLevelSystem->setSoftwareFormat(AUDIO_SAMPLE_RATE, FMOD_SPEAKERMODE_5POINT1, 0));
            ERRCHECK(lowLevelSystem->set3DSettings(1.0, DISTANCEFACTOR, 0.5f));
            ERRCHECK(lowLevelSystem->set3DNumListeners(1));
            ERRCHECK(studioSystem->initialize(MAX_AUDIO_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));
            ERRCHECK(lowLevelSystem->createChannelGroup("BGM", &BGM));
            ERRCHECK(lowLevelSystem->createChannelGroup("SFX", &SFX));
            ERRCHECK(lowLevelSystem->getMasterChannelGroup(&mastergroup));
            initReverb();

            //std::cout << "Audio Engine Init successful!" << '\n';
        }

        void AudioEngine::deactivate()
        {
            ERRCHECK(studioSystem->release());
            ERRCHECK(lowLevelSystem->close());
            ERRCHECK(lowLevelSystem->release());
        }

        AudioEngine* AudioEngine::get_audioengine_instance()
        {            if (audioE_instance == NULL)
            {
                audioE_instance = new AudioEngine();
                audioE_instance->init();
                return audioE_instance;
            }

            return audioE_instance;
        }

        void AudioEngine::update()
        {
            ERRCHECK(studioSystem->update()); // also updates the low level system
        }

        bool AudioEngine::anySoundPlaying()
        {
            bool check{ false };
            
            for (std::pair<unsigned int, FMOD::Channel*> it : channels)
            {
                it.second->isPlaying(&check);

                if (check)
                {
                    break;
                }
            }

            return check;
        }

        void AudioEngine::loadSound(SoundInfo & soundInfo)
        {
            if (!soundLoaded(soundInfo)) {
                if (soundInfo.getFilePath().find("music") != std::string::npos)
                {
                    soundInfo.is3D = false;
                }
                else
                {
                    soundInfo.is3D = true;
                }

                FMOD::Sound* sound;
                ERRCHECK(lowLevelSystem->createSound(soundInfo.getFilePath_inChar(), soundInfo.is3D ? FMOD_3D : FMOD_2D, 0, &sound));
                ERRCHECK(sound->setMode(soundInfo.isLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));
                ERRCHECK(sound->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR));
                sounds.insert({ soundInfo.getUniqueID(), sound });

                size_t first = soundInfo.getFilePath().find_last_of('\\') + 1,
                    last = soundInfo.getFilePath().find_last_of('.') - first;
                std::string sound_name = soundInfo.getFilePath().substr(first, last);
                SoundInfo_Container.insert({ sound_name, &soundInfo });

                unsigned int msLength = 0;
                ERRCHECK(sounds[soundInfo.getUniqueID()]->getLength(&msLength, FMOD_TIMEUNIT_MS));
                soundInfo.MSLength = msLength;
                soundInfo.whatState = SOUND_LOADED;
            }
            //else
                //std::cout << "Audio Engine: Sound File was already loaded!\n";
        }

        void AudioEngine::unloadSound(std::string pathing)
        {
            SoundInfo* temp = findSound(pathing);

            if (soundLoaded(*temp))
            {
                sounds.erase(temp->uniqueID);

                size_t first = temp->getFilePath().find_last_of('\\') + 1,
                    last = temp->getFilePath().find_last_of('.') - first;
                std::string sound_name = temp->getFilePath().substr(first, last);
                SoundInfo_Container.erase(sound_name);

                channels.erase(temp->uniqueID);
                temp->whatState = SOUND_ERR;
            }
            else
            {
                //std::cout << "No such sound exists in the audio engine!\n";
            }
        }

        int AudioEngine::playSound(SoundInfo & soundInfo)
        {
            if (soundLoaded(soundInfo)) {
                if(!checkPlaying(soundInfo))
                {
                    if (!checkPaused(soundInfo))
                    {
                        //std::cout << "Playing Sound\n";
                        FMOD::Channel* channel{ nullptr };

                        //Find which group sound belongs to
                        FMOD::ChannelGroup* sort_channel{ nullptr };
                        if (soundInfo.getFilePath().find("Sound Effects") != std::string::npos)
                        {
                            sort_channel = SFX;
                        }
                        else if (soundInfo.getFilePath().find("music") != std::string::npos)
                        {
                            sort_channel = BGM;
                        }

                        // start play in 'paused' state
                        ERRCHECK(lowLevelSystem->playSound(sounds[soundInfo.getUniqueID()], sort_channel, true /* start paused */, &channel));

                        if (soundInfo.is3D)
                            set3dChannelPosition(soundInfo, channel);

                        //std::cout << "Playing sound at volume " << soundInfo.getVolume() << '\n';
                        channel->setVolume(soundInfo.getVolume());

                        channels[soundInfo.getUniqueID()] = channel;

                        ERRCHECK(channel->setReverbProperties(0, soundInfo.getReverbAmount()));

                        // start audio playback
                        ERRCHECK(channel->setPaused(false));
                        soundInfo.whatState = SOUND_LOADED;
                    }
                    else
                    {
                        ERRCHECK(channels[soundInfo.getUniqueID()]->setPaused(false));
                        soundInfo.whatState = SOUND_LOADED;
                    }
                }
                else
                {
                    //std::cout << "Sound is already playing!" << std::endl;
                }
            }
            else
            {
                //std::cout << "Sound not loaded, loading in now." << std::endl;
                loadSound(soundInfo);
                return playSound(soundInfo);
            }

        }

        void AudioEngine::playAllPaused()
        {
            bool check{ false };
            
            for (std::pair<unsigned int, FMOD::Channel*> it : channels)
            {
                it.second->getPaused(&check);
                if (check)
                {
                    it.second->setPaused(false);
                }
            }
        }

        void AudioEngine::pauseSound(SoundInfo& soundInfo)
        {
            if (checkPlaying(soundInfo))
            {
                ERRCHECK(channels[soundInfo.getUniqueID()]->setPaused(true));
            }
            else
            {
                //std::cout << "Nothing is playing!" << std::endl;
            }
        }

        void AudioEngine::pauseAllSound()
        {
            bool check{ false };

            for (auto& ch : channels)
            {
                ERRCHECK(ch.second->isPlaying(&check));

                if (check)
                {
                    ch.second->setPaused(true);
                }
            }
        }

        void AudioEngine::stopSound(SoundInfo & soundInfo)
        {
            if (checkPlaying(soundInfo))
            {
                ERRCHECK(channels[soundInfo.getUniqueID()]->stop());
                //loopsPlaying.erase(soundInfo.getUniqueID());
                soundInfo.whatState = SOUND_LOADED; //set the sound back to loaded state
                channels.erase(soundInfo.getUniqueID());
                //std::cout << "Stopping sound" << std::endl;
            }
            else
            {
                //std::cout << "Audio Engine: Can't stop a sound that's not playing!\n";
                //std::cout << "No sound is playing" << std::endl;
            }
        }

        void AudioEngine::stopAllSound()
        {
            bool check{ false };

            for (auto& ch : channels)
            {
                ERRCHECK(ch.second->isPlaying(&check));

                if (check)
                {
                    ch.second->stop();
                    channels.erase(ch.first);
                }
            }
        }

        void AudioEngine::SetGlobalVolume(float vol)
        {
            vol /= 100.f;
            vol = Mathf::Clamp(vol, 0.f, 1.f);
            
            mastergroup->setPaused(true);
            mastergroup->setVolume(vol);
            mastergroup->setPaused(false);
        }

        float AudioEngine::getGlobalVolume()
        {
            float vol = 0.f;
            mastergroup->getVolume(&vol);

            return vol * 100.f;
        }

        void AudioEngine::SetChannelGroupVolume(char tag, float vol)
        {
            vol /= 100.f;
            vol = Mathf::Clamp(vol, 0.f, 1.f);

            switch (tag)
            {
                case 'S':
                {
                    SFX->setPaused(true);
                    SFX->setVolume(vol);
                    SFX->setPaused(false);
                    break;
                }
                case 'B':
                {
                    BGM->setPaused(true);
                    BGM->setVolume(vol);
                    BGM->setPaused(false);
                    break;
                }
                default:
                {
                    //std::cout << "No channelgroup chosen" << std::endl;
                }
            }
        }

        float AudioEngine::getChannelGroupVolume(char tag)
        {
            float vol = 0.f;

            switch (tag)
            {
                case 'S':
                {
                    SFX->getVolume(&vol);
                    break;
                }
                case 'B':
                {
                    BGM->getVolume(&vol);
                    break;
                }
                default:
                {
                    //std::cout << "No channelgroup chosen" << std::endl;
                }
            }
            
            return vol * 100.f;
        }

        void AudioEngine::SetSoundVolume(float vol, SoundInfo& soundInfo)
        {
            soundInfo.setVol(vol);

            channels[soundInfo.getUniqueID()]->setPaused(true);
            channels[soundInfo.getUniqueID()]->setVolume(vol);
            channels[soundInfo.getUniqueID()]->setPaused(false);
        }

        float AudioEngine::GetSoundVolume(SoundInfo& soundInfo)
        {
            float volume{ 0.f };
            channels[soundInfo.getUniqueID()]->getVolume(&volume);

            return volume;
        }

        void AudioEngine::FadeOutSound(unsigned int duration, SoundInfo& soundInfo)
        {
            unsigned long long DSPClock{ 0 };
            int rate{ 0 };
            ERRCHECK(lowLevelSystem->getSoftwareFormat(&rate, 0, 0));
            
            ERRCHECK(channels[soundInfo.uniqueID]->getDSPClock(0, &DSPClock));
            ERRCHECK(channels[soundInfo.uniqueID]->addFadePoint(DSPClock, soundInfo.volume));
            ERRCHECK(channels[soundInfo.uniqueID]->addFadePoint(DSPClock + (rate * duration), 0.f));
            ERRCHECK(channels[soundInfo.uniqueID]->setDelay(0, DSPClock + (rate * duration), true));
        }

        void AudioEngine::FadeInSound(unsigned int duration, SoundInfo& soundInfo)
        {
            playSound(soundInfo);
            
            unsigned long long DSPClock{ 0 };
            int rate{ 0 };
            ERRCHECK(lowLevelSystem->getSoftwareFormat(&rate, 0, 0));

            ERRCHECK(channels[soundInfo.uniqueID]->setPaused(true));
            ERRCHECK(channels[soundInfo.uniqueID]->getDSPClock(0, &DSPClock));
            ERRCHECK(channels[soundInfo.uniqueID]->addFadePoint(DSPClock, 0.f));
            ERRCHECK(channels[soundInfo.uniqueID]->addFadePoint(DSPClock + (rate * duration), soundInfo.volume));
            ERRCHECK(channels[soundInfo.uniqueID]->setPaused(false));
        }

        void AudioEngine::updateSoundLoopVolume(SoundInfo & soundInfo, float newVolume, unsigned int fadeSampleLength)
        {
            if (checkPlaying(soundInfo) && soundInfo.isLoop) {
                FMOD::Channel* channel = channels[soundInfo.getUniqueID()];
                if (fadeSampleLength <= 64) // 64 samples is default volume fade out
                    ERRCHECK(channel->setVolume(newVolume));
                else {
                    bool fadeUp = newVolume > soundInfo.getVolume();
                    // get current audio clock time
                    unsigned long long parentclock = 0;
                    ERRCHECK(channel->getDSPClock(NULL, &parentclock));

                    float targetFadeVol = fadeUp ? 1.0f : newVolume;

                    if (fadeUp) ERRCHECK(channel->setVolume(newVolume));

                    ERRCHECK(channel->addFadePoint(parentclock, soundInfo.getVolume()));
                    ERRCHECK(channel->addFadePoint(parentclock + fadeSampleLength, targetFadeVol));
                    //std::cout << "Current DSP Clock: " << parentclock << ", fade length in samples  = " << fadeSampleLength << "\n";
                }
                //std::cout << "Updating with new soundinfo vol \n";
                soundInfo.setVol(newVolume); // update the SoundInfo's volume
            }
            else
                std::cout << "AudioEngine: Can't update sound loop volume! (It isn't playing or might not be loaded)\n";
        }


        void AudioEngine::update3DSoundPosition(Vec3 pos, SoundInfo& soundInfo)
        {
            soundInfo.position = pos;

            set3dChannelPosition(soundInfo, channels[soundInfo.uniqueID]);
        }

        bool AudioEngine::checkPlaying(SoundInfo& soundInfo)
        {
            bool playing{ false }, pause{ false };

            channels[soundInfo.getUniqueID()]->isPlaying(&playing);
            channels[soundInfo.getUniqueID()]->getPaused(&pause);

            return (playing ^ pause);
        }

        bool AudioEngine::checkPaused(SoundInfo& soundInfo)
        {
            bool check{ false };

            channels[soundInfo.getUniqueID()]->getPaused(&check);

            return check;
        }

        bool AudioEngine::soundFinished(SoundInfo& soundInfo)
        {
            bool check{ false };

            channels[soundInfo.getUniqueID()]->isPlaying(&check);

            if (!check)
            {
                soundInfo.whatState = SOUND_LOADED;
                channels.erase(soundInfo.uniqueID);
            }

            return !check;
        }

        void AudioEngine::set3DListenerPosition(float posX, float posY, float posZ, float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ)
        {
            listenerpos = { posX,     posY,     posZ };
            forward = { forwardX, forwardY, forwardZ };
            up = { upX,      upY,      upZ };

            ERRCHECK(lowLevelSystem->set3DListenerAttributes(0, &listenerpos, 0, &forward, &up));
        }

        void AudioEngine::get3DListenerCharacteristics(float& posX, float& posY, float& posZ,
            float& velX, float& velY, float& velZ, float& forX, float& forY, float& forZ,
            float& upX, float& upY, float& upZ)
        {
            FMOD_VECTOR fpos{posX, posY, posZ},
                fvel{velX, velY, velZ},
                ffor{forX, forY, forZ},
                fup{ upX, upY, upZ };
            ERRCHECK(lowLevelSystem->get3DListenerAttributes(0, &fpos, &fvel, &ffor, &fup));

            posX = fpos.x; posY = fpos.y; posZ = fpos.z;
            velX = fvel.x; velY = fvel.y; velZ = fvel.z;
            forX = ffor.x; forY = ffor.y; forZ = ffor.z;
            upX = fup.x; upY = fup.y; upZ = fup.z;
        }

        void AudioEngine::get3DListenerCharacteristics(Vec3& pos, Vec3& vel, Vec3& forward, Vec3& upVec)
        {
            FMOD_VECTOR fpos{ pos.x, pos.y, pos.z },
                fvel{ vel.x, vel.y, vel.z },
                ffor{ forward.x, forward.y, forward.z },
                fup{ upVec.x, upVec.y, upVec.z };
            ERRCHECK(lowLevelSystem->get3DListenerAttributes(0, &fpos, &fvel, &ffor, &fup));

            pos.x = fpos.x; pos.y = fpos.y; pos.z = fpos.z;
            vel.x = fvel.x; vel.y = fvel.y; vel.z = fvel.z;
            forward.x = ffor.x; forward.y = ffor.y; forward.z = ffor.z;
            upVec.x = fup.x; upVec.y = fup.y; upVec.z = fup.z;
        }

        unsigned int AudioEngine::getSoundLengthInMS(SoundInfo soundInfo)
        {
            unsigned int length = 0;
            if (sounds.count(soundInfo.getUniqueID()))
                ERRCHECK(sounds[soundInfo.getUniqueID()]->getLength(&length, FMOD_TIMEUNIT_MS));
            return length;
        }

        void AudioEngine::loadFMODStudioBank(const char* filepath)
        {
            //std::cout << "Audio Engine: Loading FMOD Studio Sound Bank " << filepath << '\n';
            FMOD::Studio::Bank* bank = NULL;
            ERRCHECK(studioSystem->loadBankFile(filepath, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank));
            soundBanks.insert({ filepath, bank });
        }

        void AudioEngine::loadFMODStudioEvent(const char* eventName, std::vector<std::pair<const char*, float>> paramsValues)
        { //std::vector<std::map<const char*, float>> perInstanceParameterValues) {
            //std::cout << "AudioEngine: Loading FMOD Studio Event " << eventName << '\n';
            FMOD::Studio::EventDescription* eventDescription = NULL;
            ERRCHECK(studioSystem->getEvent(eventName, &eventDescription));
            // Create an instance of the event
            FMOD::Studio::EventInstance* eventInstance = NULL;
            ERRCHECK(eventDescription->createInstance(&eventInstance));
            for (const auto& parVal : paramsValues) {
                //std::cout << "AudioEngine: Setting Event Instance Parameter " << parVal.first << "to value: " << parVal.second << '\n';
                // Set the parameter values of the event instance
                ERRCHECK(eventInstance->setParameterByName(parVal.first, parVal.second));
            }
            eventInstances.insert({ eventName, eventInstance });
            eventDescriptions.insert({ eventName, eventDescription });
        }

        void AudioEngine::setFMODEventParamValue(const char* eventName, const char* parameterName, float value)
        {
            if (eventInstances.count(eventName) > 0)
                ERRCHECK(eventInstances[eventName]->setParameterByName(parameterName, value));
            else
                std::cout << "AudioEngine: Event " << eventName << " was not in event instance cache, can't set param \n";

        }

        void AudioEngine::playEvent(const char* eventName, int instanceIndex)
        {
            (void)instanceIndex;//TODO
            printEventInfo(eventDescriptions[eventName]);
            //auto eventInstance = eventInstances[eventName];
            if (eventInstances.count(eventName) > 0)
                ERRCHECK(eventInstances[eventName]->start());
            else
                std::cout << "AudioEngine: Event " << eventName << " was not in event instance cache, cannot play \n";
        }

        void AudioEngine::stopEvent(const char* eventName, int instanceIndex)
        {
            (void)instanceIndex;//TODO
            if (eventInstances.count(eventName) > 0)
                ERRCHECK(eventInstances[eventName]->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT));
            else
                std::cout << "AudioEngine: Event " << eventName << " was not in event instance cache, cannot stop \n";
        }

        void AudioEngine::setEventVolume(const char* eventName, float volume0to1)
        {
            std::cout << "AudioEngine: Setting Event Volume\n";
            ERRCHECK(eventInstances[eventName]->setVolume(volume0to1));
        }

        bool AudioEngine::eventIsPlaying(const char* eventName, int instance /*= 0*/)
        {
            (void)instance;
            FMOD_STUDIO_PLAYBACK_STATE playbackState;
            ERRCHECK(eventInstances[eventName]->getPlaybackState(&playbackState));
            return playbackState == FMOD_STUDIO_PLAYBACK_PLAYING;
        }

        void AudioEngine::mute(SoundInfo& soundInfo)
        {
            if (checkPlaying(soundInfo))
            {
                if (soundInfo.isLoop)
                {
                    ERRCHECK(channels[soundInfo.getUniqueID()]->setMute(true));
                }

                soundInfo.isMuted = true;
            }
        }

        void AudioEngine::unmute(SoundInfo& soundInfo)
        {
            if (soundInfo.isMuted)
            {
                if (soundInfo.isLoop)
                {
                    ERRCHECK(channels[soundInfo.getUniqueID()]->setMute(false));
                }

                soundInfo.isMuted = false;
            }
        }

        void AudioEngine::muteAllSounds()
        {
            ERRCHECK(mastergroup->setMute(true));
            muted = true;
        }

        void AudioEngine::unmuteAllSound()
{
            ERRCHECK(mastergroup->setMute(false));
            muted = false;
        }

        bool AudioEngine::isMuted()
        {
            return muted;
        }

        void AudioEngine::setLoop(SoundInfo& soundInfo, bool set)
        {
            soundInfo.isLoop = set;

            if (soundInfo.isLoop)
            {
                ERRCHECK(channels[soundInfo.uniqueID]->setMode(FMOD_LOOP_NORMAL));
            }
            else
            {
                ERRCHECK(channels[soundInfo.uniqueID]->setMode(FMOD_LOOP_OFF));
            }
        }

        bool AudioEngine::GetLoop(SoundInfo& soundInfo)
        {
            return soundInfo.isLoop;
        }

        std::map<unsigned int, FMOD::Sound*> AudioEngine::getSoundContainer()
        {
            return sounds;
        }

        std::map<std::string, SoundInfo> AudioEngine::getChannelInfoContainer()
        {
            std::map<std::string, SoundInfo> temp;

            for (auto& it : channels)
            {
                temp[findSound(it.first)->getFilePath()] = *findSound(it.first);
            }

            return temp;
        }

        int AudioEngine::GetAmountOfChannelsPlaying()
        {
            return channels.size();
        }

        std::map<std::string, FMOD::Studio::Bank*> AudioEngine::getBankContainer()
        {
            return soundBanks;
        }

        std::map<std::string, FMOD::Studio::EventDescription*> AudioEngine::getEventDescriptionContainer()
        {
            return eventDescriptions;
        }

        std::map<std::string, FMOD::Studio::EventInstance*> AudioEngine::getEventInstanceContainer()
        {
            return eventInstances;
        }

        SoundInfo* AudioEngine::findSound(std::string name)
        {
            return SoundInfo_Container[name];
        }

        SoundInfo* AudioEngine::findSound(unsigned int ID)
        {
            for (auto& it : SoundInfo_Container)
            {
                if (it.second->uniqueID == ID)
                {
                    return it.second;
                }
            }

            return nullptr;
        }

        //// Private definitions 
        bool AudioEngine::soundLoaded(SoundInfo soundInfo)
        {
            //std::cout << "Checking sound " << soundInfo.getUniqueID() << " exists\n";
            return (sounds.count(soundInfo.getUniqueID()) > 0);
        }

        void AudioEngine::set3dChannelPosition(SoundInfo& soundInfo, FMOD::Channel * channel)
        {
            FMOD_VECTOR position = { soundInfo.getX(), soundInfo.getY(), soundInfo.getZ() };
            FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f }; // TODO Add dopplar (velocity) support
            ERRCHECK(channel->set3DAttributes(&position, &velocity));
        }

        void AudioEngine::initReverb()
        {
            ERRCHECK(lowLevelSystem->createReverb3D(&reverb));
            FMOD_REVERB_PROPERTIES prop2 = FMOD_PRESET_CONCERTHALL;
            ERRCHECK(reverb->setProperties(&prop2));
            ERRCHECK(reverb->set3DAttributes(&revPos, revMinDist, revMaxDist));
        }

        // Error checking/debugging function definitions

        void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line)
        {
            //(void)file;//TODO
            //if (result != FMOD_OK)
            //    std::cout << "FMOD ERROR: AudioEngine.cpp [Line " << line << "] " << result << "  - " << FMOD_ErrorString(result) << '\n';
        }

        void AudioEngine::printEventInfo(FMOD::Studio::EventDescription * eventDescription)
        {

            int params;
            bool is3D, isOneshot;
            ERRCHECK(eventDescription->getParameterDescriptionCount(&params));
            ERRCHECK(eventDescription->is3D(&is3D));
            ERRCHECK(eventDescription->isOneshot(&isOneshot));

            /*std::cout << "FMOD EventDescription has " << params << " parameter descriptions, "
                << (is3D ? " is " : " isn't ") << " 3D, "
                << (isOneshot ? " is " : " isn't ") << " oneshot, "
                << (eventDescription->isValid() ? " is " : " isn't ") << " valid."
                << '\n';*/
        }
    } //end of AudioWerks

    AudioWerks::AudioEngine* proxy_audio_system::aud_instance = nullptr;
    int proxy_audio_system::totalNumClips{ 0 };
    bool proxy_audio_system::Q_state{ false };
    SoundInfo proxy_audio_system::Q_name{};

    std::map<std::string, SoundInfo> proxy_audio_system::allSounds;
    std::map<std::string, std::pair<bool, SoundInfo*>> proxy_audio_system::Queue;
    //std::map<unsigned int, std::map<Vec3*, SOUND_STATE*>> sound_events{};

    void proxy_audio_system::audio_system_init()
    {
        aud_instance = AudioWerks::AudioEngine::get_audioengine_instance();
        totalNumClips = aud_instance->getSoundContainer().size();

        allSounds.clear();
        Queue.clear();

        load_all_audio_files();
    }

    void proxy_audio_system::audio_system_update(const float dt, const std::vector<EntityID>& entities, Transform* soundInfo)
    {
        aud_instance->update();
        Play_queue();
    }

    void proxy_audio_system::audio_event_init(SoundInfo* container)
    {
        //sound_events[container->getUniqueID()] = container->getEvents();
    }

    void proxy_audio_system::audio_event_update()
    {
    }

    void proxy_audio_system::load_all_audio_files()
    {
        std::filesystem::path full_path = "../assets/audioFiles/"; //pathing / append
        std::vector<std::filesystem::path> all_files; //store all file path

        if (std::filesystem::is_directory(full_path))
        {
            all_files = go_deeper(full_path);
        }

        for (auto& str : all_files)
        {
            if (str.string().find(".txt") == std::string::npos && str.string().find(".meta") == std::string::npos)
            {
                SoundInfo temp(str.string());
                size_t first = str.string().find_last_of('\\') + 1,
                    last = str.string().find_last_of('.') - first;
                std::string sound_name = str.string().substr(first, last);

                allSounds[sound_name] = (temp);
                aud_instance->loadSound(temp);
            }
        }
    }

    std::vector<std::filesystem::path> proxy_audio_system::go_deeper(std::filesystem::path f_path)
    {
        std::vector<std::filesystem::path> folders;
        std::vector<std::filesystem::path> files;

        for (auto& temp : std::filesystem::directory_iterator(f_path))
        {
            if (temp.is_directory())
            {
                folders = go_deeper(temp);

                for (auto& temp2 : folders)
                {
                    files.push_back(temp2);
                }
            }
            else
            {
                files.push_back(temp);
            }
        }

        return files;
    }

    void proxy_audio_system::ScriptPlay(std::string pathing)
    {
        if (find_sound_info(pathing) == nullptr)
        {
            ScriptLoad(pathing);
        }
        aud_instance->playSound(*find_sound_info(pathing));
    }

    void proxy_audio_system::ScriptPause(std::string pathing)
    {
        aud_instance->pauseSound(*find_sound_info(pathing));
    }

    void proxy_audio_system::ScriptStop(std::string pathing)
    {
        aud_instance->stopSound(*find_sound_info(pathing));
    }

    void proxy_audio_system::ScriptLoad(std::string pathing)
    {
        SoundInfo temp(pathing);

        if (allSounds.find(pathing) != allSounds.end())
        {
            allSounds[pathing] = temp;
            aud_instance->loadSound(temp);
        }
        else
        {
            //std::cout << "Sound already loaded!\n" << std::endl;
        }
    }

    void proxy_audio_system::ScriptUnload(std::string pathing)
    {
        aud_instance->unloadSound(pathing);
    }

    SoundInfo* proxy_audio_system::ScriptGetSound(std::string pathing)
    {
        return aud_instance->findSound(pathing);
    }

    unsigned int proxy_audio_system::ScriptGetID(std::string pathing)
    {
        return aud_instance->findSound(pathing)->uniqueID;
    }

    bool proxy_audio_system::ScriptGetLoop(std::string pathing)
    {
        return aud_instance->GetLoop(*find_sound_info(pathing));
    }

    bool proxy_audio_system::CheckPlaying(std::string pathing)
    {
        return aud_instance->checkPlaying(*find_sound_info(pathing));
    }

    bool proxy_audio_system::CheckPause(std::string pathing)
    {
        return aud_instance->checkPaused(*find_sound_info(pathing));
    }

    void proxy_audio_system::ScriptPlayAllPaused()
    {
        aud_instance->playAllPaused();
    }

    void proxy_audio_system::ScriptPauseAll()
    {
        aud_instance->pauseAllSound();
    }

    void proxy_audio_system::ScriptStopAll()
    {
        for (auto& temp : allSounds)
        {
            aud_instance->stopSound(temp.second);
        }
    }

    bool proxy_audio_system::ScriptAnySoundPlaying()
    {
        return aud_instance->anySoundPlaying();
    }

    void proxy_audio_system::ScriptFadeOut(unsigned int duration, std::string pathing)
    {
        if (allSounds.find(pathing) != allSounds.end())
        {
            aud_instance->FadeOutSound(duration, allSounds[pathing]);
        }
    }

    void proxy_audio_system::ScriptFadeIn(unsigned int duration, std::string pathing)
    {
        if (allSounds.find(pathing) != allSounds.end())
        {
            aud_instance->FadeInSound(duration, allSounds[pathing]);
        }
    }

    void proxy_audio_system::ScriptSetPosition(Vec3 pos, std::string pathing)
    {
        SoundInfo* temp = find_sound_info(pathing);

        aud_instance->update3DSoundPosition(pos, *temp);
    }

    void proxy_audio_system::ScriptSetListenerPos(Vec3 pos, Vec3 for_vec, Vec3 up_vec)
    {        
        aud_instance->set3DListenerPosition(pos.x, pos.y, pos.z,
            for_vec.x, for_vec.y, for_vec.z,
            up_vec.x, up_vec.y, up_vec.z);
    }

    void proxy_audio_system::ScriptSetLoop(bool set, std::string pathing)
    {
        aud_instance->setLoop(*find_sound_info(pathing), set);
    }

    SoundInfo* proxy_audio_system::find_sound_info(std::string str)
    {
        for (auto& temp : allSounds)
        {
            if (temp.first == str)
            {
                return &temp.second;
            }
        }

        return nullptr;
    }

    void proxy_audio_system::Add_to_Queue(std::string str)
    {
        Queue[str] = std::make_pair(true, find_sound_info(str));
    }

    void proxy_audio_system::Remove_from_Queue(std::string str)
    {
        Queue.erase(str);
    }

    void proxy_audio_system::Play_queue()
    {       
        if(!Queue.empty())
        {
            if (Queue.begin()->second.first)
            {
                Queue.begin()->second.first = false;
                aud_instance->playSound(*Queue.begin()->second.second);
            }
            else if (checkifdone(Queue.begin()->first))
            {
                Queue.erase(Queue.begin()->first);
            }
        }
    }

    void proxy_audio_system::Clear_queue()
    {
        Queue.clear();
    }

    float proxy_audio_system::getVolume(std::string pathing)
    {
        return aud_instance->GetSoundVolume(*find_sound_info(pathing));
    }

    float proxy_audio_system::getMasterVolume()
    {
        return aud_instance->getGlobalVolume();
    }

    float proxy_audio_system::getBGMVolume()
    {
        return aud_instance->getChannelGroupVolume('B');
    }

    float proxy_audio_system::getSFXVolume()
    {
        return aud_instance->getChannelGroupVolume('S');
    }

    Vec3 proxy_audio_system::getListenerPos()
    {
        Vec3 temppos, tempvel, tempfor, tempup;
        aud_instance->get3DListenerCharacteristics(temppos, tempvel, tempfor, tempup);

        return temppos;
    }

    void proxy_audio_system::SetVolume(float vol, std::string pathing)
    {
        aud_instance->SetSoundVolume(vol, *find_sound_info(pathing));
    }

    void proxy_audio_system::SetMasterVolume(float vol)
    {
        aud_instance->SetGlobalVolume(vol);
    }

    void proxy_audio_system::SetBGMVolume(float vol)
    {
        aud_instance->SetChannelGroupVolume('B', vol);
    }

    void proxy_audio_system::SetSFXVolume(float vol)
    {
        aud_instance->SetChannelGroupVolume('S', vol);
    }

    bool proxy_audio_system::checkifdone(std::string str)
    {
        bool check{ false };

        for (auto& temp : allSounds)
        {
            if (strstr(temp.first.c_str(), str.c_str()))
            {
                check = aud_instance->soundFinished(temp.second);
            }
        }

        return check;
    }
    
    /*void proxy_audio_system::audio_event_play(SoundInfo& soundInfo)
    {
        aud_instance->playSound(soundInfo);
    }*/
} //end of TDS