package com.tungsten.photogrammetry.camera

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.RectF
import android.util.AttributeSet
import android.view.View
import kotlin.math.min

class CaptureGuideView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    private val circlePaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        style = Paint.Style.STROKE
        strokeWidth = 4f
        color = Color.argb(160, 255, 255, 255)
    }

    private val arcPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        style = Paint.Style.STROKE
        strokeWidth = 8f
        strokeCap = Paint.Cap.ROUND
    }

    private val textPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.WHITE
        textSize = 48f
        textAlign = Paint.Align.CENTER
        setShadowLayer(4f, 2f, 2f, Color.BLACK)
    }

    private val guideCircle = RectF()
    private val arcRect = RectF()

    private var rotationDegrees: Float = 0f
    private var targetDegrees: Float = 12f
    private var statusText: String = "Rotate your device 0.0\u00b0 around the object"

    private val requiredRotation: Float = 12f

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        val cx = width / 2f
        val cy = height / 2f
        val radius = min(cx, cy) * 0.6f

        guideCircle.set(cx - radius, cy - radius, cx + radius, cy + radius)
        canvas.drawOval(guideCircle, circlePaint)

        val margin = 20f
        arcRect.set(
            cx - radius - margin,
            cy - radius - margin,
            cx + radius + margin,
            cy + radius + margin
        )

        val progress = (rotationDegrees / requiredRotation).coerceIn(0f, 1f)

        arcPaint.color = when {
            progress < 0.5f -> Color.argb(200, 244, 67, 54)
            progress < 0.85f -> Color.argb(200, 255, 193, 7)
            else -> Color.argb(200, 76, 175, 80)
        }

        val sweepAngle = (progress * 360f).coerceAtMost(359f)
        canvas.drawArc(arcRect, -90f, sweepAngle, false, arcPaint)

        statusText = if (progress >= 1f) {
            "Ready to capture"
        } else {
            String.format("Rotate your device %.1f\u00b0 around the object", requiredRotation - rotationDegrees)
        }
        canvas.drawText(statusText, cx, cy + radius + margin + 80f, textPaint)
    }

    fun updateRotation(deltaDegrees: Float) {
        rotationDegrees = (rotationDegrees + deltaDegrees).coerceAtMost(requiredRotation * 1.5f)
        invalidate()
    }

    fun shouldCapture(): Boolean {
        return rotationDegrees >= requiredRotation
    }

    fun reset() {
        rotationDegrees = 0f
        invalidate()
    }

    fun setTargetDegrees(degrees: Float) {
        targetDegrees = degrees
        invalidate()
    }
}
