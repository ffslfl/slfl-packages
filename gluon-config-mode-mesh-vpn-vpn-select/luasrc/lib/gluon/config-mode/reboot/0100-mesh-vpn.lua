local uci = require("simple-uci").cursor()
local lutil = require "gluon.web.util"

local site = require 'gluon.site_config'
local sysconfig = require 'gluon.sysconfig'
local util = require "gluon.util"

local pretty_hostname = require 'pretty_hostname'

local hostname = pretty_hostname.get(uci)
local contact = uci:get_first("gluon-node-info", "owner", "contact")

local pubkey
local msg

local pkg_i18n = i18n 'gluon-config-mode-mesh-vpn-vpn-select'

if uci:get_bool("tunneldigger", "mesh_vpn", "enabled") or uci:get_bool("tunneldigger", "mesh_vpn", "enabled") == "1" then
	msg = pkg_i18n._translate('gluon-config-mode:tunneldigger')
elseif uci:get_bool("fastd", "mesh_vpn", "enabled") or uci:get_bool("fastd", "mesh_vpn", "enabled") == "1" then
	pubkey = util.trim(lutil.exec("/etc/init.d/fastd show_key mesh_vpn"))
	msg = pkg_i18n._translate('gluon-config-mode:pubkey')
else
	msg = pkg_i18n._translate('gluon-config-mode:novpn')
end

if not msg then return end

renderer.render_string(msg, {
	pubkey = pubkey,
	hostname = hostname,
	site = site,
	sysconfig = sysconfig,
	contact = contact,
})
