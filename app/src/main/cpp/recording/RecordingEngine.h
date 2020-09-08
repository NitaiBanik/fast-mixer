//
// Created by asalehin on 7/9/20.
//

#ifndef FAST_MIXER_RECORDINGENGINE_H
#define FAST_MIXER_RECORDINGENGINE_H

#ifndef MODULE_NAME
#define MODULE_NAME "AudioEngine"
#endif

#include <oboe/Definitions.h>
#include <oboe/AudioStream.h>
#include "../logging_macros.h"
#include "RecordingIO.h"
#include "../streams/BaseStream.h"
#include "../streams/RecordingStream.h"
#include "../streams/LivePlaybackStream.h"
#include "../streams/PlaybackStream.h"

class RecordingEngine {
public:
    RecordingEngine(char* appDir, bool recordingScreenViewModelPassed);
    ~RecordingEngine();

    void setRecordingSessionId(char* recordingSessionId);

    void startRecording();
    void stopRecording();
    void pauseRecording();

    void startLivePlayback();
    void stopLivePlayback();
    void pauseLivePlayback();

    bool startPlayback();
    void stopAndResetPlayback();
    void pausePlayback();

    void flushWriteBuffer();
    void restartPlayback();

    int getCurrentMax();

    void resetCurrentMax();

    void setStopPlayback();

    int getTotalRecordedFrames();

    int getCurrentPlaybackProgress();

    void setPlayHead(int position);

    int getDurationInSeconds();

    void resetRecordingEngine();

private:
    const char* TAG = "Audio Engine:: %s";

    char* mRecordingSessionId = nullptr;
    char* mAppDir = nullptr;
    bool mPlayback = true;

    RecordingIO mRecordingIO;
    RecordingStream recordingStream = RecordingStream(&mRecordingIO);
    LivePlaybackStream livePlaybackStream = LivePlaybackStream(&mRecordingIO);
    PlaybackStream playbackStream = PlaybackStream(&mRecordingIO);
    bool mRecordingScreenViewModelPassed = false;

    void closePlaybackStream();

    void stopPlayback();
};


#endif //FAST_MIXER_RECORDINGENGINE_H
