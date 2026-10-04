# Fork changes and maintenance

This personal fork follows GNOME's `main` branch. As of 2026-10-03 that branch
identifies itself as `52.alpha`, a development version.

## Custom behavior

| Behavior | Source | Scope |
| --- | --- | --- |
| Leading `·` (U+00B7) sorts first in ascending name order | `src/nautilus-file.c` | Normal collation within each group, folders-first, reverse ordering, and the existing `.`/`#` group remain intact. Underscores and similar-looking characters keep upstream behavior. |
| Encrypted-volume prompts default to forgetting the passphrase | `src/nautilus-file-operations.c` | Leave `GMountOperation` at its `G_PASSWORD_SAVE_NEVER` default. The user can still choose to remember a passphrase. |
| External open requests create new windows | `src/nautilus-application.c` | Command-line folder opens, application open requests, and FileManager1 folder/reveal requests open separate windows. Explicit internal tab navigation keeps its existing behavior. |
| Routine dialogs allow browsing while open | Dialog presentation in `src/` | New Folder, Compress, Properties, Preferences, batch rename, Open With, view settings, search filters, selection patterns, and informational messages use separate non-modal windows. New-folder targets are captured when the prompt opens; view-specific settings close when their view changes. Operation decisions and destructive confirmations retain their blocking behavior. |

Keep the diff against upstream small. Planned filename substitutions and broader
sorting rules are separate decisions and are not implemented here.

## Updating the source

Fetch both the published fork and upstream before working; local branches may
predate changes published from another checkout. Integrate from the published
fork tip and merge upstream without rewriting the published history.

```sh
git fetch --no-tags origin main
git fetch --no-tags upstream main
# From a clean branch based on origin/main:
git merge --no-ff upstream/main
git diff upstream/main --stat
git diff --check
```

Review upstream changes to the files above together with the custom
behavior. Resolve conflicts explicitly and preserve both sets of changes.

## Reusing the build

Use the existing configured build directory in its original source checkout:

```sh
env PATH="/usr/bin:/bin:$PATH" meson compile -C build -j 4
```

Ninja rebuilds changed inputs and Meson regenerates its build files when needed.
Routine upstream merges do not require deleting the build directory or running
`meson setup --wipe`. Some merges legitimately rebuild many objects; they still
reuse the configured dependencies and unaffected outputs.

Keep upstream's dependency requirements. At the date above, the minimums include
GLib 2.89.0, GTK 4.24.0, and libadwaita 1.8.alpha. Check `meson.build` and actual
installed versions before advancing again. A higher minimum alone is not a
reason to lower it, force a clean build, or upgrade unrelated system packages.

Use system Python for build tools. If Blueprint is supplied from a local package
extraction, retain its executable path and Python module path when reusing the
build. Changing `PATH` without its `PYTHONPATH` can break that tool.

Run the existing tests with `meson test -C build --no-rebuild`, in a private home
with separate XDG directories and a private D-Bus session. Display tests also
need a private display, such as Xvfb. Start those processes with an explicit
environment allowlist so test logs cannot capture inherited credentials. Check
the real external open routes, existing windows, and file selection in that
isolated session before installing.

If `localsearch test-sandbox` fails because its packaged `trackertestutils`
directory is missing, record that unavailable test separately. It does not
establish a search regression or a successful search test.

## Installing on Arch-based systems

Use a pacman-managed `nautilus-skrblk` package that provides `nautilus`, conflicts
with the distribution's `nautilus` package, and declares the actual runtime
dependency minimums. Stage from the incremental build with
`meson install --no-rebuild --destdir <staging-directory>` before packaging.

The distribution's `libnautilus-extension` package stays separately owned. Check
its ABI against the staged library before excluding its library, headers, GIR,
typelib, and pkg-config files from the application payload. Check every remaining
file for ownership conflicts; do not overwrite another package's files.

Preserve the currently installed package and verify the new package's dependency
transaction before installation. Record the source commit, package version,
binary checksum, package integrity, and any unverified behavior separately.
Back up the real bookmarks before running desktop checks, and verify their bytes
afterward. Restart the desktop app only when needed, preserving its open folders.
