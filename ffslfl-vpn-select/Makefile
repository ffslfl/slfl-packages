include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-vpn-select
PKG_VERSION:=2
PKG_RELEASE:=1

include $(TOPDIR)/../package/gluon.mk

define Package/$(PKG_NAME)
	SECTION:=base
	CATEGORY:=Freifunk
	TITLE:= Freifunk-Schleswig-Flensburg vpn-select
	URL:=http://ffslfl.net
	DEPENDS:=+@BUSYBOX_CONFIG_FEATURE_WGET_TIMEOUT \
		+ffslfl-tunneldigger \
		+ffslfl-fastd \
		+gluon-core \
		+gluon-mesh-vpn-core
endef

define Package/$(PKG_NAME)/description
	This package selects and starts the VPN
	In this version fastd and l2tp via tunneldigger.
	It is based on the version from Freifunk Franken
endef

$(eval $(call BuildPackageGluon,$(PKG_NAME)))
