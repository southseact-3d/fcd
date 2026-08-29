package com.tungsten.photogrammetry.sensors

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import com.tungsten.photogrammetry.models.CameraPose
import kotlin.math.sqrt

class ImuManager : SensorEventListener {

    private var sensorManager: SensorManager? = null
    private var rotationSensor: Sensor? = null
    private var linearAccelSensor: Sensor? = null

    private val currentRotation = FloatArray(4)
    private val lastRotation = FloatArray(4)
    private val position = FloatArray(3)
    private val velocity = FloatArray(3)
    private val linearAccel = FloatArray(3)

    private var lastTimestamp: Long = 0L
    private var rotationSinceLastCapture: Float = 0f

    private var isInitialized = false

    fun initialize(context: Context) {
        sensorManager = context.getSystemService(Context.SENSOR_SERVICE) as SensorManager

        rotationSensor = sensorManager?.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR)
        linearAccelSensor = sensorManager?.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION)

        isInitialized = rotationSensor != null
    }

    fun start() {
        rotationSensor?.let { sensor ->
            sensorManager?.registerListener(
                this,
                sensor,
                SensorManager.SENSOR_DELAY_GAME
            )
        }
        linearAccelSensor?.let { sensor ->
            sensorManager?.registerListener(
                this,
                sensor,
                SensorManager.SENSOR_DELAY_GAME
            )
        }
    }

    fun stop() {
        sensorManager?.unregisterListener(this)
    }

    fun getCurrentPose(): CameraPose {
        val rotation = FloatArray(4)
        System.arraycopy(currentRotation, 0, rotation, 0, 4)

        return CameraPose(
            position = position.copyOf(),
            rotation = rotation,
            timestamp = System.currentTimeMillis()
        )
    }

    fun getRotationSinceLast(): Float {
        val delta = rotationSinceLastCapture
        rotationSinceLastCapture = 0f
        return delta
    }

    fun reset() {
        position.fill(0f)
        velocity.fill(0f)
        linearAccel.fill(0f)
        currentRotation.fill(0f)
        lastRotation.fill(0f)
        rotationSinceLastCapture = 0f
        lastTimestamp = 0L
    }

    override fun onSensorChanged(event: SensorEvent) {
        when (event.sensor.type) {
            Sensor.TYPE_ROTATION_VECTOR -> {
                val qx = event.values[0]
                val qy = event.values[1]
                val qz = event.values[2]
                val qw = event.values[3]

                if (!isRotationInitialized()) {
                    currentRotation[0] = qw
                    currentRotation[1] = qx
                    currentRotation[2] = qy
                    currentRotation[3] = qz
                    lastRotation[0] = qw
                    lastRotation[1] = qx
                    lastRotation[2] = qy
                    lastRotation[3] = qz
                    lastTimestamp = event.timestamp
                    isInitialized = true
                    return
                }

                val deltaAngle = angleBetweenQuaternions(lastRotation, event.values)
                rotationSinceLastCapture += deltaAngle

                lastRotation[0] = currentRotation[0]
                lastRotation[1] = currentRotation[1]
                lastRotation[2] = currentRotation[2]
                lastRotation[3] = currentRotation[3]

                currentRotation[0] = qw
                currentRotation[1] = qx
                currentRotation[2] = qy
                currentRotation[3] = qz
            }

            Sensor.TYPE_LINEAR_ACCELERATION -> {
                if (lastTimestamp == 0L) {
                    lastTimestamp = event.timestamp
                    return
                }

                val dt = (event.timestamp - lastTimestamp) / 1_000_000_000.0f

                linearAccel[0] = event.values[0]
                linearAccel[1] = event.values[1]
                linearAccel[2] = event.values[2]

                velocity[0] += linearAccel[0] * dt
                velocity[1] += linearAccel[1] * dt
                velocity[2] += linearAccel[2] * dt

                val decay = 0.98f
                velocity[0] *= decay
                velocity[1] *= decay
                velocity[2] *= decay

                position[0] += velocity[0] * dt
                position[1] += velocity[1] * dt
                position[2] += velocity[2] * dt

                lastTimestamp = event.timestamp
            }
        }
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {}

    private fun isRotationInitialized(): Boolean {
        return currentRotation.any { it != 0f }
    }

    private fun angleBetweenQuaternions(q1: FloatArray, q2: FloatArray): Float {
        val dot = q1[0] * q2[0] + q1[1] * q2[1] + q1[2] * q2[2] + q1[3] * q2[3]
        val clampedDot = dot.coerceIn(-1f, 1f)
        val angle = 2f * kotlin.math.acos(kotlin.math.abs(clampedDot))
        return Math.toDegrees(angle.toDouble()).toFloat()
    }
}
