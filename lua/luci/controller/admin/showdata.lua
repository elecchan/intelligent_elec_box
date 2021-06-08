-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

module("luci.controller.admin.showdata", package.seeall)

function index()
	entry({"admin", "showdata", "show"}, cbi("admin_showdata/show"), _("数据展示"), 1).index=true
end