#!/bin/sh
set -e

KERNEL_VERSION="6.9.0"

echo "[GenesisOS] Running depmod for $KERNEL_VERSION"
echo "[GenesisOS] Target dir $TARGET_DIR"
sudo depmod -v -b $TARGET_DIR $KERNEL_VERSION -v

