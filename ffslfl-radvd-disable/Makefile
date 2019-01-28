include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-radvd-disable
PKG_VERSION:=2
PKG_RELEASE:=1

include $(TOPDIR)/../package/gluon.mk

define Package/$(PKG_NAME)
	SECTION:=base
	CATEGORY:=Freifunk
	TITLE:= Freifunk-Schleswig-Flensburg radvd-disable
	URL:=http://ffslfl.net
endef

define Package/$(PKG_NAME)/description
	This package stops radvd from working
endef

$(eval $(call BuildPackageGluon,$(PKG_NAME)))
