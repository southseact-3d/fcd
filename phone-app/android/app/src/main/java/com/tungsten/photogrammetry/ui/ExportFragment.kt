package com.tungsten.photogrammetry.ui

import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.ProgressBar
import android.widget.TextView
import androidx.core.content.FileProvider
import androidx.fragment.app.Fragment
import com.tungsten.photogrammetry.MainActivity
import com.tungsten.photogrammetry.R
import com.tungsten.photogrammetry.export.ProjectExporter
import kotlinx.coroutines.*
import java.io.File
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class ExportFragment : Fragment() {

    private lateinit var projectInfoText: TextView
    private lateinit var exportButton: Button
    private lateinit var shareButton: Button
    private lateinit var progressBar: ProgressBar

    private val exporter = ProjectExporter()
    private val scope = CoroutineScope(Dispatchers.Main + SupervisorJob())
    private var exportedFile: File? = null

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_export, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        projectInfoText = view.findViewById(R.id.project_info_text)
        exportButton = view.findViewById(R.id.export_button)
        shareButton = view.findViewById(R.id.share_button)
        progressBar = view.findViewById(R.id.progress_bar)

        shareButton.isEnabled = false

        exportButton.setOnClickListener {
            exportProject()
        }

        shareButton.setOnClickListener {
            shareProject()
        }

        updateProjectInfo()
    }

    private fun updateProjectInfo() {
        val activity = requireActivity() as MainActivity
        val session = activity.getCaptureSession()

        val dateFormat = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault())
        val dateString = dateFormat.format(Date(session.startTime))

        val info = buildString {
            appendLine("Device: ${android.os.Build.MANUFACTURER} ${android.os.Build.MODEL}")
            appendLine("Date: $dateString")
            appendLine("Images: ${session.imageCount}")
            appendLine("Calibration: ${if (session.calibration != null) "Available" else "Default"}")
            appendLine("Session ID: ${session.id.take(8)}...")
        }

        projectInfoText.text = info
    }

    private fun exportProject() {
        val activity = requireActivity() as MainActivity
        val session = activity.getCaptureSession()

        if (session.imageCount == 0) {
            projectInfoText.text = "No images captured. Take some photos first."
            return
        }

        progressBar.visibility = View.VISIBLE
        exportButton.isEnabled = false
        shareButton.isEnabled = false

        scope.launch {
            try {
                val outputDir = File(requireContext().filesDir, "exports")
                val file = withContext(Dispatchers.IO) {
                    exporter.exportProject(session, outputDir)
                }

                exportedFile = file
                progressBar.visibility = View.GONE
                exportButton.isEnabled = true
                shareButton.isEnabled = true

                projectInfoText.text = "Export complete!\nFile: ${file.name}\nSize: ${file.length() / 1024} KB"
            } catch (e: Exception) {
                progressBar.visibility = View.GONE
                exportButton.isEnabled = true
                projectInfoText.text = "Export failed: ${e.message}"
            }
        }
    }

    private fun shareProject() {
        val file = exportedFile ?: return

        val uri = FileProvider.getUriForFile(
            requireContext(),
            "${requireContext().packageName}.fileprovider",
            file
        )

        val shareIntent = Intent(Intent.ACTION_SEND).apply {
            type = "application/zip"
            putExtra(Intent.EXTRA_STREAM, uri)
            addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
        }

        startActivity(Intent.createChooser(shareIntent, "Share Project"))
    }

    override fun onDestroyView() {
        super.onDestroyView()
        scope.cancel()
    }
}
