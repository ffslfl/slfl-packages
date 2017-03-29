include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-fastd
PKG_VERSION:=1
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/ffslfl-fastd
    SECTION:=base
    CATEGORY:=Freifunk
    TITLE:=Freifunk-Schleswig-Flensburg fastd configuration script
    URL:=http://ffslfl.net
    DEPENDS:=+@BUSYBOX_CUSTOM \
             +@FASTD_ENABLE_METHOD_NULL \
             +@FASTD_ENABLE_CIPHER_NULL \
             +@FASTD_WITH_STATUS_SOCKET \
             +fastd \
             +fff-random
endef

define Package/ffslfl-fastd/description
    This is the fastd for the Freifunk Schleswig-Flensburg Firmware
    It is used to configure fastd.
		It is based on the version from Freifunk Franken
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Package/ffslfl-fastd/install
	$(CP) ./files/* $(1)/
endef

define Build/Compile
			#Nothing to do
endef

$(eval $(call BuildPackage,ffslfl-fastd))
