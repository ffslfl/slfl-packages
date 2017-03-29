include $(TOPDIR)/rules.mk

PKG_NAME:=ffslfl-tunneldigger
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ffslfl-tunneldigger
    SECTION:=base
    CATEGORY:=Freifunk
    TITLE:= Freifunk-Schleswig-Flensburg tunneldigger
    URL:=http://ffslfl.net
    DEPENDS:=+tunneldigger
endef

define Package/ffslfl-tunneldigger/description
    This is tunneldigger for the Freifunk Schleswig-Flensburg Firmware.
		It is based on the version made by Freifunk Franken
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Package/ffslfl-tunneldigger/install
        $(CP) ./files/* $(1)/
endef

define Build/Compile
			#Nothing to do
endef

$(eval $(call BuildPackage,ffslfl-tunneldigger))
