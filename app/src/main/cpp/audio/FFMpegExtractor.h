/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FFMPEG_FFMPEGEXTRACTOR_H
#define FFMPEG_FFMPEGEXTRACTOR_H


extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

#include <cstdint>
#include <android/asset_manager.h>
#include "../Constants.h"

using namespace std;

class FFMpegExtractor {
public:
    FFMpegExtractor(const string &filePath, const AudioProperties targetProperties);

    bool decode();

    int64_t readData(uint8_t *targetData);

    const char *mFilePath;
    FILE* fp = nullptr;

    int mSampleRate = 0;
    int mChannelCount = 0;
    int mAudioFormat = 0;

private:
    AudioProperties mTargetProperties;

    bool decodedSuccessfully = false;

    AVStream *mStream;

    unique_ptr<AVFormatContext, decltype(&avformat_free_context)> mFormatContext {nullptr,nullptr};
    unique_ptr<AVCodecContext, void(*)(AVCodecContext *)> mCodecContext {nullptr, nullptr};

    bool createAVIOContext(uint8_t *buffer, uint32_t bufferSize,
                                  AVIOContext **avioContext);

    bool createAVFormatContext(AVIOContext *avioContext, AVFormatContext **avFormatContext);

    bool openAVFormatContext(AVFormatContext *avFormatContext, FILE* fl);

    int32_t cleanup(AVIOContext *avioContext, AVFormatContext *avFormatContext);

    bool getStreamInfo(AVFormatContext *avFormatContext);

    AVStream *getBestAudioStream(AVFormatContext *avFormatContext);

    AVCodec *findCodec(AVCodecID id);

    void printCodecParameters(AVCodecParameters *params);
};


#endif //FFMPEG_FFMPEGEXTRACTOR_H