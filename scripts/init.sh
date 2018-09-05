#!/usr/bin/env bash
# Copyright 2018 Slightech Co., Ltd. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# _VERBOSE_=1
# _INIT_LINTER_=1
# _FORCE_INSRALL_=1

BASE_DIR=$(cd "$(dirname "$0")" && pwd)

source "$BASE_DIR/init_tools.sh"

## deps

_echo_s "Init deps"

if [ "$HOST_OS" = "Linux" ]; then
  _install_deps "$SUDO apt-get install" libv4l-dev
elif [ "$HOST_OS" = "Mac" ]; then
  _install_deps "brew install" libuvc
elif [ "$HOST_OS" = "Win" ]; then
  # _install_deps "pacman -S" ?
  _echo
else  # unexpected
  _echo_e "Unknown host os :("
  exit 1
fi

## cmake version

_echo_s "Expect cmake version >= 3.0"
cmake --version | head -1
if [ "$HOST_NAME" = "Ubuntu" ]; then
  # sudo apt remove cmake
  _echo "How to upgrade cmake in Ubuntu"
  _echo "  https://askubuntu.com/questions/829310/how-to-upgrade-cmake-in-ubuntu"
fi

exit 0
