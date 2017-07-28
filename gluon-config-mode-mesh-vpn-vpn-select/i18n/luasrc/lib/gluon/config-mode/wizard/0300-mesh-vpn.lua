local fs = require "nixio.fs"

return function(form, uci)
	local msg = translate(
		'Your internet connection can be used to establish a ' ..
	        'VPN connection with other nodes. ' ..
	        'Enable this option if there are no other nodes reachable ' ..
	        'over WLAN in your vicinity or you want to make a part of ' ..
	        'your connection\'s bandwidth available for the network. You can limit how ' ..
	        'much bandwidth the node will use at most.'
	)

	local s = form:section(Section, nil, msg)

	local o

	local meshvpn = s:option(ListValue, "meshvpn", translate("Select VPN Type to use for internet connection (mesh VPN)"))
	meshvpn.default = "Tunneldigger - L2TP (faster but unencrypted)"
	meshvpn:value("Tunneldigger (faster but unencrypted)")
	meshvpn:value("Fastd (slower but encrypted)")
	meshvpn:value("Disable")
	function meshvpn:write(data)
		if data == "Fastd (slower but encrypted)" then
			uci:set("fastd", "mesh_vpn", "enabled", "1")
			uci:set("tunneldigger", "mesh_vpn", "enabled", "0")
		end
		if data == "Tunneldigger - L2TP (faster but unencrypted)" then
			uci:set("fastd", "mesh_vpn", "enabled", "0")
			uci:set("tunneldigger", "mesh_vpn", "enabled", "1")
		end
		if data == "Disable" then
			uci:set("fastd", "mesh_vpn", "enabled", "0")
			uci:set("tunneldigger", "mesh_vpn", "enabled", "0")
		end
		uci:commit("fastd")
		uci:commit("tunneldigger")
	end

	local limit = s:option(Flag, "limit_enabled", translate("Limit bandwidth"))
	limit:depends(meshvpn, true)
	limit.default = uci:get_bool("simple-tc", "mesh_vpn", "enabled")
	function limit:write(data)
		uci:set("simple-tc", "mesh_vpn", "interface")
		uci:set("simple-tc", "mesh_vpn", "enabled", data)
		uci:set("simple-tc", "mesh_vpn", "ifname", "mesh-vpn")
	end

	o = s:option(Value, "limit_ingress", translate("Downstream (kbit/s)"))
	o:depends(limit, true)
	o.default = uci:get("simple-tc", "mesh_vpn", "limit_ingress")
	o.datatype = "uinteger"
	function o:write(data)
		uci:set("simple-tc", "mesh_vpn", "limit_ingress", data)
	end

	o = s:option(Value, "limit_egress", translate("Upstream (kbit/s)"))
	o:depends(limit, true)
	o.default = uci:get("simple-tc", "mesh_vpn", "limit_egress")
	o.datatype = "uinteger"
	function o:write(data)
		uci:set("simple-tc", "mesh_vpn", "limit_egress", data)
	end

	return {'fastd', 'tunneldigger', 'simple-tc'}
end
