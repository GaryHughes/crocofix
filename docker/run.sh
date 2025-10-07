#!/bin/bash
set -e
source "config.sh"

source_path="$(realpath ..)"

docker run -it --rm -v ${source_path}:/crocofix ${build_image_name}
