#!/bin/bash
set -e
source "config.sh"

source_path="$(realpath ..)"

docker run -it --rm -v ${source_path}:/crocofix -w /crocofix -p 8089:8089 ${build_image_name}
