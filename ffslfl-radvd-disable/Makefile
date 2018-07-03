include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-radvd-disable
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/../package/gluon.mk

define Package/ffslfl-radvd-disable
	SECTION:=base
	CATEGORY:=Freifunk
	TITLE:= Freifunk-Schleswig-Flensburg radvd-disable
	URL:=http://ffslfl.net
endef

define Package/ffslfl-radvd-disable/description
	This package stops radvd from working
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/ffslfl-radvd-disable/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ffslfl-radvd-disable))
