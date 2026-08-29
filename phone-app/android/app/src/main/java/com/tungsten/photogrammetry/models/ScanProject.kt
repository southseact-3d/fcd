package com.tungsten.photogrammetry.models

data class ScanProject(
    val version: Int = 1,
    val device: String,
    val date: String,
    val captureSettings: CaptureSettings,
    val numImages: Int
)

data class CaptureSettings(
    val resolution: Pair<Int, Int>,
    val format: String = "jpeg",
    val quality: Float = 0.92f
)
