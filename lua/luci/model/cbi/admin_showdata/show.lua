-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"
local ut = require "luci.util"

local m, s, o
m = Map("network", translate("网络设置"))
m:chain("luci")

s = m:section(NamedSection, "lan", translate("网络设置"))
s.anonymous = true
s.addremove = false

s:tab("net_set",  translate("网络设置"))

local addr = s:taboption("net_set", Value, "ipaddr",translate("IP设置"))
local netmask = s:taboption("net_set", Value, "netmask",translate("Netmask"))
local macaddr = s:taboption("net_set", Value, "macaddr",translate("MAC地址"),translate("MAC地址不可修改"))

return m


