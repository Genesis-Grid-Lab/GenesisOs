# external/external.mk

# Optionally include hooks or configs, but can be empty
GENESISOS_EXTERNAL_PACKAGES += genesis-ui

include $(sort $(wildcard $(BR2_EXTERNAL_GenesisOS_PATH)/package/*/*.mk))

