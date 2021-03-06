package com.bluehub.fastmixer.screens.mixing

import android.content.Context
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import com.bluehub.fastmixer.MixerApplication
import com.bluehub.fastmixer.common.permissions.PermissionViewModel
import com.bluehub.fastmixer.common.utils.PermissionManager
import com.bluehub.fastmixer.common.utils.ScreenConstants
import timber.log.Timber
import java.io.File
import javax.inject.Inject

class MixingScreenViewModel(override val context: Context, mixerApplication: MixerApplication, override val tag: String): PermissionViewModel(context, mixerApplication, tag) {
    override var TAG: String = javaClass.simpleName

    private var audioFiles: MutableList<AudioFile>
    val audioFilesLiveData = MutableLiveData<MutableList<AudioFile>>(mutableListOf())

    @Inject
    override lateinit var permissionManager: PermissionManager

    @Inject
    lateinit var mixingRepository: MixingRepository

    private val _eventRecord = MutableLiveData<Boolean>()
    val eventRecord: LiveData<Boolean>
        get() = _eventRecord

    val _eventRead = MutableLiveData<Boolean>()
        val eventRead: LiveData<Boolean>
        get() = _eventRead

    init {
        getViewModelComponent().inject(this)
        audioFiles = mutableListOf()
        mixingRepository.createMixingEngine()
    }

    fun onRecord() {
        _eventRecord.value = true
    }

    fun onReadFromDisk() {
        if(!checkReadFilePermission()) {
            setRequestReadFilePermission(ScreenConstants.READ_FROM_FILE)
            return
        }
    }

    fun onSaveToDisk() {
        if(!checkWriteFilePermission()) {
            setRequestWriteFilePermission(ScreenConstants.WRITE_TO_FILE)
            return
        }
    }

    fun onRecordNavigated() {
        _eventRecord.value = false
    }

    fun addRecordedFilePath(filePath: String) {
        val file = File(filePath)
        if (file.exists()) {
            audioFiles.add(AudioFile(filePath, AudioFileType.RECORDED, false))
            audioFilesLiveData.value = audioFiles
        }
    }

    fun reInitRecordedFiles() {
        audioFiles = audioFiles.map {
            it.copy(rendered = false)
        } as MutableList<AudioFile>
        audioFilesLiveData.value = audioFiles
    }

    fun renderAudioFiles() {

    }
}