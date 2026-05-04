#!/bin/bash

exit 0

# Just first time to install.
sudo -E ./waf prerequisites

# Once after download/clean.
./waf configure

# Build
./waf build upload --app=01

# Debug with USB2UART dongle
./waf log --port=0
# Exit on Ctrl+A Ctrl+X
