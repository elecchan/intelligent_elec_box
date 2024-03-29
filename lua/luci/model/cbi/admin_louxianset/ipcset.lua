-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"
local ut = require "luci.util"

local m, s, o
m = Map("ipcset", translate("IPC设置"))
m:chain("luci")

s = m:section(NamedSection, "ipc", translate("IPC参数"))
s.anonymous = true
s.addremove = false

s:tab("ipc_set",  translate("IPC参数"))

s:taboption("ipc_set", Flag, "fenable",translate("显示楼层信息"))

--type
local ipctype = s:taboption("ipc_set", ListValue, "type",translate("IPC类型"))
ipctype :value(1,translate("海康"))
ipctype :value(2,translate("大华"))
ipctype :value(3,translate("雄迈"))

local ipcversion = s:taboption("ipc_set", ListValue, "ipcversion",translate("型号"))
ipcversion:value("Default")
ipcversion:value("DS-2CD3320D-I")
ipcversion:value("DS-2CC52D5S-IT3")
--username
local ipcuser = s:taboption("ipc_set", Value, "username",translate("用户名"))

--password
local ipcpasswd = s:taboption("ipc_set", Value, "passwd",translate("密码"))

--ipcport
local ipcport = s:taboption("ipc_set", Value, "port",translate("端口号"))

--ipcmess
local ipcmess = s:taboption("ipc_set", Value, "mess",translate("楼层信息设置"))

--ipcaddr
local ipcaddr = s:taboption("ipc_set", Value, "addr",translate("IP地址"))

--ipc xposition
local xposition= s:taboption("ipc_set", Value, "xposition",translate("楼层信息显示X坐标"))

--ipc yposition
local yposition= s:taboption("ipc_set", Value, "yposition",translate("楼层信息显示Y坐标"))

--ipc yposition
local startfloor= s:taboption("ipc_set", Value, "startfloor",translate("开始楼层"))


function m.on_commit(map)
	luci.sys.call("/usr/bin/ipc_conf")
end

return m
