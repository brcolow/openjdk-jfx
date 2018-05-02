#!/usr/bin/env bash
set -euo pipefail

ccache_wrapper () {
  mkdir -p ~/ccache
  echo '#!/bin/sh' >> ~/ccache/ccache-"$1"
  echo "ccache $1 \"\$*\"" >> ~/ccache/ccache-"$1"
  chmod +x ~/ccache/ccache-"$1"
}

if [[ "${TRAVIS_OS_NAME}" == osx ]]; then
  brew update
  brew install findutils ccache
  brew cask reinstall java
  brew unlink python # fixes 'run_one_line' is not defined error in backtrace
  ccache_wrapper clang
  ccache_wrapper clang++
  ln -s "$(brew --prefix clang)"/clang /usr/local/opt/ccache/libexec/ccache
  ln -s "$(brew --prefix clang)"/clang++ /usr/local/opt/ccache/libexec/ccache
fi

if [[ "${TRAVIS_OS_NAME}" == linux ]]; then
  wget https://raw.githubusercontent.com/sormuras/bach/master/install-jdk.sh
  ccache_wrapper gcc
  ccache_wrapper g++
  ln -s  "$(command -v gcc)" /usr/lib/ccache
  ln -s  "$(command -v g++)" /usr/lib/ccache
fi
