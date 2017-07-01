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

function make_config {
    -- Cleanup tmp dir
    fs.remove("/tmp/fastd_mesh_vpn_peers/")
    fs.mkdir("/tmp/fastd_mesh_vpn_peers/")
    
    

}

if isdir('/tmp/fastd_mesh_vpn_peers') then
    local sumold = io.popen(string.format("exec sha256sum '%s'", "/etc/config/tunneldigger"))
    make_config()
    local sumnew = io.popen(string.format("exec sha256sum '%s'", "/etc/config/tunneldigger"))
    if sumold != sumnew then
        io.popen('/etc/init.d/tunneldigger restart')
    end
    io.popen('/etc/init.d/fastd reload')

    if isdir('/etc/fastd/mesh_vpn/peers/') then
        if not exists(string.format('/proc/%s', io.popen('cat /tmp/run/fastd.mesh_vpn.pid'))) then
            io.popen('/etc/init.d/fastd start')
        end
    else
        if exists(string.format('/proc/%s', io.popen('cat /tmp/run/fastd.mesh_vpn.pid'))) then
            io.popen('/etc/init.d/fastd stop')
        end
    end
    
    io.popen('brctl addif l2p_bridge "mesh_vpn"')
    io.popen('brctl addif l2p_bridge "l2tp"')


else
    fs.mkdir('/tmp/fastd_mesh_vpn_peers')
    if not io.popen('egrep "option secret \'[0-9a-f]{64}\'" /etc/config/fastd &>/dev/null') then
        local secret = io.popen('fastd --generate-key 2>&1 |  awk \'/[Ss]ecret/ { print $2 }\'')
        uci:set('fastd', 'mesh_vpn', 'secret', secret)
        uci:save('fastd')
        uci:commit('fastd')
    end
    make_config()
    io.popen('/etc/init.d/fastd start')
    io.popen('/etc/init.d/tunneldigger start')
    io.popen('brctl addif l2p_bridge "mesh_vpn"')
    io.popen('brctk addif l2p_brisge "l2tp"')
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
