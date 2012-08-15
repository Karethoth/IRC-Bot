#!/bin/bash

action="$1"
extensionName="$2"

case "$action" in
  "build" )
  echo Building $extensionName
  g++ -g -shared -Wl,-soname,$extensionName.so.1 -o lib/$extensionName.so.1.0 src/extensions/$extensionName/*.cpp
  echo Setting up links for $extensionName
  ln -f lib/$extensionName.so.1.0 extensions/$extensionName.so.1
#ln -f lib/$extensionName.so.1.0 extensions/$extensionName.so
  echo Finished
  ;;

  * )
  echo "Usage:   ./extension.sh <command> <directory of extension>"
  echo "Example: ./extension.sh build bot"
  echo "- - - - - - - - - -"
  echo "Usable functions:"
  echo "  build - Builds and links an extension. If bot is"
  echo "          running, extension get's loaded automatically."
esac

