# SPDX-License-Identifier: LGPL-2.1-or-later

"""PartDesign namespace compatibility module for join features."""

from PartDesign.BOPTools.JoinFeatures import *  # noqa: F401,F403

__all__ = [name for name in globals() if not name.startswith("_")]
