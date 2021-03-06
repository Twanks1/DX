#pragma once
/**********************************************************************
    class: AudioManager (audio_manager.h)

    author: S. Hau
    date: April 14, 2018

    Manages audio assets and initialization of the audio lib.
**********************************************************************/

#include "Common/i_subsystem.hpp"

namespace Core { namespace Audio {

    #define INPUTCHANNELS 1  // Number of source channels
    #define OUTPUTCHANNELS 8 // Maximum number of destination channels supported

    //**********************************************************************
    class AudioManager : public Core::ISubSystem
    {
    public:
        AudioManager() = default;
        ~AudioManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Set the volume level for all audio sources.
        //----------------------------------------------------------------------
        void setVolume(F32 volume);
        F32 getVolume() const;

        //----------------------------------------------------------------------
        // Update the listeners position / orientation for computing 3d sound.
        //----------------------------------------------------------------------
        void updateListener(const X3DAUDIO_LISTENER& listener);

        //----------------------------------------------------------------------
        // Update the 3d sound effect for a source voice and a given emitter.
        //----------------------------------------------------------------------
        void update3DVoice(const X3DAUDIO_EMITTER& emitter, IXAudio2SourceVoice* voice, UINT32 sourceChannels);

        //----------------------------------------------------------------------
        // This is needed to create XAudio2SourceVoices.
        IXAudio2* getXAudio2() { return m_pXAudio2; }

    private:
        IXAudio2*               m_pXAudio2      = nullptr;
        IXAudio2MasteringVoice* m_pMasterVoice  = nullptr;
        XAUDIO2_VOICE_DETAILS   m_voiceDetails;
        X3DAUDIO_HANDLE         m_X3DInstance;
        F32                     m_matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];
        X3DAUDIO_LISTENER       m_listener = { {}, {}, { 4096, 4096, 4096 } };
        X3DAUDIO_DSP_SETTINGS   m_DSPSettings = {};

        NULL_COPY_AND_ASSIGN(AudioManager)
    };

} } // End namespaces