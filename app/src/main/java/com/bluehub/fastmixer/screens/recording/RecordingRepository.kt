package com.bluehub.fastmixer.screens.recording

import android.content.Context
import android.os.Build
import android.os.Environment
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.File
import java.nio.file.Files
import java.nio.file.Paths
import java.nio.file.StandardCopyOption
import java.util.*

class RecordingRepository(val recordingEngineProxy: RecordingEngineProxy) {
    private lateinit var recordingSessionId: String
    private lateinit var cacheDirForSession: String
    private lateinit var cacheDir: String

    suspend fun stopRecording() {
        withContext(Dispatchers.IO) {
            recordingEngineProxy.stopRecording()
        }
    }

    fun startLivePlayback() {
        recordingEngineProxy.startLivePlayback()
    }

    fun pauseLivePlayback() {
        recordingEngineProxy.pauseLivePlayback()
    }

    fun stopLivePlayback() {
        recordingEngineProxy.stopLivePlayback()
    }

    fun startPlaying(): Boolean {
        return recordingEngineProxy.startPlayback()
    }

    fun pausePlaying() {
        recordingEngineProxy.pausePlayback()
    }

    suspend fun stopPlaying() {
        withContext(Dispatchers.IO) {
            recordingEngineProxy.stopPlayback()
        }
    }

    fun stopPlayingSync() {
        recordingEngineProxy.stopPlayback()
    }

    fun startRecording() {
        recordingEngineProxy.startRecording()
    }

    suspend fun pauseRecording() {
        withContext(Dispatchers.IO) {
            recordingEngineProxy.pauseRecording()
        }
    }

    suspend fun flushWriteBuffer() {
        withContext(Dispatchers.IO) {
            recordingEngineProxy.flushWriteBuffer()
        }
    }

    suspend fun restartPlayback() {
        withContext(Dispatchers.IO) {
            recordingEngineProxy.restartPlayback()
        }
    }

    fun deleteRecordingEngine() {
        recordingEngineProxy.delete()
    }

    fun setCacheDirectory(cacheDirPath: String) {
        cacheDir = cacheDirPath
    }

    fun setRecordingSessionId() {
        recordingSessionId = UUID.randomUUID().toString()
        cacheDirForSession = "$cacheDir/$recordingSessionId"
        val cacheDirFile = File(cacheDirForSession)
        if (!cacheDirFile.exists()) {
            cacheDirFile.mkdir()
        }
        recordingEngineProxy.setRecordingSessionId(recordingSessionId)
    }

    fun createRecordingEngine() {
        recordingEngineProxy.create(cacheDir, true)
    }

    fun copyRecordedFile(context: Context) {
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.O) {
            val externalPath = context.getExternalFilesDir(Environment.DIRECTORY_MUSIC)
            val cacheFile = File("$cacheDirForSession/recording.wav")
            if (cacheFile.exists()) {
                Files.copy(
                    Paths.get("$cacheDirForSession/recording.wav"),
                    Paths.get(externalPath!!.path + "/$recordingSessionId.wav"),
                    StandardCopyOption.REPLACE_EXISTING
                )
            }
        }
    }

    fun getCurrentMax() = recordingEngineProxy.getCurrentMax()

    fun resetCurrentMax() = recordingEngineProxy.resetCurrentMax()

    fun getTotalRecordedFrames() = recordingEngineProxy.getTotalRecordedFrames()

    fun getCurrentPlaybackProgress() = recordingEngineProxy.getCurrentPlaybackProgress()

    fun setPlayHead(position: Int) = recordingEngineProxy.setPlayHead(position)

    fun getDurationInSeconds() = recordingEngineProxy.getDurationInSeconds()

    fun resetRecordingEngine() = recordingEngineProxy.resetRecordingEngine()

    fun getRecordedFilePath(): String = "$cacheDirForSession/recording.wav"
}