#!/usr/bin/env bash
set -euo pipefail

ccache_wrapper () {
  echo "#!/bin/bash" >> ~/ccache/ccache-"$1"
  echo "ccache $1 \"$*\"" >> ~/ccache/ccache-"$1"
  chmod +x ~/ccache/ccache-"$1"
}

if [[ "${TRAVIS_OS_NAME}" == osx ]]; then
  brew update
  brew install findutils
  brew cask reinstall java
  brew unlink python # fixes 'run_one_line' is not defined error in backtrace
  brew install ccache
  ccache_wrapper clang
  ccache_wrapper clang++
fi

if [[ "${TRAVIS_OS_NAME}" == linux ]]; then
  wget https://raw.githubusercontent.com/sormuras/bach/master/install-jdk.sh
  ccache_wrapper gcc
  ccache_wrapper g++
fi

export PATH="$HOME/ccache:$PATH"
