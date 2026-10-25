# Pulsar

Markdown editor for TrangorgeOS. Inspired by Obsidian.

## Features
- Live split-view preview (Source / Split / Preview modes)
- Syntax highlighting: headers, bold, italic, code, links, lists, quotes, tags
- File tree sidebar with folder navigation
- Multi-tab editing
- GitHub-flavored markdown rendering
- Word and character count in status bar
- Toolbar shortcuts: B, I, H, list, link, code, image, quote
- Ctrl+S / Ctrl+B / Ctrl+I / Ctrl+1/2/3 shortcuts
- Cosmic dark UI with magenta/green accents

## Build

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . -j$(nproc)
    ./pulsar

## Install (from release)

    tar -xzf pulsar-linux-v0.1.0.tar.gz
    cd pulsar-linux-v0.1.0
    ./install.sh
    pulsar

## Requirements
- Arch: sudo pacman -S qt6-base
- Debian/Ubuntu: sudo apt install qt6-base-dev

## License
GPL-3.0
