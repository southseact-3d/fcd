# SPDX-License-Identifier: LGPL-2.1-or-later

"""PartDesign namespace compatibility module for split features."""

from PartDesign.BOPTools.SplitFeatures import *  # noqa: F401,F403

__all__ = [name for name in globals() if not name.startswith("_")]
