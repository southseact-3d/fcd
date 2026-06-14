"""
Resin slicer output file writers for various printer formats.

Supports writing resin slicer files in multiple formats:
- CTB: ChiTuBox format (most common, LZ4 compressed + XOR encrypted)
- GOO: Elegoo format (similar to CTB with different header)
- SL1: Prusa SLA format (ZIP archive with PNG layers)
- Photon: Anycubic Photon format (binary with PNG layers)

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import io
import math
import os
import struct
import zipfile
import zlib
from pathlib import Path
from typing import Any, Dict, List, Optional, Sequence, Tuple, Union


# ============================================================================
# Constants
# ============================================================================

CTB_MAGIC = b"CTB\x00"
CTB_VERSION = 4
CTB_XOR_KEY = b"TDz\x00"

GOO_MAGIC = b"GOO\x01"
GOO_VERSION = 1

PHOTON_MAGIC = b"PHOTON"
PHOTON_VERSION = 1

THUMBNAIL_SIZE = 200

DEFAULT_LAYER_WIDTH = 1920
DEFAULT_LAYER_HEIGHT = 1080
DEFAULT_LAYER_HEIGHT_MM = 0.05


# ============================================================================
# Helper functions
# ============================================================================


def _make_png_chunk(chunk_type: bytes, data: bytes) -> bytes:
    """Create a PNG chunk with length prefix and CRC32 suffix."""
    length = struct.pack(">I", len(data))
    body = chunk_type + data
    crc = struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)
    return length + body + crc


def create_png_gray(width: int, height: int, pixels: bytes) -> bytes:
    """Create a grayscale PNG image from raw 8-bit pixel data.

    Args:
        width: Image width in pixels.
        height: Image height in pixels.
        pixels: Raw grayscale pixel data, one byte per pixel, row-major order.

    Returns:
        Complete PNG file data as bytes.
    """
    signature = b"\x89PNG\r\n\x1a\n"

    ihdr_data = struct.pack(">IIBBBBB", width, height, 8, 0, 0, 0, 0)
    ihdr_chunk = _make_png_chunk(b"IHDR", ihdr_data)

    raw_data = bytearray()
    for y in range(height):
        raw_data.append(0)  # filter byte: None
        row_start = y * width
        raw_data.extend(pixels[row_start : row_start + width])

    compressed = zlib.compress(bytes(raw_data), 9)
    idat_chunk = _make_png_chunk(b"IDAT", compressed)

    iend_chunk = _make_png_chunk(b"IEND", b"")
    return signature + ihdr_chunk + idat_chunk + iend_chunk


def create_png_rgba(width: int, height: int, pixels: bytes) -> bytes:
    """Create an RGBA PNG image from raw 32-bit pixel data.

    Args:
        width: Image width in pixels.
        height: Image height in pixels.
        pixels: Raw RGBA pixel data, 4 bytes per pixel, row-major order.

    Returns:
        Complete PNG file data as bytes.
    """
    signature = b"\x89PNG\r\n\x1a\n"

    ihdr_data = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    ihdr_chunk = _make_png_chunk(b"IHDR", ihdr_data)

    raw_data = bytearray()
    stride = width * 4
    for y in range(height):
        raw_data.append(0)  # filter byte: None
        row_start = y * stride
        raw_data.extend(pixels[row_start : row_start + stride])

    compressed = zlib.compress(bytes(raw_data), 9)
    idat_chunk = _make_png_chunk(b"IDAT", compressed)

    iend_chunk = _make_png_chunk(b"IEND", b"")
    return signature + ihdr_chunk + idat_chunk + iend_chunk


def xor_encrypt(data: bytes, key: bytes) -> bytes:
    """XOR encrypt or decrypt data with a repeating key.

    Symmetric operation: encrypting twice with the same key returns the
    original data.

    Args:
        data: Input bytes.
        key: XOR key.  Repeats cyclically over *data*.

    Returns:
        Transformed bytes.
    """
    if not key:
        return data
    key_len = len(key)
    out = bytearray(len(data))
    for i in range(len(data)):
        out[i] = data[i] ^ key[i % key_len]
    return bytes(out)


def pack_string(s: str, length: int) -> bytes:
    """Encode a string to a fixed-length, null-padded byte array."""
    encoded = s.encode("ascii")[:length]
    return encoded + b"\x00" * (length - len(encoded))


def _bitmap_to_flat_bytes(bitmap: List[List[int]]) -> bytes:
    """Convert a 2-D bitmap (list of rows) to a flat byte string."""
    out = bytearray()
    for row in bitmap:
        out.extend(bytes(max(0, min(255, v)) for v in row))
    return bytes(out)


def _downsample_bitmap(bitmap: List[List[int]], dst_w: int, dst_h: int) -> bytes:
    """Nearest-neighbour downsample a 2-D bitmap and return flat grayscale bytes."""
    src_h = len(bitmap)
    src_w = len(bitmap[0]) if src_h > 0 else 0
    if src_w == 0 or src_h == 0:
        return b"\x80" * (dst_w * dst_h)

    out = bytearray(dst_w * dst_h)
    x_scale = src_w / dst_w
    y_scale = src_h / dst_h
    for dy in range(dst_h):
        sy = min(int(dy * y_scale), src_h - 1)
        row = bitmap[sy]
        base = dy * dst_w
        for dx in range(dst_w):
            sx = min(int(dx * x_scale), src_w - 1)
            out[base + dx] = row[sx]
    return bytes(out)


# ============================================================================
# LZ4 Block Compression
# ============================================================================


class LZ4Compressor:
    """Pure-Python LZ4 block compressor.

    Produces raw LZ4 block data (no frame headers) suitable for embedding
    inside CTB layer records.  The compression ratio is modest but the
    encoder is fast enough for typical MSLA layer images.

    The algorithm scans each position for a matching back-reference within
    the last 4 096 bytes (configurable) and emits standard LZ4 tokens:
    ``[token][extra-len][literals][offset LE16]``.
    """

    MIN_MATCH = 4
    DEFAULT_MAX_DIST = 4096

    def __init__(self, max_distance: int = DEFAULT_MAX_DIST) -> None:
        self.max_distance = max_distance

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def compress(self, data: bytes) -> bytes:
        """Compress *data* and return the raw LZ4 block."""
        if not data:
            return b""

        n = len(data)
        output = bytearray()
        i = 0

        while i < n:
            match_len, match_off = self._find_match(data, i)

            # Collect preceding literals
            lit_start = i
            if match_len >= self.MIN_MATCH:
                literal_len = 0
                literals = b""
            else:
                i += 1
                while i < n:
                    ml, _ = self._find_match(data, i)
                    if ml >= self.MIN_MATCH:
                        break
                    i += 1
                    if i - lit_start >= 0x7FFF:
                        break
                literal_len = i - lit_start
                literals = data[lit_start:i]

                match_len, match_off = self._find_match(data, i)
                if match_len < self.MIN_MATCH:
                    match_len = 0

            # --- token ---
            lit_nib = min(literal_len, 15)
            mat_nib = min(match_len - 4, 15) if match_len >= 4 else 0
            output.append((lit_nib << 4) | mat_nib)

            # --- extra literal length ---
            if literal_len >= 15:
                rem = literal_len - 15
                while rem >= 255:
                    output.append(255)
                    rem -= 255
                output.append(rem)

            # --- literal bytes ---
            output.extend(literals)

            # --- extra match length + offset ---
            if match_len >= 4:
                if match_len - 4 >= 15:
                    rem = match_len - 4 - 15
                    while rem >= 255:
                        output.append(255)
                        rem -= 255
                    output.append(rem)
                output.extend(struct.pack("<H", match_off))
                i += match_len

        return bytes(output)

    # ------------------------------------------------------------------
    # Match finder
    # ------------------------------------------------------------------

    def _find_match(self, data: bytes, pos: int) -> Tuple[int, int]:
        """Return ``(length, offset)`` of the longest match at *pos*."""
        best_len = 0
        best_off = 0
        limit = min(pos, self.max_distance)

        for dist in range(1, limit + 1):
            ref = pos - dist
            length = 0
            while (
                pos + length < len(data)
                and ref + length < pos
                and data[pos + length] == data[ref + length]
            ):
                length += 1
                if length >= 0x0F + 4 + 255:
                    break
            if length > best_len:
                best_len = length
                best_off = dist

        return best_len, best_off


# ============================================================================
# Thumbnail Generator
# ============================================================================


class ThumbnailGenerator:
    """Generate small preview thumbnails from slice layer data."""

    def __init__(self, size: int = THUMBNAIL_SIZE) -> None:
        self.size = size

    def generate_from_bitmaps(self, bitmaps: List[List[List[int]]]) -> bytes:
        """Return a PNG thumbnail built from the first (bottom) layer.

        Args:
            bitmaps: List of 2-D bitmaps (each a list of rows of ints 0-255).

        Returns:
            PNG image data.
        """
        if not bitmaps:
            return self._empty_thumb()
        flat = _downsample_bitmap(bitmaps[0], self.size, self.size)
        return create_png_gray(self.size, self.size, flat)

    def generate_from_layer_bitmaps(
        self,
        bitmaps: List[List[List[int]]],
        target_w: int = 0,
        target_h: int = 0,
    ) -> bytes:
        """Generate thumbnail with explicit target dimensions."""
        tw = target_w or self.size
        th = target_h or self.size
        if not bitmaps:
            return self._empty_thumb()
        flat = _downsample_bitmap(bitmaps[0], tw, th)
        return create_png_gray(tw, th, flat)

    def _empty_thumb(self) -> bytes:
        pixels = b"\x80" * (self.size * self.size)
        return create_png_gray(self.size, self.size, pixels)


# ============================================================================
# CTB Writer — ChiTuBox format
# ============================================================================


class CTBWriter:
    """Writer for ChiTuBox CTB v4 files.

    CTB is the most widely supported resin slicer format.  The file layout
    is::

        [Header]              fixed-size parameter block + thumbnail
        [Layer offset table]  uint32 per layer
        [Layer records]       per-layer header + LZ4-compressed image

    Layer images are LZ4-block-compressed and then XOR-encrypted with a
    fixed key before being written to disk.

    The header stores resolution, exposure parameters, lift geometry and
    an optional thumbnail image (typically a small PNG).
    """

    HEADER_BASE_SIZE = 80

    def __init__(self) -> None:
        self.lz4 = LZ4Compressor()

    # ------------------------------------------------------------------
    # Public entry point
    # ------------------------------------------------------------------

    def write(
        self,
        bitmaps: List[List[List[int]]],
        settings: Dict[str, Any],
        output_path: str,
    ) -> None:
        """Write a CTB v4 file.

        Args:
            bitmaps: Per-layer 2-D bitmaps (list of rows of ints 0-255).
            settings: Dictionary with printer/print parameters.
            output_path: Destination file path.
        """
        res_x = settings.get("resolution_x", DEFAULT_LAYER_WIDTH)
        res_y = settings.get("resolution_y", DEFAULT_LAYER_HEIGHT)
        layer_height = settings.get("layer_height", DEFAULT_LAYER_HEIGHT_MM)
        exposure_time = settings.get("exposure_time", 2.5)
        bottom_exposure = settings.get("bottom_exposure_time", 30.0)
        bottom_layers = settings.get("bottom_layers", 5)
        off_time = settings.get("light_off_delay", 0.0)
        lift_distance = settings.get("lift_height", 5.0)
        lift_speed = settings.get("lift_speed", 1.0)
        retract_speed = settings.get("retract_speed", 1.0)
        volume = settings.get("volume_ml", 0.0)
        cost = settings.get("cost", 0.0)
        currency = settings.get("currency", "USD")
        anti_alias = 1 if settings.get("anti_aliasing", True) else 0

        num_layers = len(bitmaps)

        thumb_gen = ThumbnailGenerator()
        thumbnail = thumb_gen.generate_from_bitmaps(bitmaps)

        buf = bytearray()

        self._write_header(
            buf,
            res_x,
            res_y,
            num_layers,
            layer_height,
            exposure_time,
            bottom_exposure,
            bottom_layers,
            off_time,
            lift_distance,
            lift_speed,
            retract_speed,
            volume,
            cost,
            currency,
            anti_alias,
            thumbnail,
        )

        # Layer offset table (8 bytes per entry: 4-byte offset + 4-byte size)
        table_offset = len(buf)
        buf.extend(b"\x00" * (num_layers * 8))

        layer_offsets = []
        layer_sizes = []
        for i, bm in enumerate(bitmaps):
            layer_offsets.append(len(buf))
            size = self._write_layer(
                buf,
                bm,
                res_x,
                res_y,
                i,
                layer_height,
                exposure_time,
                bottom_exposure,
                bottom_layers,
            )
            layer_sizes.append(size)

        # Patch the offset table
        for i in range(num_layers):
            struct.pack_into("<I", buf, table_offset + i * 8, layer_offsets[i])
            struct.pack_into("<I", buf, table_offset + i * 8 + 4, layer_sizes[i])

        os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
        with open(output_path, "wb") as fh:
            fh.write(buf)

    # ------------------------------------------------------------------
    # Header
    # ------------------------------------------------------------------

    def _write_header(
        self,
        buf: bytearray,
        res_x: int,
        res_y: int,
        num_layers: int,
        layer_height: float,
        exposure_time: float,
        bottom_exposure: float,
        bottom_layers: int,
        off_time: float,
        lift_distance: float,
        lift_speed: float,
        retract_speed: float,
        volume: float,
        cost: float,
        currency: str,
        anti_alias: int,
        thumbnail: bytes,
    ) -> None:
        header_size = self.HEADER_BASE_SIZE + len(thumbnail)

        buf.extend(CTB_MAGIC)
        buf.extend(struct.pack("<I", CTB_VERSION))
        buf.extend(struct.pack("<I", header_size))
        buf.extend(struct.pack("<I", res_x))
        buf.extend(struct.pack("<I", res_y))
        buf.extend(struct.pack("<I", int(layer_height * 1000)))
        buf.extend(struct.pack("<I", int(exposure_time * 1000)))
        buf.extend(struct.pack("<I", int(bottom_exposure * 1000)))
        buf.extend(struct.pack("<I", bottom_layers))
        buf.extend(struct.pack("<I", int(off_time * 1000)))
        buf.extend(struct.pack("<I", int(lift_distance * 1000)))
        buf.extend(struct.pack("<I", int(lift_speed * 1000)))
        buf.extend(struct.pack("<I", int(retract_speed * 1000)))
        buf.extend(struct.pack("<I", int(volume * 1000)))
        buf.extend(struct.pack("<I", int(cost * 100000)))
        buf.extend(pack_string(currency, 4))
        buf.extend(struct.pack("<I", anti_alias))
        buf.extend(struct.pack("<I", num_layers))
        buf.extend(struct.pack("<I", len(thumbnail)))
        buf.extend(thumbnail)

    # ------------------------------------------------------------------
    # Layer record
    # ------------------------------------------------------------------

    def _write_layer(
        self,
        buf: bytearray,
        bitmap: List[List[int]],
        res_x: int,
        res_y: int,
        layer_index: int,
        layer_height: float,
        exposure_time: float,
        bottom_exposure: float,
        bottom_layers: int,
    ) -> int:
        flat = _bitmap_to_flat_bytes(bitmap)
        compressed = self.lz4.compress(flat)
        encrypted = xor_encrypt(compressed, CTB_XOR_KEY)

        exp_time = bottom_exposure if layer_index < bottom_layers else exposure_time
        z_um = int((layer_index + 1) * layer_height * 1000)

        record_start = len(buf)
        buf.extend(struct.pack("<I", len(encrypted)))
        buf.extend(struct.pack("<I", 0))
        buf.extend(struct.pack("<I", int(exp_time * 1000)))
        buf.extend(struct.pack("<I", z_um))
        buf.extend(encrypted)
        return len(buf) - record_start


# ============================================================================
# GOO Writer — Elegoo format
# ============================================================================


class GOOWriter:
    """Writer for Elegoo GOO format files.

    The GOO format is structurally similar to CTB but uses different magic
    bytes, a slightly rearranged header, and does **not** apply XOR
    encryption to the LZ4-compressed layer data.
    """

    def __init__(self) -> None:
        self.lz4 = LZ4Compressor()

    # ------------------------------------------------------------------

    def write(
        self,
        bitmaps: List[List[List[int]]],
        settings: Dict[str, Any],
        output_path: str,
    ) -> None:
        """Write a GOO format file.

        Args:
            bitmaps: Per-layer 2-D bitmaps.
            settings: Printer/print parameters.
            output_path: Destination file path.
        """
        res_x = settings.get("resolution_x", DEFAULT_LAYER_WIDTH)
        res_y = settings.get("resolution_y", DEFAULT_LAYER_HEIGHT)
        layer_height = settings.get("layer_height", DEFAULT_LAYER_HEIGHT_MM)
        exposure_time = settings.get("exposure_time", 2.5)
        bottom_exposure = settings.get("bottom_exposure_time", 30.0)
        bottom_layers = settings.get("bottom_layers", 5)
        off_time = settings.get("light_off_delay", 0.0)
        lift_distance = settings.get("lift_height", 5.0)
        lift_speed = settings.get("lift_speed", 1.0)
        retract_speed = settings.get("retract_speed", 1.0)

        num_layers = len(bitmaps)

        thumb_gen = ThumbnailGenerator()
        thumbnail = thumb_gen.generate_from_bitmaps(bitmaps)

        buf = bytearray()

        self._write_header(
            buf,
            res_x,
            res_y,
            num_layers,
            layer_height,
            exposure_time,
            bottom_exposure,
            bottom_layers,
            off_time,
            lift_distance,
            lift_speed,
            retract_speed,
            thumbnail,
        )

        for i, bm in enumerate(bitmaps):
            self._write_layer(
                buf,
                bm,
                i,
                layer_height,
                exposure_time,
                bottom_exposure,
                bottom_layers,
            )

        os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
        with open(output_path, "wb") as fh:
            fh.write(buf)

    # ------------------------------------------------------------------

    def _write_header(
        self,
        buf: bytearray,
        res_x: int,
        res_y: int,
        num_layers: int,
        layer_height: float,
        exposure_time: float,
        bottom_exposure: float,
        bottom_layers: int,
        off_time: float,
        lift_distance: float,
        lift_speed: float,
        retract_speed: float,
        thumbnail: bytes,
    ) -> None:
        buf.extend(GOO_MAGIC)
        buf.extend(struct.pack("<I", GOO_VERSION))
        buf.extend(struct.pack("<I", res_x))
        buf.extend(struct.pack("<I", res_y))
        buf.extend(struct.pack("<I", int(layer_height * 1000)))
        buf.extend(struct.pack("<I", int(exposure_time * 1000)))
        buf.extend(struct.pack("<I", int(bottom_exposure * 1000)))
        buf.extend(struct.pack("<I", bottom_layers))
        buf.extend(struct.pack("<I", int(off_time * 1000)))
        buf.extend(struct.pack("<I", int(lift_distance * 1000)))
        buf.extend(struct.pack("<I", int(lift_speed * 1000)))
        buf.extend(struct.pack("<I", int(retract_speed * 1000)))
        buf.extend(struct.pack("<I", num_layers))
        buf.extend(struct.pack("<I", len(thumbnail)))
        buf.extend(thumbnail)

    def _write_layer(
        self,
        buf: bytearray,
        bitmap: List[List[int]],
        layer_index: int,
        layer_height: float,
        exposure_time: float,
        bottom_exposure: float,
        bottom_layers: int,
    ) -> None:
        flat = _bitmap_to_flat_bytes(bitmap)
        compressed = self.lz4.compress(flat)

        exp_time = bottom_exposure if layer_index < bottom_layers else exposure_time
        z_um = int((layer_index + 1) * layer_height * 1000)

        buf.extend(struct.pack("<I", len(compressed)))
        buf.extend(struct.pack("<I", int(exp_time * 1000)))
        buf.extend(struct.pack("<I", z_um))
        buf.extend(compressed)


# ============================================================================
# SL1 Writer — Prusa SLA format
# ============================================================================


class SL1Writer:
    """Writer for Prusa SL1 format files.

    SL1 files are ZIP archives containing:
    - ``config.ini`` with all print parameters (INI-like format)
    - ``SLA/layerNNNNN.png`` grayscale PNG images, one per layer
    - ``thumbnails/thumbnail.png`` a small preview image

    This format is human-readable and easy to inspect.
    """

    def write(
        self,
        bitmaps: List[List[List[int]]],
        settings: Dict[str, Any],
        output_path: str,
    ) -> None:
        """Write an SL1 format ZIP file.

        Args:
            bitmaps: Per-layer 2-D bitmaps.
            settings: Printer/print parameters.
            output_path: Destination .sl1 file path.
        """
        res_x = settings.get("resolution_x", DEFAULT_LAYER_WIDTH)
        res_y = settings.get("resolution_y", DEFAULT_LAYER_HEIGHT)
        layer_height = settings.get("layer_height", DEFAULT_LAYER_HEIGHT_MM)
        exposure_time = settings.get("exposure_time", 2.5)
        bottom_exposure = settings.get("bottom_exposure_time", 30.0)
        bottom_layers = settings.get("bottom_layers", 5)
        off_time = settings.get("light_off_delay", 0.0)
        display_width = settings.get("display_width_mm", 68.04)
        display_height = settings.get("display_height_mm", 120.96)
        printer_height = settings.get("printer_height_mm", 150.0)

        num_layers = len(bitmaps)

        thumb_gen = ThumbnailGenerator()
        thumbnail = thumb_gen.generate_from_bitmaps(bitmaps)

        os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
        with zipfile.ZipFile(output_path, "w", zipfile.ZIP_DEFLATED) as zf:
            # --- config.ini ---
            config = self._build_config(
                res_x,
                res_y,
                num_layers,
                layer_height,
                exposure_time,
                bottom_exposure,
                bottom_layers,
                off_time,
                display_width,
                display_height,
                printer_height,
            )
            zf.writestr("config.ini", config)

            # --- layer PNGs ---
            for i, bm in enumerate(bitmaps):
                flat = _bitmap_to_flat_bytes(bm)
                png = create_png_gray(res_x, res_y, flat)
                zf.writestr(f"SLA/layer{i:05d}.png", png)

            # --- thumbnail ---
            zf.writestr("thumbnails/thumbnail.png", thumbnail)

    # ------------------------------------------------------------------

    @staticmethod
    def _build_config(
        res_x: int,
        res_y: int,
        num_layers: int,
        layer_height: float,
        exposure_time: float,
        bottom_exposure: float,
        bottom_layers: int,
        off_time: float,
        display_width: float,
        display_height: float,
        printer_height: float,
    ) -> str:
        lines = [
            "[sla]",
            f"resolution_x = {res_x}",
            f"resolution_y = {res_y}",
            f"display_width = {display_width}",
            f"display_height = {display_height}",
            f"height = {printer_height}",
            f"layer_height = {layer_height}",
            f"exposure_time = {exposure_time}",
            f"bottom_exposure_time = {bottom_exposure}",
            f"bottom_layers = {bottom_layers}",
            f"light_off_time = {off_time}",
            f"num_layers = {num_layers}",
        ]
        return "\n".join(lines) + "\n"


# ============================================================================
# Photon Writer — Anycubic Photon format
# ============================================================================


class PhotonWriter:
    """Writer for Anycubic Photon (.photon) format files.

    The Photon binary format has a fixed header followed by per-layer
    PNG image data.  Each layer is stored as a complete PNG image rather
    than a compressed bitmap, making the file larger but self-contained.
    """

    HEADER_SIZE = 68

    def write(
        self,
        bitmaps: List[List[List[int]]],
        settings: Dict[str, Any],
        output_path: str,
    ) -> None:
        """Write a Photon format file.

        Args:
            bitmaps: Per-layer 2-D bitmaps.
            settings: Printer/print parameters.
            output_path: Destination .photon file path.
        """
        res_x = settings.get("resolution_x", DEFAULT_LAYER_WIDTH)
        res_y = settings.get("resolution_y", DEFAULT_LAYER_HEIGHT)
        layer_height = settings.get("layer_height", DEFAULT_LAYER_HEIGHT_MM)
        exposure_time = settings.get("exposure_time", 2.5)
        bottom_exposure = settings.get("bottom_exposure_time", 30.0)
        bottom_layers = settings.get("bottom_layers", 5)
        off_time = settings.get("light_off_delay", 0.0)
        lift_distance = settings.get("lift_height", 5.0)
        lift_speed = settings.get("lift_speed", 1.0)
        retract_speed = settings.get("retract_speed", 1.0)

        num_layers = len(bitmaps)

        thumb_gen = ThumbnailGenerator()
        thumbnail = thumb_gen.generate_from_bitmaps(bitmaps)

        buf = bytearray()

        self._write_header(
            buf,
            res_x,
            res_y,
            num_layers,
            layer_height,
            exposure_time,
            bottom_exposure,
            bottom_layers,
            off_time,
            lift_distance,
            lift_speed,
            retract_speed,
            thumbnail,
        )

        for bm in bitmaps:
            flat = _bitmap_to_flat_bytes(bm)
            png = create_png_gray(res_x, res_y, flat)
            buf.extend(struct.pack("<I", len(png)))
            buf.extend(png)

        os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
        with open(output_path, "wb") as fh:
            fh.write(buf)

    # ------------------------------------------------------------------

    def _write_header(
        self,
        buf: bytearray,
        res_x: int,
        res_y: int,
        num_layers: int,
        layer_height: float,
        exposure_time: float,
        bottom_exposure: float,
        bottom_layers: int,
        off_time: float,
        lift_distance: float,
        lift_speed: float,
        retract_speed: float,
        thumbnail: bytes,
    ) -> None:
        buf.extend(PHOTON_MAGIC)
        buf.extend(struct.pack("<I", PHOTON_VERSION))
        buf.extend(struct.pack("<I", res_x))
        buf.extend(struct.pack("<I", res_y))
        buf.extend(struct.pack("<I", int(layer_height * 1000)))
        buf.extend(struct.pack("<I", int(exposure_time * 1000)))
        buf.extend(struct.pack("<I", int(bottom_exposure * 1000)))
        buf.extend(struct.pack("<I", bottom_layers))
        buf.extend(struct.pack("<I", int(off_time * 1000)))
        buf.extend(struct.pack("<I", int(lift_distance * 1000)))
        buf.extend(struct.pack("<I", int(lift_speed * 1000)))
        buf.extend(struct.pack("<I", int(retract_speed * 1000)))
        buf.extend(struct.pack("<I", num_layers))
        buf.extend(struct.pack("<I", len(thumbnail)))
        buf.extend(thumbnail)


# ============================================================================
# Unified output interface
# ============================================================================


class ResinOutput:
    """Unified dispatcher for writing resin slicer output files.

    Usage::

        ResinOutput.write(bitmaps, settings, "output.ctb", fmt="ctb")

    Supported *fmt* values: ``'ctb'``, ``'goo'``, ``'sl1'``, ``'photon'``.
    """

    FORMATS: Dict[str, type] = {
        "ctb": CTBWriter,
        "goo": GOOWriter,
        "sl1": SL1Writer,
        "photon": PhotonWriter,
    }

    @classmethod
    def write(
        cls,
        bitmaps: List[List[List[int]]],
        settings: Dict[str, Any],
        output_path: str,
        fmt: str = "ctb",
    ) -> None:
        """Write slicer output in the specified format.

        Args:
            bitmaps: Per-layer 2-D bitmaps (list of rows of ints 0-255).
            settings: Printer and print settings.
            output_path: Destination file path.
            fmt: Output format identifier.

        Raises:
            ValueError: If *fmt* is not a supported format.
        """
        fmt_lower = fmt.lower()
        if fmt_lower not in cls.FORMATS:
            raise ValueError(
                f"Unsupported format '{fmt}'. "
                f"Choose from: {', '.join(sorted(cls.FORMATS))}"
            )

        writer = cls.FORMATS[fmt_lower]()
        writer.write(bitmaps, settings, output_path)

    @classmethod
    def supported_formats(cls) -> List[str]:
        """Return list of supported format identifiers."""
        return sorted(cls.FORMATS.keys())


# ============================================================================
# Settings helpers
# ============================================================================

# Default settings dictionary that callers can copy and modify.
DEFAULT_SETTINGS: Dict[str, Any] = {
    "resolution_x": 1440,
    "resolution_y": 2560,
    "display_width_mm": 68.04,
    "display_height_mm": 120.96,
    "printer_height_mm": 150.0,
    "layer_height": 0.05,
    "exposure_time": 2.5,
    "bottom_exposure_time": 30.0,
    "bottom_layers": 5,
    "light_off_delay": 0.0,
    "lift_height": 5.0,
    "lift_speed": 1.0,
    "retract_speed": 1.0,
    "volume_ml": 0.0,
    "cost": 0.0,
    "currency": "USD",
    "anti_aliasing": True,
}


def make_settings(**overrides: Any) -> Dict[str, Any]:
    """Return a copy of DEFAULT_SETTINGS with *overrides* applied.

    This is a convenience for callers that do not want to import or
    depend on the ``ResinSettings`` dataclass from ``resin_slicer``.
    """
    settings = dict(DEFAULT_SETTINGS)
    settings.update(overrides)
    return settings


# ============================================================================
# LZ4 Decompression (for validation / round-trip testing)
# ============================================================================


class LZ4Decompressor:
    """Pure-Python LZ4 block decompressor.

    Decompresses raw LZ4 block data produced by :class:`LZ4Compressor` or
    other conforming encoders.  Used primarily for round-trip validation
    and debugging.
    """

    def decompress(self, data: bytes, uncompressed_size: int = 0) -> bytes:
        """Decompress an LZ4 block.

        Args:
            data: Compressed LZ4 block bytes.
            uncompressed_size: Expected output size.  If *0* the
                decompressor reads until the input is exhausted.

        Returns:
            Decompressed bytes.
        """
        output = bytearray()
        src = data
        si = 0
        n = len(src)

        while si < n:
            token = src[si]
            si += 1

            # Decode literal length
            lit_len = (token >> 4) & 0x0F
            if lit_len == 15:
                while si < n:
                    extra = src[si]
                    si += 1
                    lit_len += extra
                    if extra != 255:
                        break

            # Copy literals
            end = min(si + lit_len, n)
            output.extend(src[si:end])
            si = end

            if si >= n:
                break

            # Decode match offset (2 bytes LE)
            if si + 2 > n:
                break
            offset = struct.unpack_from("<H", src, si)[0]
            si += 2

            if offset == 0:
                raise ValueError("LZ4 decompressor: zero offset encountered")

            # Decode match length
            match_len = (token & 0x0F) + 4
            if (token & 0x0F) == 15:
                while si < n:
                    extra = src[si]
                    si += 1
                    match_len += extra
                    if extra != 255:
                        break

            # Copy match bytes (handles overlapping matches correctly)
            src_pos = len(output) - offset
            for j in range(match_len):
                output.append(output[src_pos + j])

        return bytes(output)


# ============================================================================
# Format detection
# ============================================================================


def detect_format(file_path: str) -> Optional[str]:
    """Detect the resin slicer file format from the magic bytes.

    Args:
        file_path: Path to the file to inspect.

    Returns:
        Format string (``'ctb'``, ``'goo'``, ``'sl1'``, ``'photon'``)
        or *None* if the format cannot be identified.
    """
    with open(file_path, "rb") as fh:
        header = fh.read(8)

    if len(header) >= 4 and header[:4] == CTB_MAGIC:
        return "ctb"
    if len(header) >= 4 and header[:4] == GOO_MAGIC:
        return "goo"
    if len(header) >= 6 and header[:6] == PHOTON_MAGIC:
        return "photon"

    # SL1 is a ZIP archive; verify the PK signature then check for config.ini
    if len(header) >= 4 and header[:4] == b"PK\x03\x04":
        try:
            with zipfile.ZipFile(file_path, "r") as zf:
                names = zf.namelist()
                if "config.ini" in names:
                    return "sl1"
        except (zipfile.BadZipFile, OSError):
            pass

    return None


# ============================================================================
# CTB header reader (for inspection / debugging)
# ============================================================================


def read_ctb_header(file_path: str) -> Dict[str, Any]:
    """Read and parse the header of a CTB v4 file.

    This is a diagnostic helper that returns the header fields as a
    dictionary.  It does **not** decompress layer data.

    Args:
        file_path: Path to a CTB file.

    Returns:
        Dictionary of header fields.

    Raises:
        ValueError: If the file does not start with a valid CTB magic.
    """
    with open(file_path, "rb") as fh:
        raw = fh.read(80)

    if raw[:4] != CTB_MAGIC:
        raise ValueError(f"Not a CTB file: bad magic {raw[:4]!r}")

    fields: Dict[str, Any] = {}
    fields["magic"] = raw[:4]
    fields["version"] = struct.unpack_from("<I", raw, 4)[0]
    fields["header_size"] = struct.unpack_from("<I", raw, 8)[0]
    fields["resolution_x"] = struct.unpack_from("<I", raw, 12)[0]
    fields["resolution_y"] = struct.unpack_from("<I", raw, 16)[0]
    fields["layer_height_um"] = struct.unpack_from("<I", raw, 20)[0]
    fields["exposure_time_ms"] = struct.unpack_from("<I", raw, 24)[0]
    fields["bottom_exposure_ms"] = struct.unpack_from("<I", raw, 28)[0]
    fields["bottom_layers"] = struct.unpack_from("<I", raw, 32)[0]
    fields["off_time_ms"] = struct.unpack_from("<I", raw, 36)[0]
    fields["lift_distance_um"] = struct.unpack_from("<I", raw, 40)[0]
    fields["lift_speed_um_s"] = struct.unpack_from("<I", raw, 44)[0]
    fields["retract_speed_um_s"] = struct.unpack_from("<I", raw, 48)[0]
    fields["volume_ul"] = struct.unpack_from("<I", raw, 52)[0]
    fields["cost_millicents"] = struct.unpack_from("<I", raw, 56)[0]
    fields["currency"] = raw[60:64].rstrip(b"\x00").decode("ascii", errors="replace")
    fields["anti_aliasing"] = struct.unpack_from("<I", raw, 64)[0]
    fields["num_layers"] = struct.unpack_from("<I", raw, 68)[0]
    thumb_size = struct.unpack_from("<I", raw, 72)[0]
    fields["thumbnail_size"] = thumb_size

    # Convert micrometres / milliseconds back to mm / seconds
    fields["layer_height_mm"] = fields["layer_height_um"] / 1000.0
    fields["exposure_time_s"] = fields["exposure_time_ms"] / 1000.0
    fields["bottom_exposure_s"] = fields["bottom_exposure_ms"] / 1000.0
    fields["off_time_s"] = fields["off_time_ms"] / 1000.0
    fields["lift_distance_mm"] = fields["lift_distance_um"] / 1000.0
    fields["lift_speed_mm_s"] = fields["lift_speed_um_s"] / 1000.0
    fields["retract_speed_mm_s"] = fields["retract_speed_um_s"] / 1000.0
    fields["volume_ml"] = fields["volume_ul"] / 1000.0

    return fields


# ============================================================================
# Validation helpers
# ============================================================================


def validate_bitmaps(
    bitmaps: List[List[List[int]]],
    width: int,
    height: int,
) -> None:
    """Validate that bitmap data matches the expected dimensions.

    Args:
        bitmaps: List of layer bitmaps.
        width: Expected horizontal resolution.
        height: Expected vertical resolution.

    Raises:
        ValueError: If dimensions do not match or data is malformed.
    """
    if not bitmaps:
        raise ValueError("Layer list is empty")

    for i, bm in enumerate(bitmaps):
        if not isinstance(bm, list):
            raise ValueError(f"Layer {i}: expected list, got {type(bm).__name__}")
        if len(bm) != height:
            raise ValueError(f"Layer {i}: expected {height} rows, got {len(bm)}")
        for row_idx, row in enumerate(bm):
            if not isinstance(row, list):
                raise ValueError(
                    f"Layer {i}, row {row_idx}: expected list, got {type(row).__name__}"
                )
            if len(row) != width:
                raise ValueError(
                    f"Layer {i}, row {row_idx}: expected {width} columns, "
                    f"got {len(row)}"
                )
            for col_idx, px in enumerate(row):
                if not isinstance(px, (int, float)):
                    raise ValueError(
                        f"Layer {i}, row {row_idx}, col {col_idx}: "
                        f"expected int, got {type(px).__name__}"
                    )
                if not (0 <= px <= 255):
                    raise ValueError(
                        f"Layer {i}, row {row_idx}, col {col_idx}: "
                        f"pixel value {px} out of range 0-255"
                    )


def validate_settings(settings: Dict[str, Any], strict: bool = False) -> List[str]:
    """Validate a settings dictionary and return any warnings.

    Args:
        settings: Settings dictionary to validate.
        strict: If *True*, raise :class:`ValueError` on the first error
            instead of collecting warnings.

    Returns:
        List of warning strings (empty if valid).
    """
    warnings: List[str] = []

    def _check(key: str, low: float, high: float, label: str) -> None:
        val = settings.get(key)
        if val is None:
            return
        if not isinstance(val, (int, float)):
            msg = f"{label}: expected number, got {type(val).__name__}"
            if strict:
                raise ValueError(msg)
            warnings.append(msg)
            return
        if val < low or val > high:
            msg = f"{label}: value {val} outside expected range [{low}, {high}]"
            if strict:
                raise ValueError(msg)
            warnings.append(msg)

    _check("resolution_x", 1, 16384, "resolution_x")
    _check("resolution_y", 1, 16384, "resolution_y")
    _check("layer_height", 0.001, 1.0, "layer_height")
    _check("exposure_time", 0.0, 120.0, "exposure_time")
    _check("bottom_exposure_time", 0.0, 300.0, "bottom_exposure_time")
    _check("bottom_layers", 0, 100, "bottom_layers")
    _check("light_off_delay", 0.0, 30.0, "light_off_delay")
    _check("lift_height", 0.0, 50.0, "lift_height")
    _check("lift_speed", 0.01, 20.0, "lift_speed")
    _check("retract_speed", 0.01, 20.0, "retract_speed")

    return warnings


# ============================================================================
# Bitmap utilities
# ============================================================================


def create_blank_bitmap(width: int, height: int, fill: int = 0) -> List[List[int]]:
    """Create a 2-D bitmap filled with *fill*.

    Args:
        width: Number of columns.
        height: Number of rows.
        fill: Pixel value (0-255).

    Returns:
        2-D list of integers.
    """
    return [[fill] * width for _ in range(height)]


def invert_bitmap(bitmap: List[List[int]]) -> List[List[int]]:
    """Return a new bitmap with each pixel inverted (255 - value)."""
    return [[255 - px for px in row] for row in bitmap]


def bitmap_pixel_count(bitmap: List[List[int]], threshold: int = 1) -> int:
    """Count the number of non-zero (lit) pixels in a bitmap."""
    count = 0
    for row in bitmap:
        for px in row:
            if px >= threshold:
                count += 1
    return count


def bitmaps_total_pixels(bitmaps: List[List[List[int]]], threshold: int = 1) -> int:
    """Sum the lit pixel count across all layers."""
    return sum(bitmap_pixel_count(bm, threshold) for bm in bitmaps)


def estimate_resin_ml(
    bitmaps: List[List[List[int]]],
    pixel_width_mm: float,
    pixel_height_mm: float,
    layer_height_mm: float,
    threshold: int = 1,
) -> float:
    """Estimate resin volume in millilitres from layer bitmaps.

    Args:
        bitmaps: Per-layer 2-D bitmaps.
        pixel_width_mm: Horizontal pixel pitch in mm.
        pixel_height_mm: Vertical pixel pitch in mm.
        layer_height_mm: Layer thickness in mm.
        threshold: Pixel value above which is considered "lit".

    Returns:
        Estimated volume in ml.
    """
    total_px = bitmaps_total_pixels(bitmaps, threshold)
    vol_mm3 = total_px * pixel_width_mm * pixel_height_mm * layer_height_mm
    return vol_mm3 / 1000.0


def estimate_print_time_seconds(
    num_layers: int,
    exposure_time: float,
    bottom_exposure: float,
    bottom_layers: int,
    light_off_delay: float,
    lift_height: float,
    lift_speed: float,
    retract_speed: float,
) -> float:
    """Estimate total print time in seconds.

    This is a rough estimate based on per-layer exposure and lift/retract
    cycle times.  It does not account for ramping or acceleration.
    """
    total = 0.0
    for i in range(num_layers):
        exp = bottom_exposure if i < bottom_layers else exposure_time
        total += exp + light_off_delay
        if lift_speed > 0:
            total += lift_height / lift_speed
        if retract_speed > 0:
            total += lift_height / retract_speed
    return total


# ============================================================================
# Module-level convenience
# ============================================================================


def write_resin_file(
    bitmaps: List[List[List[int]]],
    settings: Dict[str, Any],
    output_path: str,
    fmt: str = "ctb",
) -> None:
    """Write a resin slicer output file (convenience wrapper).

    Args:
        bitmaps: Per-layer 2-D bitmaps.
        settings: Printer and print settings.
        output_path: Destination file path.
        fmt: Output format (``'ctb'``, ``'goo'``, ``'sl1'``, ``'photon'``).
    """
    ResinOutput.write(bitmaps, settings, output_path, fmt=fmt)


def write_all_formats(
    bitmaps: List[List[List[int]]],
    settings: Dict[str, Any],
    output_dir: str,
    base_name: str = "output",
) -> Dict[str, str]:
    """Write the same slice data in every supported format.

    This is primarily useful for testing and comparison.  Returns a
    dictionary mapping format name to the written file path.

    Args:
        bitmaps: Per-layer 2-D bitmaps.
        settings: Printer and print settings.
        output_dir: Directory to write files into.
        base_name: Base file name (without extension).

    Returns:
        Dict of ``{format: file_path}``.
    """
    extensions = {"ctb": ".ctb", "goo": ".goo", "sl1": ".sl1", "photon": ".photon"}
    os.makedirs(output_dir, exist_ok=True)
    results: Dict[str, str] = {}
    for fmt, ext in extensions.items():
        path = os.path.join(output_dir, base_name + ext)
        ResinOutput.write(bitmaps, settings, path, fmt=fmt)
        results[fmt] = path
    return results
