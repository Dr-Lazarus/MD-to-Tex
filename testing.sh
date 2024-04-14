#!/bin/bash
echo "Compiling md_to_tex..."
make

if [ $? -ne 0 ]; then
	echo "Compilation failed."
	exit 1
fi

input_dir="test_folder/input"
output_dir="test_folder/output"

if [ ! -d "$output_dir" ]; then
	echo "Output directory does not exist, creating it..."
	mkdir -p "$output_dir"
	if [ $? -ne 0 ]; then
		echo "Failed to create output directory."
		exit 1
	fi
else
	echo "Output directory already exists."
fi

convert_to_tex() {
	input_file="$1"
	output_file="${output_dir}/$(basename "${input_file%.*}").tex"
	echo "Converting $input_file to $output_file"
	./md_to_tex -i "$input_file" -o "test_folder/output$output_file"
}

export -f convert_to_tex

find "$input_dir" -type f -exec bash -c 'convert_to_tex "$0"' {} \;

echo "Conversion completed."
