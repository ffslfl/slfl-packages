include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-tunneldigger
PKG_VERSION:=2
PKG_RELEASE:=1

include $(TOPDIR)/../package/gluon.mk

define Package/$(PKG_NAME)
    SECTION:=base
    CATEGORY:=Freifunk
    TITLE:= Freifunk-Schleswig-Flensburg tunneldigger
    URL:=http://ffslfl.net
    DEPENDS:=+tunneldigger
endef

define Package/$(PKG_NAME)/description
    This is tunneldigger for the Freifunk Schleswig-Flensburg Firmware.
		It is based on the version made by Freifunk Franken
endef

$(eval $(call BuildPackageGluon,$(PKG_NAME)))
