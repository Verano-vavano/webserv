#!/bin/sh
# little script to copy files where the flatpak has access
# ./install.sh
# dans un navigateur se rendre sur
# ~/Downloads/web/index.html
# si vous utilisez vim tel les bonnes personnes vous pouvez faire ceci
# :autocmd! BufWritePost * silent! !./install.sh
# qui va relancer le script a chaque fois que vous ecrivez le fichier!
dest="$HOME/Downloads/web/"

[ -e "$dest" ] &&
	rm -rf "$dest"

mkdir "$dest"

cp -r * "$dest"
