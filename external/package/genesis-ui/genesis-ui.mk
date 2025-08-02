GENESIS_UI_VERSION = 1.0
GENESIS_UI_SITE = $(BR2_EXTERNAL_GenesisOS_PATH)/package/genesis-ui/src
GENESIS_UI_SITE_METHOD = local
GENESIS_UI_DEPENDENCIES = sdl2 mesa3d libgles libegl

define GENESIS_UI_PRE_INSTALL
	mkdir -p $(HOST_DIR)/$(GNU_TARGET_NAME)/sysroot/usr
endef

GENESIS_UI_POST_PATCH_HOOKS += GENESIS_UI_PRE_INSTALL

define GENESIS_UI_CONFIGURE_CMDS
    cd $(@D) && cmake -DCMAKE_INSTALL_PREFIX=/usr .
endef

define GENESIS_UI_BUILD_CMDS
    $(MAKE) -C $(@D)
endef

define GENESIS_UI_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/genesis-ui $(TARGET_DIR)/usr/bin/genesis-ui
endef

$(eval $(cmake-package))
