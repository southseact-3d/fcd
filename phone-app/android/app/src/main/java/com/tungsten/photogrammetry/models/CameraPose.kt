package com.tungsten.photogrammetry.models

data class CameraPose(
    val position: FloatArray = FloatArray(3),
    val rotation: FloatArray = FloatArray(4),
    val timestamp: Long = System.currentTimeMillis()
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is CameraPose) return false
        return position.contentEquals(other.position) &&
                rotation.contentEquals(other.rotation) &&
                timestamp == other.timestamp
    }

    override fun hashCode(): Int {
        var result = position.contentHashCode()
        result = 31 * result + rotation.contentHashCode()
        result = 31 * result + timestamp.hashCode()
        return result
    }
}
