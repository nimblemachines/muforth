#!/bin/sh

# This file is part of muforth: https://muforth.dev/
#
# Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)

# Make a configuration snippet for NixOS users with udev rules for using
# USB devices with muforth.

cat <<EOT
  services.udev.extraRules = ''
EOT
../scripts/make-udev-rules.sh $1
cat <<EOT
  '';
EOT
