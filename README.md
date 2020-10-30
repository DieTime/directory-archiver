# Directory Archiver

Allows packing directories into a single file without compression and unpacking them like tar archiver.

Arguments:
1. Action: `--pack` or `--unpack`
2. Source path: `<source folder path>` or `<archive path>`
3. Output path (optional): `[output archive path]` or `[output folder path]`

### Quick start
```bash
make
archiver --pack  <source folder path> [output archive path]
archiver --unpack <archive path> [output folder path]
```

### No memory leaks
```bash
make memcheck
make clean
```
