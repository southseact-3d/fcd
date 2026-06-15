# SPDX-License-Identifier: LGPL-2.1-or-later

import math
import FreeCAD, Part
from FreeCAD import Base


def _makeOblongWire(link_length, link_width, wire_diameter):
    """Create an oblong (racetrack) wire path for a single chain link.

    The wire is centered at the origin on the XY plane. It consists of:
    - Two straight segments along the X axis (top and bottom)
    - Two semicircular arcs at each end

    Args:
        link_length: Overall length of the link (long axis, along X)
        link_width: Overall width of the link (short axis, along Y)
        wire_diameter: Diameter of the circular cross-section forming the link

    Returns:
        A closed Part.Wire representing the centerline path of one link.
    """
    L = max(link_length - link_width, 0.01)
    W = link_width - wire_diameter

    if L < 0:
        L = 0.01
    if W < 0:
        W = 0.01

    half_len = L / 2.0
    R = W / 2.0

    if R < 0.001:
        R = 0.001

    # Bottom-left, bottom-right, top-right, top-left corners
    bl = Base.Vector(-half_len, -R, 0)
    br = Base.Vector(half_len, -R, 0)
    tr = Base.Vector(half_len, R, 0)
    tl = Base.Vector(-half_len, R, 0)

    # Midpoints on the arcs (leftmost and rightmost points)
    left_mid = Base.Vector(-half_len - R, 0, 0)
    right_mid = Base.Vector(half_len + R, 0, 0)

    # Build edges: bottom line, right arc, top line, left arc
    bottom = Part.LineSegment(bl, br)
    right_arc = Part.Arc(br, right_mid, tr)
    top = Part.LineSegment(tr, tl)
    left_arc = Part.Arc(tl, left_mid, bl)

    wire = Part.Wire([bottom.toShape(), right_arc.toShape(), top.toShape(), left_arc.toShape()])
    return wire


def createSingleLink(link_length, link_width, wire_diameter):
    """Create a single solid chain link as an oblong ring.

    The link is created by sweeping a circular cross-section along an
    oblong centerline path using Part.makePipeShell().

    Args:
        link_length: Overall length of the link (long axis)
        link_width: Overall width of the link (short axis)
        wire_diameter: Diameter of the circular cross-section

    Returns:
        A Part.Shape (solid) of a single chain link.
    """
    path_wire = _makeOblongWire(link_length, link_width, wire_diameter)

    # Create circular cross-section profile
    profile_radius = wire_diameter / 2.0
    profile_circle = Part.makeCircle(profile_radius)
    profile_wire = Part.Wire([profile_circle])

    # Sweep the profile along the path
    section = Part.Wire([profile_wire])
    link = Part.Wire(path_wire).makePipeShell([section], True, True)
    return link


def _samplePath(path_shape, num_samples):
    """Sample positions and tangent vectors along a path shape.

    Args:
        path_shape: A Part.Shape (wire or edge) to sample
        num_samples: Number of sample points

    Returns:
        List of (position, tangent) tuples, each being Base.Vector pairs.
    """
    if num_samples < 2:
        num_samples = 2

    length = path_shape.Length
    if length < 1e-6:
        return [(Base.Vector(0, 0, 0), Base.Vector(1, 0, 0))] * num_samples

    samples = []
    for i in range(num_samples):
        t = i / (num_samples - 1.0)
        param = t * length
        try:
            vertex = path_shape.distToShape(path_shape)
        except Exception:
            pass

        # Use edge parameterization for more accurate sampling
        edges = path_shape.Edges
        if not edges:
            return [(Base.Vector(0, 0, 0), Base.Vector(1, 0, 0))] * num_samples

        # Accumulate edge lengths to find which edge contains param
        accumulated = 0.0
        target_edge = edges[0]
        target_param = 0.0
        for edge in edges:
            elen = edge.Length
            if accumulated + elen >= param - 1e-6 or edge is edges[-1]:
                target_edge = edge
                local_t = (param - accumulated) / elen if elen > 1e-6 else 0.0
                local_t = max(0.0, min(1.0, local_t))
                u_min = edge.FirstParameter
                u_max = edge.LastParameter
                target_param = u_min + local_t * (u_max - u_min)
                break
            accumulated += elen

        # Get position and tangent
        try:
            pos = target_edge.valueAt(target_param)
            tangent = target_edge.tangentAt(target_param)
            if tangent.Length > 1e-6:
                tangent = tangent.normalize()
            else:
                tangent = Base.Vector(1, 0, 0)
        except Exception:
            pos = Base.Vector(0, 0, 0)
            tangent = Base.Vector(1, 0, 0)

        samples.append((pos, tangent))

    return samples


def _buildLinkTransform(position, tangent, up_hint=Base.Vector(0, 0, 1)):
    """Build a rotation matrix to orient a link along a path tangent.

    The link is oriented so that:
    - Its long axis (X) aligns with the tangent direction
    - Its face normal (Z) is roughly aligned with up_hint

    Args:
        position: Center position (Base.Vector)
        tangent: Direction vector (Base.Vector, should be normalized)
        up_hint: Hint for the up direction

    Returns:
        A Base.Matrix representing the rotation (no translation).
    """
    # Build orthonormal basis: X = tangent, Z = up_hint cross tangent, Y = Z cross X
    x_axis = tangent.normalize()

    # Cross product of up_hint and tangent to get Z axis
    z_axis = up_hint.cross(x_axis)
    if z_axis.Length < 1e-6:
        # tangent is parallel to up_hint, use a different hint
        alt_hint = Base.Vector(0, 1, 0) if abs(x_axis.z) > 0.9 else Base.Vector(0, 0, 1)
        z_axis = alt_hint.cross(x_axis)
    z_axis = z_axis.normalize()

    y_axis = z_axis.cross(x_axis)
    y_axis = y_axis.normalize()

    # Build rotation+translation matrix (columns are the new axes + position)
    mat = Base.Matrix(
        x_axis.x, y_axis.x, z_axis.x, position.x,
        x_axis.y, y_axis.y, z_axis.y, position.y,
        x_axis.z, y_axis.z, z_axis.z, position.z,
        0, 0, 0, 1,
    )
    return mat


def createChain(link_length, link_width, wire_diameter, num_links, gap,
                path_shape=None, alternate_rotation=True):
    """Create a chain of interlocking links along a path.

    Args:
        link_length: Length of each link (long axis)
        link_width: Width of each link (short axis)
        wire_diameter: Cross-section diameter of the wire forming each link
        num_links: Number of links to create
        gap: Clearance between adjacent links
        path_shape: Optional path (Part.Shape) for the chain to follow.
                     If None, creates a straight chain along the X axis.
        alternate_rotation: If True, every other link is rotated 90 degrees
                            around the local tangent axis for physical interlocking.

    Returns:
        A list of Part.Shape objects, one per link, positioned along the path.
    """
    if num_links < 1:
        num_links = 1

    # Create the base link shape (at origin, aligned with X axis)
    base_link = createSingleLink(link_length, link_width, wire_diameter)

    # Compute spacing between link centers
    effective_pitch = link_length + gap

    # Determine path for placement
    if path_shape is not None:
        samples = _samplePath(path_shape, max(num_links, 2))
    else:
        # Straight chain along X axis
        samples = []
        for i in range(max(num_links, 2)):
            x = i * effective_pitch
            samples.append((Base.Vector(x, 0, 0), Base.Vector(1, 0, 0)))

    links = []
    for i in range(num_links):
        # Get position and tangent for this link
        if i < len(samples):
            pos, tangent = samples[i]
        else:
            # Extrapolate from the last sample
            last_pos, last_tangent = samples[-1]
            extra = (i - len(samples) + 1) * effective_pitch
            pos = Base.Vector(
                last_pos.x + last_tangent.x * extra,
                last_pos.y + last_tangent.y * extra,
                last_pos.z + last_tangent.z * extra,
            )
            tangent = last_tangent

        # Build rotation matrix to orient link along path
        mat = _buildLinkTransform(pos, tangent)

        # If alternating rotation, rotate 90 degrees around tangent axis
        if alternate_rotation and i % 2 == 1:
            rot90 = _rotationAroundAxis(tangent, math.pi / 2.0)
            mat = rot90.multiply(mat)

        # Transform the base link
        transformed = base_link.copy()
        transformed.transformShape(mat)

        links.append(transformed)

    return links


def _rotationAroundAxis(axis, angle):
    """Create a rotation matrix around an arbitrary axis by a given angle.

    Uses Rodrigues' rotation formula to build the matrix.

    Args:
        axis: The axis of rotation (Base.Vector, will be normalized)
        angle: Rotation angle in radians

    Returns:
        A Base.Matrix representing the rotation.
    """
    ax = axis.normalize()
    cos_a = math.cos(angle)
    sin_a = math.sin(angle)
    one_minus_cos = 1.0 - cos_a

    # Rodrigues' rotation formula as a matrix
    # R = cos(a)*I + sin(a)*[k]x + (1-cos(a))*(k tensor k)
    m00 = cos_a + ax.x * ax.x * one_minus_cos
    m01 = ax.x * ax.y * one_minus_cos - ax.z * sin_a
    m02 = ax.x * ax.z * one_minus_cos + ax.y * sin_a

    m10 = ax.y * ax.x * one_minus_cos + ax.z * sin_a
    m11 = cos_a + ax.y * ax.y * one_minus_cos
    m12 = ax.y * ax.z * one_minus_cos - ax.x * sin_a

    m20 = ax.z * ax.x * one_minus_cos - ax.y * sin_a
    m21 = ax.z * ax.y * one_minus_cos + ax.x * sin_a
    m22 = cos_a + ax.z * ax.z * one_minus_cos

    return Base.Matrix(
        m00, m01, m02, 0,
        m10, m11, m12, 0,
        m20, m21, m22, 0,
        0, 0, 0, 1,
    )


def computeChainLength(link_length, num_links, gap):
    """Compute the total chain length.

    Args:
        link_length: Length of each link
        num_links: Number of links
        gap: Gap between links

    Returns:
        Total chain length.
    """
    if num_links < 1:
        return 0.0
    return link_length * num_links + gap * (num_links - 1)
