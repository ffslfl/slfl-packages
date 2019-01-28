include $(TOPDIR)/rules.mk

PKG_NAME:=gluon-quickfix
PKG_VERSION:=3

include $(TOPDIR)/../package/gluon.mk

define Package/$(PKG_NAME)
  SECTION:=gluon
  CATEGORY:=Workaround
  TITLE:=Reboot under certain conditions (loss of mesh neighbours, broken client wifi, unsuccessful anycast ping)
  DEPENDS:=+gluon-core +micrond
endef

define Package/$(PKG_NAME)/install
	$(CP) ./files/* $(1)/
	./gluonShellDiet.sh $(1)/lib/gluon/quickfix/quickfix.sh
endef

$(eval $(call BuildPackageGluon,$(PKG_NAME)))
