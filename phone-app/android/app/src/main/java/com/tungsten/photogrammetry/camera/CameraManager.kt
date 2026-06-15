package com.tungsten.photogrammetry.camera

import android.content.Context
import android.util.Size
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageCapture
import androidx.camera.core.ImageCaptureException
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleOwner
import com.tungsten.photogrammetry.models.CameraCalibrationData
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class CameraManager {

    private var cameraProvider: ProcessCameraProvider? = null
    private var imageCapture: ImageCapture? = null
    private var preview: Preview? = null
    private var cameraExecutor: ExecutorService = Executors.newSingleThreadExecutor()

    @androidx.camera.core.ExperimentalGetImage
    fun initialize(
        context: Context,
        lifecycleOwner: LifecycleOwner,
        previewView: PreviewView,
        targetResolution: Size = Size(1920, 1080)
    ) {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(context)
        cameraProviderFuture.addListener({
            cameraProvider = cameraProviderFuture.get()

            preview = Preview.Builder()
                .build()
                .also { it.setSurfaceProvider(previewView.surfaceProvider) }

            imageCapture = ImageCapture.Builder()
                .setTargetResolution(targetResolution)
                .setCaptureMode(ImageCapture.CAPTURE_MODE_MAXIMIZE_QUALITY)
                .setFlashMode(ImageCapture.FLASH_MODE_OFF)
                .build()

            val cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA

            try {
                cameraProvider?.unbindAll()
                cameraProvider?.bindToLifecycle(
                    lifecycleOwner,
                    cameraSelector,
                    preview,
                    imageCapture
                )
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }, ContextCompat.getMainExecutor(context))
    }

    fun takePhoto(callback: (ByteArray) -> Unit) {
        val capture = imageCapture ?: return

        capture.takePicture(
            cameraExecutor,
            object : ImageCapture.OnImageCapturedCallback() {
                @androidx.camera.core.ExperimentalGetImage
                override fun onCaptureSuccess(imageProxy: androidx.camera.core.ImageProxy) {
                    val buffer = imageProxy.planes[0].buffer
                    val bytes = ByteArray(buffer.remaining())
                    buffer.get(bytes)
                    imageProxy.close()
                    callback(bytes)
                }

                override fun onError(exception: ImageCaptureException) {
                    exception.printStackTrace()
                }
            }
        )
    }

    fun getCameraCharacteristics(context: Context): CameraCalibrationData? {
        val manager = context.getSystemService(Context.CAMERA_SERVICE) as? android.hardware.camera2.CameraManager
            ?: return null

        val cameraId = manager.cameraIdList.firstOrNull() ?: return null
        val characteristics = manager.getCameraCharacteristics(cameraId)

        val focalLengths = characteristics.get(android.hardware.camera2.CameraCharacteristics.LENS_INFO_FOCAL_LENGTH)
        val sensorSize = characteristics.get(android.hardware.camera2.CameraCharacteristics.SENSOR_INFO_PHYSICAL_SIZE)

        if (focalLengths == null || sensorSize == null) return null

        val focalLengthMm = focalLengths[0]
        val sensorWidthMm = sensorSize.width
        val sensorHeightMm = sensorSize.height

        val imageWidth = 1920
        val imageHeight = 1080

        val fx = (focalLengthMm / sensorWidthMm) * imageWidth
        val fy = (focalLengthMm / sensorHeightMm) * imageHeight
        val cx = imageWidth / 2.0
        val cy = imageHeight / 2.0

        val cameraMatrix = arrayOf(
            doubleArrayOf(fx, 0.0, cx),
            doubleArrayOf(0.0, fy, cy),
            doubleArrayOf(0.0, 0.0, 1.0)
        )

        return CameraCalibrationData(
            cameraMatrix = cameraMatrix,
            distortionCoeffs = doubleArrayOf(0.0, 0.0, 0.0, 0.0, 0.0),
            imageSize = Pair(imageWidth, imageHeight)
        )
    }

    fun stopPreview() {
        cameraProvider?.unbindAll()
    }

    fun shutdown() {
        cameraExecutor.shutdown()
    }
}
