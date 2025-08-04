GENESIS_OS_VERSION = 5c6b78e7327bdd5dd852cfa498cdc6c49aa5ddb8
GENESIS_OS_SITE = https://github.com/Genesis-Grid-Lab/Genesis-OS.git
GENESIS_OS_SITE_METHOD = git
GENESIS_OS_GIT_SUBMODULES = YES
GENESIS_OS_DEPENDENCIES = mesa3d libdrm libgbm libgles libegl

GENESIS_OS_CONF_OPTS = -DCMAKE_BUILD_TYPE=Release

define GENESIS_OS_PRE_INSTALL
	mkdir -p $(HOST_DIR)/$(GNU_TARGET_NAME)/sysroot/usr
endef

GENESIS_OS_POST_PATCH_HOOKS += GENESIS_OS_PRE_INSTALL

define GENESIS_OS_CONFIGURE_CMDS
    cd $(@D) && cmake -DCMAKE_INSTALL_PREFIX=/usr .
endef

define GENESIS_OS_BUILD_CMDS
    $(MAKE) -C $(@D)
endef

define GENESIS_UI_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/genesis-ui $(TARGET_DIR)/usr/bin/genesis-ui
endef

$(eval $(cmake-package))
