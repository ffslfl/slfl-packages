#!/usr/bin/lua

local site = require 'gluon.site_config'
local util = require 'gluon.util'
local fs = require 'nixio.fs'

local uci = require('simple-uci').cursor()
--- Check if a file or directory exists in this path
function exists(file)
    local ok, err, code = os.rename(file, file)
    if not ok then
        if code == 13 then
            -- Permission denied, but it exists
            return true
        end
    end
    return ok, err
end

--- Check if a directory exists in this path
function isdir(path)
    return exists(path.."/")
end

uci:section('firewall', 'include', 'mesh_vpn_dns', {
    type = 'restore',
    path = '/lib/gluon/vpn-select/iptables.rules',
    family = 'ipv4',
})

local mac_raw = util.trim(fs.readfile('/sys/class/net/br-wan/address'))
local mac = mac_raw.gsub(":", "").upper
local hostname = util.trim(fs.readfile('/proc/sys/kernel/hostname'))
local port = ''
local pubkey = uci:get("fastd", "mesh_vpn", "pubkey")
local lat = uci:get_first('gluon-node-info', 'location', 'latitude')
local long = uci:get_first('gluon-node-info', 'location', 'longitude')

if pubkey == '' then
    io.popen('uci set fastd.mesh_vpn.pubkey="secret \"$(uci get fastd.mesh_vpn.secret)\";" | fastd -c - --show-key --machine-readable')
    io.popen('uci commit fastd')
    pubkey = uci:get("fastd", "mesh_vpn", "pubkey")
end

if isdir('/tmp/fastd_mesh_vpn_peers') then
    -- WOOOOW
else
    fs.mkdir('/tmp/fastd_mesh_vpn_peers')
end














io.popen('wget -4 -T15 "http://keyserver.ffslfl.net/index.php?mac=' .. mac .. '&name=' .. hostname .. '&port=' .. port .. '&key=' .. pubkey.. '&lat=' .. lat .. '&long=' .. long.. ' -O /tmp/fastd_ffslfl_output ', 'r')



local enabled = uci:get('tunneldigger', 'mesh_vpn', 'enabled')
if enabled == nil then
    if uci:get_first('tunneldigger', 'broker', 'interface') == "mesh-vpn" then
        enabled = uci:get_first('tunneldigger', 'broker', 'enabled')
    end
end
if enabled == nil then
    enabled = site.mesh_vpn.enabled or false
end

-- Delete old broker config section
if not uci:get('tunneldigger', 'mesh_vpn') then
    uci:delete_all('tunneldigger', 'broker')
end

uci:section('tunneldigger', 'broker', 'mesh_vpn', {
    enabled = enabled,
    uuid = util.node_id(),
    interface = 'mesh-vpn',
    bind_interface = 'br-wan',
    group = 'gluon-mesh-vpn',
    broker_selection = 'usage',
    address = site.mesh_vpn.tunneldigger.brokers,
})

uci:save('tunneldigger')