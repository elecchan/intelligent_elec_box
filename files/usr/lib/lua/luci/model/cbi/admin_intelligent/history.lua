-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"
local ut = require "luci.util"

local m, s, o
m = Map("history")
m:chain("luci")

s = m:section(TypedSection, "history")
s.anonymous = true
--s.addremove = false

s:tab("history",  translate("历史记录"))
conf = s:taboption("history", Value, "editconf", nil)
conf.template = "cbi/tvalue"
conf.rows = 20
conf.cols = 1
conf.wrap = "off"

function conf.cfgvalue(self, section)
return fs.readfile("/etc/history") or ""
end


return m



