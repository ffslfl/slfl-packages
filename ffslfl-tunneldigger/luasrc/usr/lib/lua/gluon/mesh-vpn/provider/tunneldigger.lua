local uci = require('simple-uci').cursor()

local vpn_core = require 'gluon.mesh-vpn'

local M = {}

function M.public_key()
	return nil
end

function M.enable(val)
	uci:set('tunneldigger', 'mesh_vpn', 'enabled', val)
	uci:save('tunneldigger')
end

function M.active()
	return true
end

function M.enabled()
	return uci:get_bool("tunneldigger", "mesh_vpn", "enabled") or uci:get("tunneldigger", "mesh_vpn", "enabled") == "1" or uci:get('tunneldigger', 'broker', 'mesh_vpn', 'enabled') == "1"
end

function M.set_limit(ingress_limit, egress_limit)
	if ingress_limit ~= nil then
		uci:set('tunneldigger', 'mesh_vpn', 'limit_bw_down', ingress_limit)
	else
		uci:delete('tunneldigger', 'mesh_vpn', 'limit_bw_down')
	end

	if egress_limit ~= nil then
		uci:section('simple-tc', 'interface', 'mesh_vpn', {
			ifname = vpn_core.get_interface(),
			enabled = true,
			limit_egress = egress_limit,
		})
	else
		uci:delete('simple-tc', 'mesh_vpn')
	end

	uci:save('tunneldigger')
	uci:save('simple-tc')
end

return M