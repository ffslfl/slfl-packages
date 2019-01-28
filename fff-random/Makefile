include $(TOPDIR)/rules.mk

PKG_NAME:=fff-random
PKG_VERSION:=0.0.2
PKG_RELEASE:=2

include $(TOPDIR)/../package/gluon.mk

define Package/$(PKG_NAME)
    SECTION:=base
    CATEGORY:=Freifunk
    TITLE:=Freifunk-Franken Random
    URL:=http://www.freifunk-franken.de
    DEPENDS:=+@BUSYBOX_CONFIG_ASH
endef

define Package/$(PKG_NAME)/description
    This is the Freifunk Franken Firmware Random package
    This packages provides a random function.
endef

$(eval $(call BuildPackageGluon,$(PKG_NAME)))
