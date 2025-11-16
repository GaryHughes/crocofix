#!/bin/bash
set -e
source "config.sh"

source_path="$(realpath ..)"

docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --platform ${platform} -it --rm -v ${source_path}:/crocofix -w /crocofix -p 8089:8089 ${build_image_name}
