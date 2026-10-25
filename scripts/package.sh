#!/usr/bin/env bash
set -e
VERSION="${1:-v0.1.0}"
REPO="$(cd "$(dirname "$0")/.." && pwd)"
NAME="pulsar-linux-${VERSION}"
DIR="${REPO}/dist/${NAME}"

if [ ! -f "${REPO}/build/pulsar" ]; then
  echo "Error: build/pulsar not found. Build first."
  exit 1
fi

rm -rf "${DIR}"
mkdir -p "${DIR}/bin" "${DIR}/share/applications" "${DIR}/share/icons/hicolor/scalable/apps" "${DIR}/src"
cp "${REPO}/build/pulsar" "${DIR}/bin/"
chmod +x "${DIR}/bin/pulsar"
cp "${REPO}/assets/pulsar.desktop" "${DIR}/share/applications/"
cp "${REPO}/assets/pulsar.svg" "${DIR}/share/icons/hicolor/scalable/apps/"
cp -r "${REPO}/src/"* "${DIR}/src/"
cp "${REPO}/CMakeLists.txt" "${DIR}/src/"
cp "${REPO}/scripts/install.sh" "${DIR}/"
chmod +x "${DIR}/install.sh"
cd "${REPO}/dist" && tar -czf "${NAME}.tar.gz" "${NAME}"
echo "=== Packaged: dist/${NAME}.tar.gz ==="
ls -lh "${REPO}/dist/${NAME}.tar.gz"
