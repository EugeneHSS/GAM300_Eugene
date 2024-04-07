/*!*************************************************************************
****
\file AudioEngine.h
\author w.chongsheng
\par DP email: w.chongsheng@digipen.edu
\par Course: CSD3400
\par Section: a
\brief  Audio Engine
****************************************************************************
***/

#ifndef AUDIOENGINE_H_
#define AUDIOENGINE_H_

#include <fmod/fmod_studio.hpp>
#include <fmod/fmod.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <filesystem>
#include "components/SoundInfo.h"
#include "dotnet/ImportExport.h"

#include "ecs/ecs.h"
#include "components/components.h"

namespace TDS
{
    namespace AudioWerks
    {
        /**
         * Error Handling Function for FMOD Errors
         * @param result - the FMOD_RESULT generated during every FMOD
         */
        void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

        /**
         * Class that handles the process of loading and playing sounds by wrapping FMOD's functionality.
         * Deals with all FMOD calls so that FMOD-specific code does not need to be used outside this class.
         * Only one AudioEngine should be constructed for an application.
         */
        class AudioEngine
        {
        public:
            /**
             * Remove AudioEngine copy constructor.
             * It is the ensure singleton
             */
            DLL_API  AudioEngine(const AudioEngine& rhs) = delete;

            /**
            * Destructor for Audio Engine
            */
            DLL_API  ~AudioEngine();

            /**
             * Remove AudioEngine operator= copy constructor.
             * It is the ensure singleton
             */
            DLL_API  void operator=(const AudioEngine& rhs) = delete;

            /**
             * Initializes Audio Engine Studio and Core systems to default values.
             */
            DLL_API void init();

            /**
             * Method that is called to deactivate the audio engine after use.
             */
            DLL_API  void deactivate();

            /**
             * Returns a pointer to an instance of AudioEngine.
             */
            DLL_API  static AudioEngine* get_audioengine_instance();

            /**
            * Creates a unique ptr for anyone who wants to use audio engine
            */
            //DLL_API  std::unique_ptr<AudioEngine> get_audioengine_unique(std::unique_ptr<AudioEngine> ptr);

            /**
            * Method which should be called every frame of the game loop
            */
            DLL_API  void update();

            /**
            * Checks if any sound is playing
            */
            DLL_API  bool anySoundPlaying();

            /**
             * Loads a sound from disk using provided settings
             * Prepares for later playback with playSound()
             * Only reads the audio file and loads into the audio engine
             * if the sound file has already been added to the cache
             */
            DLL_API  void loadSound(SoundInfo& soundInfo);

            /**
             * Unloads a sound from container
             */
            DLL_API  void unloadSound(std::string pating);

            /**
            * Plays a sound file using FMOD's low level audio system. If the sound file has not been
            * previously loaded using loadSoundFile(), a console message is displayed
            *
            * @param filename - relative path to file from project directory. (Can be .OGG, .WAV, .MP3,
            *                 or any other FMOD-supported audio format)
            */
            DLL_API  int playSound(SoundInfo& soundInfo);

            /**
            * Plays all sound in paused state
            */
            DLL_API  void playAllPaused();

            /**
            * Pause a sound file using FMOD's low level audio system.
            *
            * @param filename - relative path to file from project directory. (Can be .OGG, .WAV, .MP3,
            *                 or any other FMOD-supported audio format)
            */
            DLL_API void pauseSound(SoundInfo& soundInfo);

            /**
            * Pause all sounds
            */
            DLL_API  void pauseAllSound();

            /**
             * Stops a sound if it's currently playing.
             */
            DLL_API  void stopSound(SoundInfo& soundInfo);

            /**
            * Stops all sound
            */
            DLL_API  void stopAllSound();

            /**
             * Set global volume
             * Range 0 - 100
             */
            DLL_API  void SetGlobalVolume(float vol);

            /**
             * Get global volume
             * Range of 0 - 100
            */
            DLL_API  float getGlobalVolume();

            /**
             * Set channelgroup volume
             * Range 0 - 100
             */
            DLL_API  void SetChannelGroupVolume(char tag, float vol);

            /**
             * Get channelgroup volume
             * Range of 0 - 100
            */
            DLL_API  float getChannelGroupVolume(char tag);

            /**
             * Set specific sound volume
             */
            DLL_API  void SetSoundVolume(float vol, SoundInfo& soundInfo);

            /**
             * Get specific sound volume
             */
            DLL_API  float GetSoundVolume(SoundInfo& soundInfo);

            /**
            * Fades out sound to a stop
            */
            DLL_API  void FadeOutSound(unsigned int duration, SoundInfo& soundInfo);

            /**
            * Fade in sound
            */
            DLL_API  void FadeInSound(unsigned int duration, SoundInfo& soundInfo);

            /**
             * Method that updates the volume of a soundloop that is playing. This can be used to create audio 'fades'
             * where the volume ramps up or down to the provided new volume
             * @param fadeSampleLength the length in samples of the intended volume sample. If less than 64 samples, the default
             *                         FMOD fade out is used
             */
            DLL_API  void updateSoundLoopVolume(SoundInfo& soundInfo, float newVolume, unsigned int fadeSampleLength = 0);


            /**
            * Updates the position of a looping 3D sound that has already been loaded and is playing back.
            * The SoundInfo object's position coordinates will be used for the new sound position, so
            * SoundInfo::set3DCoords(x,y,z) should be called before this method to set the new desired location.
            */
            DLL_API  void update3DSoundPosition(Vec3 pos, SoundInfo& soundInfo);

            /**
             * Checks if a sound is playing.
             */
            DLL_API  bool checkPlaying(SoundInfo& soundInfo);

            /**
            * Checks if a sound is paused
            */
            DLL_API  bool checkPaused(SoundInfo& soundInfo);

            /**
             * Checks if a sound has finished playing.
             */
            DLL_API  bool soundFinished(SoundInfo& soudnInfo);

            /**
             * Sets the position of the listener in the 3D scene.
             * @param posX, posY, posZ - 3D translation of listener
             * @param forwardX, forwardY, forwardZ - forward angle character is looking in
             * @param upX, upY, upZ - up which must be perpendicular to forward vector
             */
            DLL_API  void set3DListenerPosition(float posX, float posY, float posZ,
                float forwardX, float forwardY, float forwardZ,
                float upX, float upY, float upZ);

            /**
             * Get the values of the listener
             * @param float pos - position of the listener
             * @param float velocity - speed of the movement
             * @param float forward - forward angle character is looking in
             * @param float up - up vector that is perpendicular to forward vector 
            */
            DLL_API  void get3DListenerCharacteristics(float& posX, float& posY, float& posZ,
                float& velX, float& velY, float& velZ, float& forX, float& forY, float& forZ,
                float& upX, float& upY, float& upZ);

            /**
             * Get the values of the listener
             * @param Vector pos - position of the listener
             * @param Vector velocity - speed of the movement
             * @param Vector forward - forward angle character is looking in
             * @param Vector up - up vector that is perpendicular to forward vector
            */
            DLL_API  void get3DListenerCharacteristics(Vec3& pos, Vec3& vel, Vec3& forward, Vec3& upVec);

            /**
            * Utility method that returns the length of a SoundInfo's audio file in milliseconds
            * If the sound hasn't been loaded, returns 0
            */
            DLL_API  unsigned int getSoundLengthInMS(SoundInfo soundInfo);

            /**
             * Loads an FMOD Studio soundbank
             * TODO Fix
             */
            DLL_API   void loadFMODStudioBank(const char* filePath);

            /**
             * Loads an FMOD Studio Event. The Soundbank that this event is in must have been loaded before
             * calling this method.
             * TODO Fix
             */
            DLL_API   void loadFMODStudioEvent(const char* eventName, std::vector<std::pair<const char*, float>> paramsValues = { });

            /**
             * Sets the parameter of an FMOD Soundbank Event Instance.
             */
            DLL_API  void setFMODEventParamValue(const char* eventName, const char* parameterName, float value);

            /**
             * Plays the specified instance of an event
             * TODO support playback of multiple event instances
             * TODO Fix playback
             */
            DLL_API  void playEvent(const char* eventName, int instanceIndex = 0);

            /**
             * Stops the specified instance of an event, if it is playing.
             */
            DLL_API   void stopEvent(const char* eventName, int instanceIndex = 0);

            /**
             * Sets the volume of an event.
             * @param volume0to1 - volume of the event, from 0 (min vol) to 1 (max vol)
             */
            DLL_API  void setEventVolume(const char* eventName, float volume0to1 = .75f);

            /**
             * Checks if an event is playing.
             */
            DLL_API  bool eventIsPlaying(const char* eventName, int instance = 0);

            /**
            * Mute a sound
            */
            DLL_API void mute(SoundInfo& soundInfo);

            /**
            * Unmute a sound
            */
            DLL_API void unmute(SoundInfo& soundInfo);

            /**
             * Mutes all sounds for the audio engine
             */
            DLL_API  void muteAllSounds();

            /**
             * Unmutes all sounds for the audio engine
             */
            DLL_API   void unmuteAllSound();

            /**
             * Returns true if the audio engine is muted, false if not
             */
            DLL_API  bool isMuted();

            /**
             * Set loop for a sound
             */
            DLL_API  void setLoop(SoundInfo& soundInfo, bool set);

            /**
             * Returns if sound is looping
             */
            DLL_API  bool GetLoop(SoundInfo& soundInfo);

            /**
             * Get container of sounds that's loaded
             */
            DLL_API  std::map<unsigned int, FMOD::Sound*> getSoundContainer();

            /**
             * Get container of channels and the sound name together
             */
            DLL_API  std::map<std::string, SoundInfo> getChannelInfoContainer();

            /**
             * Get the amount of channels that's playing
            */
            DLL_API  int GetAmountOfChannelsPlaying();

            /**
             * Get container of banks that's loaded
             */
            DLL_API  std::map<std::string, FMOD::Studio::Bank*> getBankContainer();

            /**
             * Get container of event description that's loaded
             */
            DLL_API  std::map<std::string, FMOD::Studio::EventDescription*> getEventDescriptionContainer();

            /**
             * Get container of event instances that's loaded
             */
            DLL_API  std::map<std::string, FMOD::Studio::EventInstance*> getEventInstanceContainer();

            /**
             * Find the SoundInfo by name
             */
            DLL_API  SoundInfo* findSound(std::string name);

            /**
             * Find the SoundInfo by unique ID
             */
            DLL_API  SoundInfo* findSound(unsigned int ID);

            // The audio sampling rate of the audio engine
            DLL_API  static const int AUDIO_SAMPLE_RATE = 44100;

        private:
            /**
             * Default AudioEngine constructor.
             * AudioEngine::init() must be called before using the Audio Engine
             */
            DLL_API  AudioEngine();

            //Instance of AudioEngine
            static AudioEngine* audioE_instance;

            /**
             * Checks if a sound file is in the soundCache
             */
            bool soundLoaded(SoundInfo soundInfo);

            /**
             * Sets the 3D position of a sound
             */
            void set3dChannelPosition(SoundInfo& soundInfo, FMOD::Channel* channel);

            /**
             * Initializes the reverb effect
             */
            void initReverb();

            /**
             * Prints debug info about an FMOD event description
             */
            void printEventInfo(FMOD::Studio::EventDescription* eventDescription);

            // FMOD Studio API system, which can play FMOD sound banks (*.bank)
            FMOD::Studio::System* studioSystem{ nullptr };

            // FMOD's low-level audio system which plays audio files and is obtained from Studio System
            FMOD::System* lowLevelSystem = nullptr;

            // Max FMOD::Channels for the audio engine 
            static const unsigned int MAX_AUDIO_CHANNELS = 1024;

            // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.
            const float DISTANCEFACTOR = 200.0f;

            // Listener head position, initialized to default value
            FMOD_VECTOR listenerpos = { 0.0f, 0.0f, -1.0f };

            FMOD_VECTOR listenervelocity = { 0.f, 0.f, 0.f * DISTANCEFACTOR };

            // Listener forward vector, initialized to default value
            FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };

            // Listener upwards vector, initialized to default value
            FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };

            // Main group for low level system which all sounds go through
            FMOD::ChannelGroup* mastergroup = 0;

            // SFX group for low level system which all SFX go through
            FMOD::ChannelGroup* SFX = 0;

            // BGM group for low level system which all BGM go through
            FMOD::ChannelGroup* BGM = 0;

            // Low-level system reverb TODO add multi-reverb support
            FMOD::Reverb3D* reverb;

            // Reverb origin position
            FMOD_VECTOR revPos = { 0.0f, 0.0f, 0.0f };

            // reverb min, max distances
            float revMinDist = 10.0f, revMaxDist = 50.0f;

            // flag tracking if the Audio Engin is muted
            bool muted = false;

            /*
             * Map which caches FMOD Low-Level sounds
             * Key is the SoundInfo's uniqueKey field.
             * Value is the FMOD::Sound* to be played back.
             */
            std::map<unsigned int, FMOD::Sound*> sounds{};

            /*
             * Map which stores details for SoundInfo that's loaded
             */
            std::map<std::string, SoundInfo*> SoundInfo_Container{};

            /*
             * Map which stores the current playback channels of any playing sound
             * Key is the SoundInfo's uniqueKey field.
             * Value is the FMOD::Channel* the FMOD::Sound* is playing back on.
             */
            std::map<unsigned int, FMOD::Channel*> channels{};

            /*
             * Map which stores the soundbanks loaded with loadFMODStudioBank()
             */
            std::map<std::string, FMOD::Studio::Bank*> soundBanks{};

            /*
             * Map which stores event descriptions created during loadFMODStudioEvent()
             */
            std::map<std::string, FMOD::Studio::EventDescription*> eventDescriptions{};

            /*
             * Map which stores event instances created during loadFMODStudioEvent()
             */
            std::map<std::string, FMOD::Studio::EventInstance*> eventInstances{};
        }; //end of AudioEngine
    } //end of AudioWerks

    class DLL_API proxy_audio_system
    {
    public:
        static void audio_system_init();
        static void audio_system_update(const float dt, const std::vector<EntityID>& entities, Transform* soundInfo);

        //static void audio_event_play(SoundInfo& soundInfo);
        static void audio_event_init(SoundInfo* container);
        static void audio_event_update();

        static void load_all_audio_files();
        static std::vector<std::filesystem::path> go_deeper(std::filesystem::path f_path);

        static void ScriptPlay(std::string pathing);
        static void ScriptPause(std::string pathing);
        static void ScriptStop(std::string pathing);
        static void ScriptLoad(std::string pathing);
        static void ScriptUnload(std::string pathing);
        static SoundInfo* ScriptGetSound(std::string pathing);
        static unsigned int ScriptGetID(std::string pathing);
        static bool ScriptGetLoop(std::string pathing);

        static bool CheckPlaying(std::string pathing); //to be changed
        static bool CheckPause(std::string pathing); //to be changed
        static void ScriptPlayAllPaused();
        static void ScriptPauseAll();
        static void ScriptStopAll();
        static bool ScriptAnySoundPlaying();
        static void ScriptFadeOut(unsigned int duration, std::string pathing);
        static void ScriptFadeIn(unsigned int duration, std::string pathing);
        static void ScriptSetPosition(Vec3 pos, std::string pathing);
        static void ScriptSetListenerPos(Vec3 pos, Vec3 for_vec, Vec3 up_vec);
        static void ScriptSetLoop(bool set, std::string pathing);

        static SoundInfo* find_sound_info(std::string str);
        static void Add_to_Queue(std::string str = "");
        static void Remove_from_Queue(std::string str);
        static void Play_queue();
        static void Clear_queue();

        static float getVolume(std::string pathing);
        static float getMasterVolume();
        static float getBGMVolume();
        static float getSFXVolume();
        static Vec3 getListenerPos();

        static void SetVolume(float vol, std::string pathing);
        static void SetMasterVolume(float vol);
        static void SetBGMVolume(float vol);
        static void SetSFXVolume(float vol);

        static bool checkifdone(std::string str);

    private:
        static AudioWerks::AudioEngine* aud_instance;

        static int totalNumClips;
        static bool Q_state;
        static SoundInfo Q_name;

        static std::map<std::string, SoundInfo> allSounds;
        static std::map<std::string, std::pair<bool, SoundInfo*>> Queue;
        //static std::map<unsigned int, std::map<Vec3*, SOUND_STATE*>> sound_events;
    }; //end of proxy_audio_system
} //end of TDS

#endif