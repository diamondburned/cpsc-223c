#!/usr/bin/env bash
#
# pack.sh - pack the given files into a tarball and put a script into the
# clipboard. Simply paste the script into a terminal and the files will be
# extracted.

copy() {
	# If you're not on Wayland, you can use xclip instead.
	wl-copy
}

extraFiles=(
	.replit
	.clang-format
	.editorconfig
	shell.nix
)

main() {
	if (( $# < 1 )); then
		echo "Usage: $0 file1 [file2 ...]" >&2
		exit 1
	fi

	tars=(
		-czvf -
		--exclude-vcs
		--exclude-vcs-ignores
	)

	files=(
		"${extraFiles[@]}"
		"$@"
	)

	for f in "${files[@]}"; do
		if [[ -d "$f" ]]; then
			tars+=(-C "$(realpath "$f")")
			while read -r child; do
				if git check-ignore -q "$child"; then
					path=$(realpath --relative-to "$f" "$child")
					tars+=(--exclude "$path")
					continue
				fi
			done < <(find "$f" -type f)
			tars+=(".")
		elif [[ -f "$f" ]]; then
			if ! git check-ignore -q "$f"; then
				tars+=(-C "$PWD" "$f")
			fi
		else
			echo "File not found: $f" >&2
			exit 1
		fi
	done

	{
		echo "find . -delete; cat<<'EOF' | base64 -d | tar xzvf -"
		tar "${tars[@]}" | base64 -w0
		echo
		echo "EOF"
	} | copy
}

main "$@"
