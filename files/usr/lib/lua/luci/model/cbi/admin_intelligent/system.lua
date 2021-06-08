-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"
local ut = require "luci.util"

local m, s, s1, s2, s3, o
m = Map("setting")
m:chain("luci")
----------------------------------------------------------------------------------------
s = m:section(NamedSection, "device", translate("设备信息"))
s.anonymous = true
s.addremove = false
s:tab("device",  translate("设备信息"))
s:taboption("device", Value, "name",translate("设备名称"))
s:taboption("device", Value, "org",translate("所属组织"))
s:taboption("device", Value, "type",translate("设备类型"))
--s:taboption("device", Value, "ip",translate("设备IP"))
s:taboption("device", Value, "location",translate("地理位置"))
----------------------------------------------------------------------------------------
s1 = m:section(NamedSection, "server", translate("服务器配置"))
s1.anonymous = true
s1.addremove = false
s1:tab("server",  translate("服务器配置"))
s1:taboption("server", Value, "ip",translate("服务器IP"))
s1:taboption("server", Value, "port",translate("服务器端口"))
----------------------------------------------------------------------------------------
s3 = m:section(NamedSection, "warm", translate("报警设置"))
s3.anonymous = true
s3.addremove = false
s3:tab("warm",  translate("报警设置"))
s3:taboption("warm", Value, "temperature_h",translate("高温度警报值"))
s3:taboption("warm", Value, "temperature_l",translate("低温度警报值"))
s3:taboption("warm", Value, "humidity_h",translate("高湿度警报值"))
s3:taboption("warm", Value, "humidity_l",translate("低湿度警报值"))
----------------------------------------------------------------------------------------
s2 = m:section(NamedSection, "uart", translate("串口配置"))
s2.anonymous = true
s2.addremove = false
s2:tab("rs232",  translate("rs232配置"))
s2:tab("rs485",  translate("rs485配置"))
--for rs232
local baudrate1 = s2:taboption("rs232", ListValue, "rs232_baud",translate("波特率"))
baudrate1:value(1,translate("115200"))
baudrate1:value(2,translate("57600"))
baudrate1:value(3,translate("38400"))
baudrate1:value(4,translate("19200"))
baudrate1:value(5,translate("9600"))
baudrate1:value(6,translate("4800"))
baudrate1:value(7,translate("2400"))

local databit1 = s2:taboption("rs232", ListValue, "rs232_databit",translate("数据位"))
databit1:value(8,translate("8"))
databit1:value(7,translate("7"))
databit1:value(6,translate("6"))
databit1:value(5,translate("5"))

local parity1 = s2:taboption("rs232", ListValue, "rs232_parity",translate("校验位"))
parity1:value(1,translate("None"))
parity1:value(2,translate("Even"))
parity1:value(3,translate("Odd"))

local stopbit1 = s2:taboption("rs232", ListValue, "rs232_stopbit",translate("停止位"))
stopbit1:value(1,translate("1"))
stopbit1:value(2,translate("2"))

--for rs485
local baudrate2 = s2:taboption("rs485", ListValue, "rs485_baud",translate("波特率"))
baudrate2:value(1,translate("115200"))
baudrate2:value(2,translate("57600"))
baudrate2:value(3,translate("38400"))
baudrate2:value(4,translate("19200"))
baudrate2:value(5,translate("9600"))
baudrate2:value(6,translate("4800"))
baudrate2:value(7,translate("2400"))

local databit2 = s2:taboption("rs485", ListValue, "rs485_databit",translate("数据位"))
databit2:value(8,translate("8"))
databit2:value(7,translate("7"))
databit2:value(6,translate("6"))
databit2:value(5,translate("5"))

local parity2 = s2:taboption("rs485", ListValue, "rs485_parity",translate("校验位"))
parity2:value(1,translate("None"))
parity2:value(2,translate("Even"))
parity2:value(3,translate("Odd"))

local stopbit2 = s2:taboption("rs485", ListValue, "rs485_stopbit",translate("停止位"))
stopbit2:value(1,translate("1"))
stopbit2:value(2,translate("2"))

function m.on_commit(map)
	luci.sys.call("/usr/bin/shm_conf")
end

return m
