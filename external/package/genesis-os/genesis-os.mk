GENESIS_OS_VERSION =  fcc3640d858c9633d8cddf49558bed9500300fd8
GENESIS_OS_SITE = https://github.com/Genesis-Grid-Lab/Genesis-OS.git
GENESIS_OS_SITE_METHOD = git
GENESIS_OS_GIT_SUBMODULES = YES
GENESIS_OS_DEPENDENCIES = mesa3d libdrm libgbm libgles libegl

GENESIS_OS_CONF_OPTS = -DCMAKE_BUILD_TYPE=Debug \
                       -DCMAKE_INSTALL_PREFIX=/usr

define GENESIS_OS_PRE_INSTALL
	mkdir -p $(HOST_DIR)/$(GNU_TARGET_NAME)/sysroot/usr
endef

GENESIS_OS_POST_PATCH_HOOKS += GENESIS_OS_PRE_INSTALL

define GENESIS_OS_CONFIGURE_CMDS
	cd $(@D) && cmake $(GENESIS_OS_CONF_OPTS) .
endef

define GENESIS_OS_BUILD_CMDS
	$(MAKE) -C $(@D)
endef

define GENESIS_OS_INSTALL_TARGET_CMDS
	DESTDIR=$(TARGET_DIR) $(MAKE) -C $(@D) install
endef

$(eval $(cmake-package))

