package com.tungsten.photogrammetry.export

import android.os.Build
import com.tungsten.photogrammetry.models.CaptureSession
import com.tungsten.photogrammetry.models.CaptureSettings
import com.tungsten.photogrammetry.models.ScanProject
import org.json.JSONArray
import org.json.JSONObject
import java.io.File
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class ProjectExporter {

    fun exportProject(session: CaptureSession, outputDir: File): File {
        if (!outputDir.exists()) {
            outputDir.mkdirs()
        }

        val projectFile = File(outputDir, "photogrammetry_project.zip")

        val images = session.images.map { capturedImage ->
            capturedImage.filename to getPlaceholderImage()
        }

        val posesJson = buildPosesJson(session)
        val calibrationJson = buildCalibrationJson(session)
        val projectJson = buildProjectJson(session)

        val metadata = mapOf(
            "poses.json" to posesJson,
            "calibration.json" to calibrationJson,
            "project.json" to projectJson
        )

        return ZipCreator.createZip(projectFile, images, metadata)
    }

    private fun buildPosesJson(session: CaptureSession): String {
        val root = JSONObject()
        root.put("session_id", session.id)
        root.put("num_images", session.imageCount)

        val posesArray = JSONArray()
        session.images.forEachIndexed { index, image ->
            val poseObj = JSONObject()
            poseObj.put("filename", image.filename)
            poseObj.put("timestamp", image.timestamp)

            val positionArray = JSONArray()
            image.pose.position.forEach { positionArray.put(it.toDouble()) }
            poseObj.put("position", positionArray)

            val rotationArray = JSONArray()
            image.pose.rotation.forEach { rotationArray.put(it.toDouble()) }
            poseObj.put("rotation_quaternion", rotationArray)

            posesArray.put(poseObj)
        }
        root.put("poses", posesArray)

        return root.toString(2)
    }

    private fun buildCalibrationJson(session: CaptureSession): String {
        val root = JSONObject()
        val cal = session.calibration

        if (cal != null) {
            val matrixArray = JSONArray()
            cal.cameraMatrix.forEach { row ->
                val rowArray = JSONArray()
                row.forEach { rowArray.put(it) }
                matrixArray.put(rowArray)
            }
            root.put("camera_matrix", matrixArray)

            val distArray = JSONArray()
            cal.distortionCoeffs.forEach { distArray.put(it) }
            root.put("distortion_coefficients", distArray)

            root.put("image_width", cal.imageSize.first)
            root.put("image_height", cal.imageSize.second)
        } else {
            root.put("camera_matrix", JSONArray().apply {
                put(JSONArray().put(1.0).put(0.0).put(960.0))
                put(JSONArray().put(0.0).put(1.0).put(540.0))
                put(JSONArray().put(0.0).put(0.0).put(1.0))
            })
            root.put("distortion_coefficients", JSONArray().apply {
                put(0.0); put(0.0); put(0.0); put(0.0); put(0.0)
            })
            root.put("image_width", 1920)
            root.put("image_height", 1080)
        }

        return root.toString(2)
    }

    private fun buildProjectJson(session: CaptureSession): String {
        val dateFormat = SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'", Locale.US)
        val dateString = dateFormat.format(Date(session.startTime))

        val project = ScanProject(
            device = "${Build.MANUFACTURER} ${Build.MODEL}",
            date = dateString,
            captureSettings = CaptureSettings(
                resolution = Pair(1920, 1080),
                format = "jpeg",
                quality = 0.92f
            ),
            numImages = session.imageCount
        )

        val root = JSONObject()
        root.put("version", project.version)
        root.put("device", project.device)
        root.put("date", project.date)

        val settingsObj = JSONObject()
        settingsObj.put("resolution", JSONArray().apply {
            put(project.captureSettings.resolution.first)
            put(project.captureSettings.resolution.second)
        })
        settingsObj.put("format", project.captureSettings.format)
        settingsObj.put("quality", project.captureSettings.quality.toDouble())
        root.put("capture_settings", settingsObj)

        root.put("num_images", project.numImages)

        return root.toString(2)
    }

    private fun getPlaceholderImage(): ByteArray {
        return ByteArray(0)
    }
}
