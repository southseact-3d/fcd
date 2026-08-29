package com.tungsten.photogrammetry.ui

import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import android.widget.ImageView
import android.widget.LinearLayout
import androidx.appcompat.app.AlertDialog
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.tungsten.photogrammetry.MainActivity
import com.tungsten.photogrammetry.R
import com.tungsten.photogrammetry.models.CapturedImage

class PreviewFragment : Fragment() {

    private lateinit var recyclerView: RecyclerView
    private lateinit var emptyStateText: TextView
    private lateinit var adapter: ImageGridAdapter

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_preview, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        recyclerView = view.findViewById(R.id.image_grid)
        emptyStateText = view.findViewById(R.id.empty_state_text)

        adapter = ImageGridAdapter(
            onImageClick = { image -> showImageDetail(image) },
            onDeleteClick = { image -> confirmDelete(image) }
        )

        recyclerView.layoutManager = GridLayoutManager(requireContext(), 3)
        recyclerView.adapter = adapter

        loadImages()
    }

    private fun loadImages() {
        val activity = requireActivity() as MainActivity
        val session = activity.getCaptureSession()

        if (session.images.isEmpty()) {
            recyclerView.visibility = View.GONE
            emptyStateText.visibility = View.VISIBLE
        } else {
            recyclerView.visibility = View.VISIBLE
            emptyStateText.visibility = View.GONE
            adapter.submitList(session.images.toList())
        }
    }

    private fun showImageDetail(image: CapturedImage) {
        val dialog = AlertDialog.Builder(requireContext())
            .setTitle(image.filename)
            .setMessage(
                "Timestamp: ${image.timestamp}\n" +
                "Position: (${image.pose.position.joinToString { "%.3f".format(it) }})\n" +
                "Rotation: (${image.pose.rotation.joinToString { "%.3f".format(it) }})"
            )
            .setPositiveButton("OK", null)
            .create()
        dialog.show()
    }

    private fun confirmDelete(image: CapturedImage) {
        AlertDialog.Builder(requireContext())
            .setTitle("Delete Image")
            .setMessage("Remove ${image.filename}?")
            .setPositiveButton("Delete") { _, _ ->
                val activity = requireActivity() as MainActivity
                activity.getCaptureSession().images.remove(image)
                loadImages()
            }
            .setNegativeButton("Cancel", null)
            .show()
    }

    override fun onResume() {
        super.onResume()
        loadImages()
    }
}

class ImageGridAdapter(
    private val onImageClick: (CapturedImage) -> Unit,
    private val onDeleteClick: (CapturedImage) -> Unit
) : RecyclerView.Adapter<ImageGridAdapter.ImageViewHolder>() {

    private var items: List<CapturedImage> = emptyList()

    fun submitList(list: List<CapturedImage>) {
        items = list
        notifyDataSetChanged()
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ImageViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_image_grid, parent, false)
        return ImageViewHolder(view)
    }

    override fun onBindViewHolder(holder: ImageViewHolder, position: Int) {
        holder.bind(items[position])
    }

    override fun getItemCount(): Int = items.size

    inner class ImageViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private val imageView: ImageView = itemView.findViewById(R.id.grid_image_view)
        private val label: TextView = itemView.findViewById(R.id.grid_image_label)

        fun bind(image: CapturedImage) {
            label.text = image.filename

            itemView.setOnClickListener { onImageClick(image) }
            itemView.setOnLongClickListener {
                onDeleteClick(image)
                true
            }
        }
    }
}
