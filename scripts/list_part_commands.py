import re
import pathlib

root = pathlib.Path(r'c:/Users/liamh/Downloads/ai - 2/tungsten cad')
cmds = set()

for p in root.rglob('*'):
    if p.is_file() and p.suffix in ('.cpp', '.h', '.py') and 'Mod\\Part' in str(p):
        try:
            text = p.read_text(encoding='utf-8', errors='ignore')
        except Exception:
            continue
        for m in re.finditer(r'Command\("(Part_[A-Za-z0-9_]+)"\)', text):
            cmds.add(m.group(1))
        for m in re.finditer(r'addCommand\("(Part_[A-Za-z0-9_]+)"\)', text):
            cmds.add(m.group(1))
        for m in re.finditer(r'(?:Gui|FreeCADGui)\.addCommand\("(Part_[A-Za-z0-9_]+)"\)', text):
            cmds.add(m.group(1))
        for m in re.finditer(r'runCommand\("(Part_[A-Za-z0-9_]+)"\)', text):
            cmds.add(m.group(1))

print('PART COMMANDS:', len(cmds))
for c in sorted(cmds):
    print(c)
