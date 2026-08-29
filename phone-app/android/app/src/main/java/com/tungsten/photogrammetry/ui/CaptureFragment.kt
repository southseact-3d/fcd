package com.tungsten.photogrammetry.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import androidx.camera.view.PreviewView
import androidx.fragment.app.Fragment
import com.tungsten.photogrammetry.MainActivity
import com.tungsten.photogrammetry.R
import com.tungsten.photogrammetry.camera.CameraCalibrator
import com.tungsten.photogrammetry.camera.CameraManager
import com.tungsten.photogrammetry.camera.CaptureGuideView
import com.tungsten.photogrammetry.models.CapturedImage
import kotlinx.coroutines.*

class CaptureFragment : Fragment() {

    private lateinit var previewView: PreviewView
    private lateinit var captureGuideView: CaptureGuideView
    private lateinit var captureButton: Button
    private lateinit var imageCounter: TextView
    private lateinit var rotationIndicator: TextView

    private val cameraManager = CameraManager()
    private var autoCaptureMode = false
    private val scope = CoroutineScope(Dispatchers.Main + SupervisorJob())
    private var rotationJob: Job? = null

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_capture, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        previewView = view.findViewById(R.id.preview_view)
        captureGuideView = view.findViewById(R.id.capture_guide_view)
        captureButton = view.findViewById(R.id.capture_button)
        imageCounter = view.findViewById(R.id.image_counter)
        rotationIndicator = view.findViewById(R.id.rotation_indicator)

        val activity = requireActivity() as MainActivity

        activity.getImuManager().let { imu ->
            if (imu != null) {
                cameraManager.initialize(
                    requireContext(),
                    viewLifecycleOwner,
                    previewView
                )
            }
        }

        captureButton.setOnClickListener {
            takePhoto()
        }

        updateImageCount()
        startRotationTracking()
    }

    private fun takePhoto() {
        val activity = requireActivity() as MainActivity
        val session = activity.getCaptureSession()
        val imu = activity.getImuManager()

        if (session.calibration == null) {
            val calibration = cameraManager.getCameraCharacteristics(requireContext())
                ?: CameraCalibrator.calibrateFromModel(android.os.Build.MODEL)
            if (calibration != null) {
                val updatedSession = session.copy(calibration = calibration)
            }
        }

        cameraManager.takePhoto { imageData ->
            val currentRotation = imu?.getRotationSinceLast() ?: 0f
            val pose = imu?.getCurrentPose()
                ?: com.tungsten.photogrammetry.models.CameraPose()

            val filename = String.format("IMG_%04d.jpg", session.imageCount + 1)
            val capturedImage = CapturedImage(
                filename = filename,
                timestamp = System.currentTimeMillis(),
                pose = pose
            )

            session.images.add(capturedImage)

            activity.runOnUiThread {
                updateImageCount()
                captureGuideView.reset()
            }
        }
    }

    private fun startRotationTracking() {
        rotationJob = scope.launch {
            while (isActive) {
                val activity = requireActivity() as? MainActivity
                val imu = activity?.getImuManager()
                if (imu != null) {
                    val delta = imu.getRotationSinceLast()
                    if (delta > 0.1f) {
                        captureGuideView.updateRotation(delta)
                    }

                    val ready = captureGuideView.shouldCapture()
                    rotationIndicator.text = if (ready) "Ready" else "Rotate"

                    if (autoCaptureMode && ready) {
                        takePhoto()
                    }
                }
                delay(50)
            }
        }
    }

    private fun updateImageCount() {
        val activity = requireActivity() as MainActivity
        val count = activity.getCaptureSession().imageCount
        imageCounter.text = String.format("%d photos", count)
    }

    override fun onDestroyView() {
        super.onDestroyView()
        rotationJob?.cancel()
        scope.cancel()
        cameraManager.stopPreview()
    }
}
