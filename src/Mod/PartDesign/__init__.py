# SPDX-License-Identifier: LGPL-2.1-or-later

import _PartDesign

makeFilletArc = _PartDesign.makeFilletArc

from .PartWorkbenchMigration import migrate_macro_file
from .PartWorkbenchMigration import remap_command_name
from .PartWorkbenchMigration import rewrite_macro_content
