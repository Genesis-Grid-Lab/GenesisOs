BR2_DEFCONFIG := $(shell realpath configs/genesisos_defconfig)
BUILDROOT_DIR := buildroot
OUT_DIR := $(shell realpath output)
BR2_EXTERNAL := $(shell realpath external)
BR2_EXTERNAL_GENESISOS_PATH := $(BR2_EXTERNAL)
OVERLAY_DIR := $(BR2_EXTERNAL_GENESISOS_PATH)/overlay

.PHONY: all config build qemu menuconfig

all: config build

config:
	@echo "Using overlay at: $(OVERLAY_DIR)"
	BR2_ROOTFS_OVERLAY="$(OVERLAY_DIR)" \
	make \
		BR2_DEFCONFIG=$(BR2_DEFCONFIG) \
		BR2_EXTERNAL=$(BR2_EXTERNAL) \
		BR2_EXTERNAL_GenesisOS_PATH=$(BR2_EXTERNAL_GENESISOS_PATH) \
		O=$(OUT_DIR) \
		-C $(BUILDROOT_DIR) defconfig

build:
	@echo "Using overlay at: $(OVERLAY_DIR)"
	BR2_ROOTFS_OVERLAY="$(OVERLAY_DIR)" \
	make O=$(OUT_DIR) -C $(BUILDROOT_DIR) -j4

Make-ext4:
	dd if=/dev/zero of=$(OUT_DIR)/images/rootfs.ext4 bs=1M count=512
	mkfs.ext4 $(OUT_DIR)/images/rootfs.ext4
	sudo mount -o loop $(OUT_DIR)/images/rootfs.ext4 /mnt
	sudo tar -xf $(OUT_DIR)/images/rootfs.tar -C /mnt   # optional: populate it
	sudo umount /mnt

menuconfig:
	make O=$(OUT_DIR) -C $(BUILDROOT_DIR) menuconfig

linux-menuconfig:
	make O=$(OUT_DIR) -C $(BUILDROOT_DIR) linux-menuconfig

genesis-ui:
	make O=$(OUT_DIR) -C $(BUILDROOT_DIR) genesis-ui -j4

genesis-ui-rebuild:
	make O=$(OUT_DIR) -C $(BUILDROOT_DIR) genesis-ui-rebuild -j4

qemu:
	qemu-system-x86_64 \
		-kernel $(OUT_DIR)/images/bzImage \
		-initrd $(OUT_DIR)/images/rootfs.cpio.gz \
		-drive file=output/images/rootfs.ext4,format=raw,if=virtio \
		-append "console=ttyS0" \
		-m 1024 -enable-kvm -cpu host \
		-device virtio-vga-gl \
		-device virtio-gpu-pci \
		-display sdl,gl=on \
		-vga virtio \
		-serial mon:stdio

qemu-soft:
	qemu-system-x86_64 \
		-kernel $(OUT_DIR)/images/bzImage \
		-drive file=$(OUT_DIR)/images/rootfs.ext2,format=raw,if=virtio \
		-drive file=output/images/rootfs.ext4,format=raw,if=virtio \
		-append "console=tty0" \
		-m 1024 \
		-device virtio-gpu-pci \
		-display sdl,gl=off \
		-serial mon:stdio


# Clean everything (Buildroot output)
clean:
	rm -rf $(OUT_DIR)

# Clean only genesis-ui build files
clean-ui:
	rm -rf $(OUT_DIR)/build/genesis-ui-1.0 \
	       $(OUT_DIR)/staging/usr/bin/genesis-ui \
	       $(OUT_DIR)/target/usr/bin/genesis-ui

clean-final:
	make O=$(OUT_DIR) -C $(BUILDROOT_DIR) target-finalize-clean

