package com.tungsten.photogrammetry.models

import java.util.UUID

data class CaptureSession(
    val id: String = UUID.randomUUID().toString(),
    val startTime: Long = System.currentTimeMillis(),
    val images: MutableList<CapturedImage> = mutableListOf(),
    val calibration: CameraCalibrationData? = null
) {
    val imageCount: Int get() = images.size
}

data class CapturedImage(
    val filename: String,
    val timestamp: Long,
    val pose: CameraPose
)

data class CameraCalibrationData(
    val cameraMatrix: Array<DoubleArray>,
    val distortionCoeffs: DoubleArray,
    val imageSize: Pair<Int, Int>
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is CameraCalibrationData) return false
        return cameraMatrix.contentEquals(other.cameraMatrix) &&
                distortionCoeffs.contentEquals(other.distortionCoeffs) &&
                imageSize == other.imageSize
    }

    override fun hashCode(): Int {
        var result = cameraMatrix.contentHashCode()
        result = 31 * result + distortionCoeffs.contentHashCode()
        result = 31 * result + imageSize.hashCode()
        return result
    }
}
