#!/bin/sh
# little script to symlink files where the flatpak has access
dest="$HOME/Downloads/web/"

[ -e "$dest" ] &&
	rm -rf "$dest"

mkdir "$dest"

cp -r * "$dest"
