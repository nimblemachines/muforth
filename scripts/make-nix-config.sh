#!/bin/sh

# This file is part of muFORTH: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2014 David Frech. All rights reserved, and all wrongs
# reversed. (See the file COPYRIGHT for details.)

# Make a configuration snippet for NixOS users with udev rules for using
# USB devices with muFORTH.

cat <<EOT
services.udev.extraRules = ''
EOT
../scripts/make-udev-rules.sh $1
cat <<EOT
'';
EOT
