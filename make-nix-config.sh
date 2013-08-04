#!/bin/sh

# Make a configuration snippet for NixOS users with udev rules for using
# USB devices with muFORTH.

cat <<EOT
services.udev.extraRules = ''
EOT
./make-udev-rules.sh $1
cat <<EOT
'';
EOT
