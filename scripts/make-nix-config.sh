#!/bin/sh

# This file is part of muforth: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2017 David Frech. (Read the LICENSE for details.)

# Make a configuration snippet for NixOS users with udev rules for using
# USB devices with muforth.

cat <<EOT
  services.udev.extraRules = ''
EOT
../scripts/make-udev-rules.sh $1
cat <<EOT
  '';
EOT
