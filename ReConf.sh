#!/bin/bash

function ReConf()
{
  # Reset
  Pkgs=($(cd $1; ls | grep "^lib.") $(cd $1; ls | grep "^com."))
  File=$1/"auto.ninja"

  echo -e "# auto-generated file\n" > $File


  # Targets
  for Pkg in ${Pkgs[@]}; do
    echo "build $1/$Pkg: SUB" >> $File
    echo "  dir = $1/$Pkg" >> $File
    echo "" >> $File
  done

  echo "" >> $File

  echo "build $1/Build: phony ${Pkgs[@]/#/$1/}" >> $File
}


ReConf Code
ReConf Devel
