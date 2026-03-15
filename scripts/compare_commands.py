import re
import pathlib

root = pathlib.Path(r'c:/Users/liamh/Downloads/ai - 2/tungsten cad')

# load part commands
part_cmds = set()
for p in root.rglob('*'):
    if p.is_file() and p.suffix in ('.cpp', '.h', '.py') and 'Mod\\Part' in str(p):
        try:
            text = p.read_text(encoding='utf-8', errors='ignore')
        except Exception:
            continue
        for m in re.finditer(r'Command\("(Part_[A-Za-z0-9_]+)"\)', text):
            part_cmds.add(m.group(1))
        for m in re.finditer(r'addCommand\("(Part_[A-Za-z0-9_]+)"\)', text):
            part_cmds.add(m.group(1))

# load bridge mappings
bridge_path = root / 'src' / 'Mod' / 'PartDesign' / 'PartCommandBridge.py'
text = bridge_path.read_text(encoding='utf-8', errors='ignore')
# match lines like "PartDesign_PartBox": ("Part_Box", ...
bridge_src = set(re.findall(r'"PartDesign_[A-Za-z0-9_]+": \(\s*"(Part_[A-Za-z0-9_]+)"', text))
bridge_aliases = set(re.findall(r'"(PartDesign_[A-Za-z0-9_]+)": \(', text))

print('Part commands found:', len(part_cmds))
print('Bridge source commands:', len(bridge_src))
print('Bridge aliases:', len(bridge_aliases))
print('---\nCommands in Part but not bridged:')
missing = sorted(part_cmds - bridge_src)
for c in missing:
    print(c)
print('---\nCommands bridged but not present in Part code (maybe external):')
extra = sorted(bridge_src - part_cmds)
for c in extra:
    print(c)
