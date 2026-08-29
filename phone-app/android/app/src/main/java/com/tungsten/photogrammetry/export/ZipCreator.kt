package com.tungsten.photogrammetry.export

import java.io.File
import java.io.FileOutputStream
import java.util.zip.ZipEntry
import java.util.zip.ZipOutputStream

object ZipCreator {

    fun createZip(
        outputFile: File,
        images: List<Pair<String, ByteArray>>,
        metadata: Map<String, String>
    ): File {
        ZipOutputStream(FileOutputStream(outputFile)).use { zipOut ->
            images.forEachIndexed { index, (filename, data) ->
                val entry = ZipEntry("images/$filename")
                zipOut.putNextEntry(entry)
                zipOut.write(data)
                zipOut.closeEntry()
            }

            metadata.forEach { (name, content) ->
                val entry = ZipEntry(name)
                zipOut.putNextEntry(entry)
                zipOut.write(content.toByteArray(Charsets.UTF_8))
                zipOut.closeEntry()
            }
        }

        return outputFile
    }
}
