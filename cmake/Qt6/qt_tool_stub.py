#!/usr/bin/env python3
import sys, os, pathlib

# Determine output file from arguments (-o <file>)
args = sys.argv[1:]
out = None
it = iter(range(len(args)))
for i in it:
    arg = args[i]
    if arg in ('-o', '--output'):
        if i + 1 < len(args):
            out = args[i + 1]
            next(it, None)
    elif arg.startswith('-o') and arg != '-o':
        out = arg[2:]
    elif arg.startswith('--output='):
        out = arg.split('=', 1)[1]
    elif arg == '--output-dep-file':
        # Optionally skip dep file path if provided
        if i + 1 < len(args) and not args[i + 1].startswith('-'):
            next(it, None)

# Log invocation for debugging
log_path = os.environ.get("QT_TOOL_STUB_LOG")
if log_path:
    with open(log_path, "a") as log:
        log.write("%s\n" % " ".join(sys.argv))

# Bail out if no output file is provided
if out is None:
    sys.exit(0)

if out:
    path = pathlib.Path(out)
    path.parent.mkdir(parents=True, exist_ok=True)
    name = os.path.basename(sys.argv[0]).lower()
    if 'moc' in name:
        content = '// moc stub\n'
    elif 'uic' in name:
        content = '#pragma once\n// uic stub\n'
    else:
        content = '// qt tool stub\n'
    with open(path, 'w') as f:
        f.write(content)
