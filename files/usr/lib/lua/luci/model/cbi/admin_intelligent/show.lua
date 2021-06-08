-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"
local ut = require "luci.util"

local m, s, s1, s2, s3, o
m = Map("show")
m:chain("luci")

s = m:section(NamedSection, "sensor", translate("传感器"))
s.anonymous = true
s.addremove = false
s:tab("sensor",  translate("传感器"))
local temp = s:taboption("sensor", Value, "temp",translate("温度"),translate("只读"))
local hum = s:taboption("sensor", Value, "hum",translate("湿度"),translate("只读"))
local light = s:taboption("sensor", Value, "light",translate("光照度"),translate("只读"))

s1 = m:section(NamedSection, "switch", translate("开关量"))
s1.anonymous = true
s1.addremove = false
s1:tab("switch",  translate("开关量"))
s1:taboption("switch", Value, "volout8",translate("8路电源输出"))
s1:taboption("switch", Value, "volout2",translate("2路24V输出"))
s1:taboption("switch", Value, "switchout",translate("继电器输出状态"))
s1:taboption("switch", Value, "switchin",translate("光耦输入状态"),translate("只读"))

s2 = m:section(NamedSection, "device", translate("外设状态"))
s2.anonymous = true
s2.addremove = false
s2:tab("device",  translate("外设状态"))
s2:taboption("device", Flag, "fan",translate("风扇"),translate("只读"))
s2:taboption("device", Flag, "heat",translate("加热棒"),translate("只读"))
s2:taboption("device", Flag, "door",translate("箱门状态"),translate("只读"))
s2:taboption("device", Flag, "water",translate("箱门漏水"),translate("只读"))
s2:taboption("device", Flag, "led",translate("灯带状态"),translate("只读"))

s3 = m:section(NamedSection, "server", translate("网络状态"))
s3.anonymous = true
s3.addremove = false
s3:tab("server",  translate("网络状态"))
s3:taboption("server", Flag, "link",translate("服务器连接状态"),translate("只读"))

function m.on_commit(map)
	luci.sys.call("/usr/bin/shm_conf")
end

return m


