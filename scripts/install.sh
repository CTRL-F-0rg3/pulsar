#!/usr/bin/env bash
set -e
PKG_DIR="$(cd "$(dirname "$0")" && pwd)"
if [ -f "$PKG_DIR/bin/pulsar" ]; then SRC="$PKG_DIR/bin/pulsar"
elif [ -f "$PKG_DIR/../build/pulsar" ]; then SRC="$PKG_DIR/../build/pulsar"
else echo "Error: pulsar binary not found"; exit 1; fi

mkdir -p "$HOME/.local/bin" "$HOME/.local/share/applications" "$HOME/.local/share/icons/hicolor/scalable/apps"
cp "$SRC" "$HOME/.local/bin/pulsar"
chmod +x "$HOME/.local/bin/pulsar"
for d in "$PKG_DIR/share/applications/pulsar.desktop" "$PKG_DIR/../assets/pulsar.desktop"; do
  [ -f "$d" ] && cp "$d" "$HOME/.local/share/applications/pulsar.desktop" && break
done
for d in "$PKG_DIR/share/icons/hicolor/scalable/apps/pulsar.svg" "$PKG_DIR/../assets/pulsar.svg"; do
  [ -f "$d" ] && cp "$d" "$HOME/.local/share/icons/hicolor/scalable/apps/pulsar.svg" && break
done
gtk-update-icon-cache -f -t "$HOME/.local/share/icons/hicolor" 2>/dev/null || true
update-desktop-database "$HOME/.local/share/applications" 2>/dev/null || true
echo "=== Pulsar installed. Run: pulsar ==="
