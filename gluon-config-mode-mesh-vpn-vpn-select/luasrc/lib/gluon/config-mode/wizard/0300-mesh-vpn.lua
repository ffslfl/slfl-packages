local simpleUci = require("simple-uci").cursor()

return function(form, uci)

    local pkg_i18n = i18n 'gluon-config-mode-mesh-vpn-vpn-select'

	local msg = pkg_i18n.translate(
		'Your internet connection can be used to establish a ' ..
	        'VPN connection with other nodes. ' ..
	        'Enable this option if there are no other nodes reachable ' ..
	        'over WLAN in your vicinity or you want to make a part of ' ..
	        'your connection\'s bandwidth available for the network. You can limit how ' ..
	        'much bandwidth the node will use at most.'
	)

	local s = form:section(Section, nil, msg)

	local o

	local meshvpn = s:option(ListValue, "meshvpn", pkg_i18n.translate("Select VPN Type to use for internet connection (mesh VPN)"))
	local meshvpn_activated
	
	meshvpn:value("tunneldigger", pkg_i18n.translate("Tunneldigger - L2TP (faster but unencrypted)"))
	meshvpn:value("fastd", pkg_i18n.translate("Fastd (slower but encrypted)"))
	meshvpn:value("disabled", pkg_i18n.translate("Disable"))
	
	if uci:get_bool("tunneldigger", "mesh_vpn", "enabled") or uci:get_bool("tunneldigger", "mesh_vpn", "enabled") == "1" then
		meshvpn.default = "tunneldigger"
	else
		meshvpn.default = "fastd"
	end

	function meshvpn:write(data)
		if data == "fastd" then
			uci:set("fastd", "mesh_vpn", "enabled", "1")
			simpleUci:section('tunneldigger', 'broker', 'mesh_vpn', {
                             enabled = "0",
                        })
			meshvpn_activated = true
		end
		if data == "tunneldigger" then
			uci:set("fastd", "mesh_vpn", "enabled", "0")
			simpleUci:section('tunneldigger', 'broker', 'mesh_vpn', {
                             enabled = "1",
                        })
			meshvpn_activated = true
		end
		if data == "disabled" then
			uci:set("fastd", "mesh_vpn", "enabled", "0")
			simpleUci:section('tunneldigger', 'broker', 'mesh_vpn', {
                             enabled = "0",
                        })
			meshvpn_activated = false
		end
		simpleUci:commit('tunneldigger')
	end

	local limit = s:option(Flag, "limit_enabled", pkg_i18n.translate("Limit bandwidth"))
	limit:depends(meshvpn_activated, true)
	limit.default = uci:get_bool("simple-tc", "mesh_vpn", "enabled")
	function limit:write(data)
		uci:set("simple-tc", "mesh_vpn", "interface")
		uci:set("simple-tc", "mesh_vpn", "enabled", data)
		uci:set("simple-tc", "mesh_vpn", "ifname", "mesh-vpn")
	end

	o = s:option(Value, "limit_ingress", pkg_i18n.translate("Downstream (Mbit/s)"))
	o:depends(limit, true)
    o.default = div(uci:get("gluon", "mesh_vpn", "limit_ingress"), 1000)
    o.datatype = "ufloat"
	function o:write(data)
		uci:set("gluon", "mesh_vpn", "limit_ingress", data * 1000)
	end

	o = s:option(Value, "limit_egress", pkg_i18n.translate("Upstream (Mbit/s)"))
	o:depends(limit, true)
    o.default = div(uci:get("gluon", "mesh_vpn", "limit_egress"), 1000)
    o.datatype = "ufloat"
	function o:write(data)
		uci:set("gluon", "mesh_vpn", "limit_egress", data * 1000)
	end

	return {'fastd', 'tunneldigger', 'simple-tc'}
end
