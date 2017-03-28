include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-vpn-select
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ffslfl-vpn-select
    SECTION:=base
    CATEGORY:=Freifunk
    TITLE:= Freifunk-Schleswig-Flensburg vpn-select
    URL:=http://ffslfl.net
    DEPENDS:=+@BUSYBOX_CONFIG_FEATURE_WGET_TIMEOUT \
             +ffslfl-tunneldigger \
             +ffslfl-fastd
endef

define Package/ffslfl-vpn-select/description
    This package selects and starts the VPN
    In this version fastd and l2tp via tunneldigger.
		It is based on the version from Freifunk Franken
endef

define Package/ffslfl-vpn-select/install
       $(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ffslfl-vpn-select))
