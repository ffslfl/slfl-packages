include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-legacy-upgrade
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/../package/gluon.mk

define Package/ffslfl-legacy-upgrade
	SECTION:=base
	CATEGORY:=Freifunk
	TITLE:= Freifunk-Schleswig-Flensburg legacy-upgrade
	URL:=http://ffslfl.net
	DEPENDS:= +gluon-core
endef

define Package/ffslfl-legacy-upgrade/description
	Supposed to upgrade the 0.3.0 config to the newer config format
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/ffslfl-legacy-upgrade/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ffslfl-legacy-upgrade))
