include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-legacy-upgrade
PKG_VERSION:=2
PKG_RELEASE:=1

include $(TOPDIR)/../package/gluon.mk

define Package/$(PKG_NAME)
	SECTION:=base
	CATEGORY:=Freifunk
	TITLE:= Freifunk-Schleswig-Flensburg legacy-upgrade
	URL:=http://ffslfl.net
	DEPENDS:= +gluon-core
endef

define Package/$(PKG_NAME)/description
	Supposed to upgrade the 0.3.0 config to the newer config format
endef

$(eval $(call BuildPackageGluon,$(PKG_NAME)))
