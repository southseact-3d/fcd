## Summary

- **Enable timeline by default** - The parametric timeline is now visible on startup at the bottom of the viewport
- **History marker with rollback** - Drag the slider or right-click a feature to "Roll History Marker Here" to see the model at that point in time. Features after the marker are dimmed and suppressed.
- **Suppress/Unsuppress** - Right-click any timeline feature to suppress it. Downstream dependencies are automatically suppressed/unsuppressed.
- **Drag-and-drop reorder** - Drag features to reorder them in the timeline. Dependency validation prevents invalid reorderings.
- **Play animation** - Play button steps through features at 500ms intervals, showing the model being built step-by-step
- **Groups** - TimelineGroup class for collapsible feature grouping (foundation for future UI)
- **Auto-refresh** - Timeline connects to document signals (new/delete/undo/redo) and updates automatically

## Bug Fixes

- Fixed Loft icon key typo (leading space in "Loft")
- Fixed stretch widget memory leak in updateOperationsList()
- Added dependency checking before delete operations

## Files Changed

| File | Changes |
|------|---------|
| src/Gui/Timeline.h | Complete rewrite with new classes, signals, and member variables |
| src/Gui/Timeline.cpp | Full implementation of all features (~1100 lines) |
| src/Gui/MainWindow.cpp | Enable timeline by default, add getTimeline() accessor |
| src/Gui/MainWindow.h | Add getTimeline() public method |
| src/Gui/CommandView.cpp | Add Std_ToggleTimeline command for View menu |

## Testing

- Timeline appears by default when opening Tungsten CAD
- Slider navigation works (start/prev/next/end)
- Right-click context menu provides Edit, Suppress, Rollback, Delete
- Play button animates through features
- Drag-and-drop reorder validates dependencies
- Undo/Redo operations refresh the timeline automatically
