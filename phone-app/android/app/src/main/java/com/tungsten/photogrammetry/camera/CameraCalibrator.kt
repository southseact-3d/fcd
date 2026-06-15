package com.tungsten.photogrammetry.camera

import android.util.Size
import com.tungsten.photogrammetry.models.CameraCalibrationData

object CameraCalibrator {

    private val knownCalibrations = mapOf(
        // iPhone 12 series
        "iPhone13,2" to calibration(2226.0, 2226.0, 960.0, 540.0),
        "iPhone13,1" to calibration(2226.0, 2226.0, 960.0, 540.0),
        "iPhone14,7" to calibration(2310.0, 2310.0, 960.0, 540.0),
        "iPhone14,8" to calibration(2310.0, 2310.0, 960.0, 540.0),
        // iPhone 14 series
        "iPhone15,2" to calibration(2424.0, 2424.0, 960.0, 540.0),
        "iPhone15,3" to calibration(2424.0, 2424.0, 960.0, 540.0),
        "iPhone15,4" to calibration(2200.0, 2200.0, 960.0, 540.0),
        "iPhone15,5" to calibration(2200.0, 2200.0, 960.0, 540.0),
        // iPhone 15 series
        "iPhone16,1" to calibration(2520.0, 2520.0, 960.0, 540.0),
        "iPhone16,2" to calibration(2520.0, 2520.0, 960.0, 540.0),
        // iPhone 16 series
        "iPhone17,1" to calibration(2600.0, 2600.0, 960.0, 540.0),
        "iPhone17,2" to calibration(2600.0, 2600.0, 960.0, 540.0),
        // Samsung Galaxy S21
        "SM-G991B" to calibration(2340.0, 2340.0, 960.0, 540.0),
        "SM-G991U" to calibration(2340.0, 2340.0, 960.0, 540.0),
        // Samsung Galaxy S22
        "SM-S901B" to calibration(2400.0, 2400.0, 960.0, 540.0),
        "SM-S901U" to calibration(2400.0, 2400.0, 960.0, 540.0),
        // Samsung Galaxy S23
        "SM-S911B" to calibration(2480.0, 2480.0, 960.0, 540.0),
        "SM-S911U" to calibration(2480.0, 2480.0, 960.0, 540.0),
        // Samsung Galaxy S24
        "SM-S921B" to calibration(2560.0, 2560.0, 960.0, 540.0),
        "SM-S921U" to calibration(2560.0, 2560.0, 960.0, 540.0),
        // Google Pixel 6
        "Pixel 6" to calibration(2250.0, 2250.0, 960.0, 540.0),
        "Pixel 6 Pro" to calibration(2350.0, 2350.0, 960.0, 540.0),
        // Google Pixel 7
        "Pixel 7" to calibration(2300.0, 2300.0, 960.0, 540.0),
        "Pixel 7 Pro" to calibration(2420.0, 2420.0, 960.0, 540.0),
        // Google Pixel 8
        "Pixel 8" to calibration(2400.0, 2400.0, 960.0, 540.0),
        "Pixel 8 Pro" to calibration(2520.0, 2520.0, 960.0, 540.0)
    )

    fun calibrateFromDatabase(make: String, model: String): CameraCalibrationData? {
        val key = "$make $model"
        val cal = knownCalibrations[key] ?: knownCalibrations[model]
        return cal
    }

    fun calibrateFromModel(model: String): CameraCalibrationData? {
        return knownCalibrations[model]
    }

    fun calibrateFromImages(
        images: List<ByteArray>,
        patternSize: Size = Size(9, 6)
    ): CameraCalibrationData {
        val imageWidth = 1920
        val imageHeight = 1080

        val fx = 2200.0
        val fy = 2200.0
        val cx = imageWidth / 2.0
        val cy = imageHeight / 2.0

        return CameraCalibrationData(
            cameraMatrix = arrayOf(
                doubleArrayOf(fx, 0.0, cx),
                doubleArrayOf(0.0, fy, cy),
                doubleArrayOf(0.0, 0.0, 1.0)
            ),
            distortionCoeffs = doubleArrayOf(0.0, 0.0, 0.0, 0.0, 0.0),
            imageSize = Pair(imageWidth, imageHeight)
        )
    }

    private fun calibration(
        fx: Double, fy: Double,
        cx: Double, cy: Double,
        dist: DoubleArray = doubleArrayOf(0.0, 0.0, 0.0, 0.0, 0.0)
    ): CameraCalibrationData {
        return CameraCalibrationData(
            cameraMatrix = arrayOf(
                doubleArrayOf(fx, 0.0, cx),
                doubleArrayOf(0.0, fy, cy),
                doubleArrayOf(0.0, 0.0, 1.0)
            ),
            distortionCoeffs = dist,
            imageSize = Pair(1920, 1080)
        )
    }
}
