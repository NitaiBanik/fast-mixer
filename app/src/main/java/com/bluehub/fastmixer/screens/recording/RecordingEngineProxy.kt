package com.bluehub.fastmixer.screens.recording

class RecordingEngineProxy {
    companion object {
        private val INSTANCE: RecordingEngineProxy =
            RecordingEngineProxy()

        public fun getInstance(): RecordingEngineProxy {
            return INSTANCE
        }
    }

    fun create(appPathStr: String,
               recordingScreenViewModelPassed: Boolean = false): Boolean =
        RecordingEngine.create(
            appPathStr,
            recordingScreenViewModelPassed
        )

    fun delete() =
        RecordingEngine.delete()

    fun startRecording() =
        RecordingEngine.startRecording()

    fun stopRecording() =
        RecordingEngine.stopRecording()

    fun pauseRecording() =
        RecordingEngine.pauseRecording()

    fun startLivePlayback() =
        RecordingEngine.startLivePlayback()

    fun stopLivePlayback() =
        RecordingEngine.stopLivePlayback()

    fun pauseLivePlayback() =
        RecordingEngine.pauseLivePlayback()

    fun startPlayback(): Boolean =
        RecordingEngine.startPlayback()

    fun stopPlayback() =
        RecordingEngine.stopPlayback()

    fun pausePlayback() =
        RecordingEngine.pausePlayback()

    fun flushWriteBuffer() =
        RecordingEngine.flushWriteBuffer()

    fun restartPlayback() =
        RecordingEngine.restartPlayback()

    fun getCurrentMax(): Int =
        RecordingEngine.getCurrentMax()

    fun resetCurrentMax() =
        RecordingEngine.resetCurrentMax()

    fun getTotalRecordedFrames() =
        RecordingEngine.getTotalRecordedFrames()

    fun getCurrentPlaybackProgress() =
        RecordingEngine.getCurrentPlaybackProgress()

    fun setPlayHead(position: Int) =
        RecordingEngine.setPlayHead(
            position
        )

    fun getDurationInSeconds(): Int =
        RecordingEngine.getDurationInSeconds()

    fun resetRecordingEngine() =
        RecordingEngine.resetRecordingEngine()

    fun setRecordingSessionId(recordingSessionIdStr: String) =
        RecordingEngine.setRecordingSessionId(recordingSessionIdStr)
}