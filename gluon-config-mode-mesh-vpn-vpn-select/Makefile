include $(TOPDIR)/rules.mk

PKG_NAME:=gluon-config-mode-mesh-vpn-vpn-select
PKG_VERSION:=3

include $(TOPDIR)/../package/gluon.mk

PKG_CONFIG_DEPENDS += $(GLUON_I18N_CONFIG)

define Package/$(PKG_NAME)
  SECTION:=gluon
  CATEGORY:=Gluon
  TITLE:=Toggle mesh-vpn and bandwidth limit
  DEPENDS:=gluon-config-mode-core +gluon-mesh-vpn-core
endef

define Build/Compile
	$(call GluonBuildI18N,gluon-config-mode-mesh-vpn-vpn-select,i18n)
	$(call GluonSrcDiet,./luasrc,$(PKG_BUILD_DIR)/luadest/)
endef

define Package/gluon-config-mode-mesh-vpn-vpn-select/install
	$(CP) $(PKG_BUILD_DIR)/luadest/* $(1)/
	$(call GluonInstallI18N,gluon-config-mode-mesh-vpn-vpn-select,$(1))
endef

$(eval $(call BuildPackageGluon,$(PKG_NAME)))
