# Part → PartDesign Migration Status (March 15 2026)

## ✅ Completed Work (what is already done)

### 1) Workbench / UI Migration
- **Part workbench registration removed**: `src/Mod/Part/InitGui.py` no longer registers `PartWorkbench` and prints a notice to use PartDesign.
- **PartDesign menu/toolbar now exposes Part features**: `src/Mod/PartDesign/Gui/Workbench.cpp` includes all major Part operations (primitives, boolean, join/split/compound, copy, and misc operations) under PartDesign UI.
- **PartDesign command bridge**: `src/Mod/PartDesign/PartCommandBridge.py` defines `PartDesign_*` command aliases for legacy `Part_*` commands and includes explicit handlers for key operations.
- **Macro migration tooling**: `src/Mod/PartDesign/PartWorkbenchMigration.py` and `src/Mod/PartDesign/PartWorkbenchMigrationGui.py` provide utilities and a GUI command for rewriting macros.

### 2) Backend Namespace Migration
- **Migrated Part backend modules into PartDesign namespace**:
  - `src/Mod/PartDesign/BOPTools/*` (Boolean/Join/Split/Tolerance) imported from Part.
  - `src/Mod/PartDesign/CompoundTools/*` (CompoundFilter / Explode) imported from Part.
- **Command registration bound to PartDesign commands**:
  - `PartDesign_*` commands are now registered directly from migrated backend modules (Join/Split/Tolerance/CompoundTools), decreasing dependency on the bridge.
- **Compatibility wrapper modules**:
  - `src/Mod/PartDesign/JoinFeatures.py` and `src/Mod/PartDesign/SplitFeatures.py` re-export full PartDesign backend for compatibility.

### 3) Test & Build Infrastructure Updates
- **New tests added**:
  - `src/Mod/PartDesign/PartDesignTests/TestPartMigration.py` covers command remapping + macro rewriting.
  - `src/Mod/PartDesign/TestPartDesignGui.py` added a `PartDesignBridgeCommandTests` class to verify GUI commands and registration.
- **CMake packaging updated**:
  - `src/Mod/PartDesign/CMakeLists.txt` now includes BOPTools/CompoundTools modules and test file.

### 4) Encoding/Tooling Hygiene
- Ensured newly added backend files have no UTF‑8 BOM to avoid parser/toolchain issues.

---

## ⏳ Remaining Work (what is left to complete)

### A) Full C++ Backend Migration (complete for UI commands)
- **Goal:** Transfer C++ implementations of Part workbench commands into the PartDesign module space, while maintaining compatibility.
- **Progress:** All `PartDesign_Part*` commands exposed in the PartDesign workbench UI now have native wrappers in `src/Mod/PartDesign/Gui/Command.cpp` (wrapping the corresponding `Part_*` command behavior via `Gui.runCommand`).
- Remaining: none for UI command coverage — the workbench now fully owns the `PartDesign_Part*` command set.

### B) Remaining implementation/integration tasks
#### 1) Runtime validation / build verification
- **Action:** Perform a full build and run the application to ensure all new commands work as expected, and no missing symbols are introduced by the new C++ wrappers.
- **Status:** Not yet executed in this environment (build toolchain not available in the current terminal context).

#### 2) Automated tests for migrated commands
- **Action:** Extend `src/Mod/PartDesign/TestPartDesignGui.py` to invoke a subset of `PartDesign_Part*` commands end-to-end (e.g., create a box, perform boolean operations, create offsets, run cross-sections).
- **Purpose:** Ensure the new wrapper commands function correctly and that the PartDesign workbench can operate without relying on the legacy Part workbench.

#### 3) Cleanup & documentation
- **Action:** Ensure the Part workbench is clearly deprecated (console messages, docs, tooltips) and that the migration path is documented for users.
- **Action:** Verify there are no remaining `Part_*` menu/toolbar entries exposed in PartDesign (beyond the wrapper command set). If any remain, replace them with their `PartDesign_*` equivalents or remove them.

---

## 📌 Suggested Next Step
Run a full build + smoke test run (ideally via the project’s CI/build script) and then add/modify unit tests to execute key `PartDesign_Part*` flows.

### B) End-to-End Runtime Validation
- Validate that the full GUI + document workflows work correctly (e.g., creating PartDesign bodies + using PartDesign‑migrated boolean/join/split tools, running legacy macros).
- This requires a full build and interactive test, which is not in the current automated test coverage.

### C) Cleanup / Deprecation Messaging
- Add explicit deprecation warnings to Part workbench and Part module docs to guide users toward PartDesign.
- Ensure any remaining `Part_` command registrations are appropriately flagged or hidden.

---

## 🧠 Step-by-Step File-by-File To-Do List (next actions)

### 1) Migrate remaining C++ commands into PartDesign (primary effort)

1. **Identify remaining Part C++ command files** (likely in `src/Mod/Part/Gui/`):
   - `CommandCreateBox.*`, `CommandCreateCylinder.*`, ..., `CommandCreateTorus.*`
   - `CommandBoolean.*`, `CommandSection.*`, `CommandOffset.*`, `CommandScale.*`, `CommandShapeFromMesh.*`, `CommandCheckGeometry.*`, etc.

2. **Example template:** `PartDesign_PartBox` has already been added to `src/Mod/PartDesign/Gui/Command.cpp`.
   - For each remaining command, create a PartDesign C++ command class following the same pattern (wrap the corresponding Part command logic via `doCommand` or direct feature creation).
   - Update internal command strings / icons to use `PartDesign_*`.
   - Register the new PartDesign command in `CreatePartDesignCommands()`.

3. **Update workbench registration**:
   - Ensure `src/Mod/PartDesign/Gui/Workbench.cpp` uses the PartDesign command names consistently (not legacy Part names).
   - Verify that PartDesign workbench registers all new commands and that no `Part_*` command registration is required for operation.

4. **Drop legacy Part command registrations**:
   - Once the PartDesign workbench owns the command, remove or disable the equivalent `Part_*` registration in `src/Mod/Part/InitGui.py` and related init files (except where needed for backward compatibility).

---

### 2) Add coverage/runtime tests for new command paths

1. Expand `src/Mod/PartDesign/TestPartDesignGui.py` (or create new tests) that:
   - Run key `PartDesign_*` commands end-to-end (create a body, create a box, perform boolean, etc.)
   - Validate the resulting objects exist and have expected type/geometry.

2. Add a test that imports and runs `PartDesign.BOPTools` and `PartDesign.CompoundTools` features directly (not through bridge) to verify direct registration works.

---

### 3) Final cleanup and deprecation messaging

1. Add or augment a deprecation note in `src/Mod/Part/InitGui.py` and other Part init scripts (if any) to make the migration path explicit.
2. Remove any unused legacy command name lists or duplicate registrations (verify via grep for `Part_` strings in `src/Mod/PartDesign` and `src/Mod/Part`).

---

## 📌 Where to start next (recommended order)

1. **Proof of concept:** Port a single Part C++ command (e.g., `CommandCreateBox`) into PartDesign and verify it works via GUI tests.
2. Expand to all remaining Part workbench commands.
3. Add or update tests to ensure PartDesign workbench can fully replace Part workbench commands.

---