#!/usr/bin/env bash
make
while read -r input expect; do
	output=$(./main.out "$input")
	if [[ "$output" != "$expect" ]]; then
		echo "FAIL: $input"
		echo "  expect: $expect"
		echo "  output: $output"
		exit 1
	fi
done < testcases.txt
