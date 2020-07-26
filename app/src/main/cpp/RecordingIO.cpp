//
// Created by asalehin on 7/11/20.
//

#include <cstdint>
#include <cstdio>
#include <string>
#include <unistd.h>
#include "RecordingIO.h"
#include "logging_macros.h"
#include "Utils.h"
#include <mutex>
#include <condition_variable>
#include <sys/stat.h>
#include "Constants.h"
#include "AudioEngine.h"

std::mutex RecordingIO::mtx;
std::condition_variable RecordingIO::reallocated;
bool RecordingIO::is_reallocated = false;

bool RecordingIO::check_if_reallocated() {
    return is_reallocated;
}

bool RecordingIO::setup_audio_source() {
    if (!validate_audio_file()) {
        return false;
    }

    AudioProperties targetProperties {
            .channelCount = AudioEngine::mOutputChannelCount,
            .sampleRate = AudioEngine::mSampleRate
    };

    std::shared_ptr<FileDataSource> audioSource {
            FileDataSource::newFromCompressedFile(mRecordingFilePath.c_str(), targetProperties)
    };

    if (audioSource == nullptr) {
        return false;
    }

    mRecordedTrack = std::make_unique<Player>(audioSource);
    mRecordedTrack->setPlaying(true);
    return true;
}

void RecordingIO::pause_audio_source() {
    if (mRecordedTrack == nullptr) {
        return;
    }
    mRecordedTrack->setPlaying(false);
}

void RecordingIO::stop_audio_source() {
    pause_audio_source();
    mRecordedTrack = nullptr;
}

bool RecordingIO::validate_audio_file() {
    return !(mRecordingFilePath.empty() || (access(mRecordingFilePath.c_str(), F_OK) == -1));
}

void RecordingIO::read_playback(int16_t *targetData, int32_t numSamples) {
    LOGD(TAG, "read(): ");
    LOGD(TAG, std::to_string(numSamples).c_str());

    if (!validate_audio_file()) {
        return;
    }

    if (mRecordedTrack == nullptr) {
        return;
    }

    if (mTotalSamples > 0) {
        mRecordedTrack->renderAudio(targetData, numSamples);
    }
}

void RecordingIO::flush_to_file(int16_t* buffer, int length, const std::string& recordingFilePath, std::unique_ptr<SndfileHandle>& recordingFile) {
    if (recordingFile == nullptr) {
        int format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
        SndfileHandle file = SndfileHandle(recordingFilePath, SFM_WRITE, format, AudioEngine::mInputChannelCount, AudioEngine::mSampleRate);
        recordingFile = std::make_unique<SndfileHandle>(file);
    }
    recordingFile->write(buffer, length);

    std::unique_lock<std::mutex> lck(mtx);
    reallocated.wait(lck, check_if_reallocated);
    delete[] buffer;
    is_reallocated = false;
}

void RecordingIO::perform_flush(int flushIndex) {
    int16_t* oldBuffer = mData;
    is_reallocated = false;
    taskQueue->enqueue([&]() {
        flush_to_file(oldBuffer, flushIndex, mRecordingFilePath, mRecordingFile);
    });

    auto * newData = new int16_t[kMaxSamples]{0};
    std::copy(mData + flushIndex, mData + mWriteIndex, newData);
    mData = newData;
    is_reallocated = true;
    mWriteIndex -= flushIndex;
    mLivePlaybackReadIndex -= flushIndex;
    readyToFlush = false;
    toFlush = false;
    mIteration = 1;
}

int32_t RecordingIO::write(const int16_t *sourceData, int32_t numSamples) {
    LOGD(TAG, "write(): ");

    if (mWriteIndex + numSamples > kMaxSamples) {
        readyToFlush = true;
    }

    int flushIndex = 0;
    if (readyToFlush) {
        int upperBound  = 0;
        if (mWriteIndex < kMaxSamples) {
            upperBound = mWriteIndex;
        } else {
            upperBound = kMaxSamples;
        }
        if (livePlaybackEnabled && mLivePlaybackReadIndex >= upperBound) {
            flushIndex = upperBound;
            toFlush = true;
        } else if (!livePlaybackEnabled) {
            flushIndex = mWriteIndex;
            toFlush = true;
        }
    }

    if (toFlush) {
        perform_flush(flushIndex);
    }

    if (mWriteIndex + numSamples > mIteration * kMaxSamples) {
        readyToFlush = true;
        mIteration++;
        int32_t newSize = mIteration * kMaxSamples;
        auto * newData = new int16_t[newSize]{0};
        std::copy(mData, mData + mWriteIndex, newData);
        delete[] mData;
        mData = newData;
    }

    for(int i = 0; i < numSamples; i++) {
        mData[mWriteIndex++] = sourceData[i] * gain_factor;
    }
    mTotalSamples += numSamples;

    return numSamples;
}

void RecordingIO::flush_buffer() {
    if (mWriteIndex > 0) {
        int16_t* oldBuffer = mData;
        is_reallocated = false;
        taskQueue->enqueue([&]() {
            flush_to_file(oldBuffer, static_cast<int>(mWriteIndex), mRecordingFilePath, mRecordingFile);
        });

        mIteration = 1;
        mWriteIndex = 0;
        mLivePlaybackReadIndex = 0;
        mData = new int16_t[kMaxSamples]{0};
        is_reallocated = true;
        readyToFlush = false;
        toFlush = false;
    }
}

int32_t RecordingIO::read_live_playback(int16_t *targetData, int32_t numSamples) {
    int32_t framesRead = 0;
    while (framesRead < numSamples && mLivePlaybackReadIndex < mTotalSamples) {
        targetData[framesRead++] = mData[mLivePlaybackReadIndex++];
    }
    return framesRead;
}