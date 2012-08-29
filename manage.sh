#!/bin/bash

action="$1"
extensionName="$2"

case "$action" in
  "build" )
  echo Building $extensionName
  g++ -fPIC -g -shared -Wl,-soname,$extensionName.so.1 -o lib/$extensionName.so.1.0 src/extensions/$extensionName/*.cpp -lsqlite3
  if [ $? == 0 ]
    then
      echo Setting up links for $extensionName
      ln -f lib/$extensionName.so.1.0 extensions/$extensionName.so.1
      echo Finished
    else
      echo Failed to compile the $extensionName!
  fi
  ;;

  * )
  echo "Usage:   ./extension.sh <command> <directory of extension>"
  echo "Example: ./extension.sh build bot"
  echo "- - - - - - - - - -"
  echo "Usable functions:"
  echo "  build - Builds and links an extension. If bot is"
  echo "          running, extension get's loaded automatically."
esac

