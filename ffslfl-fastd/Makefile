include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-fastd
PKG_VERSION:=2
PKG_RELEASE:=2
PKG_BUILD_DEPENDS := respondd

include $(TOPDIR)/../package/gluon.mk

define Package/$(PKG_NAME)
    SECTION:=base
    CATEGORY:=Freifunk
    TITLE:=Freifunk-Schleswig-Flensburg fastd configuration script
    URL:=http://ffslfl.net
    DEPENDS:=+@BUSYBOX_CUSTOM \
             +@FASTD_ENABLE_METHOD_NULL \
             +@FASTD_ENABLE_CIPHER_NULL \
             +@FASTD_WITH_STATUS_SOCKET \
             +fastd \
             +fff-random \
	     +libgluonutil \
	     +gluon-mesh-vpn-core \
	     +gluon-core
endef

define Package/$(PKG_NAME)/description
    This is the fastd for the Freifunk Schleswig-Flensburg Firmware
    It is used to configure fastd.
		It is based on the version from Freifunk Franken
endef

$(eval $(call BuildPackageGluon,$(PKG_NAME)))
