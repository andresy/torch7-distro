package = "compat"
version = "scm-1"

source = {
   url = "git://github.com/andresy/torch7-split.git",
   dir = "torch7-split/rocks/compat"
}

description = {
   summary = "Torch7 compatibility module (with older version)",
   detailed = [[
   ]],
   homepage = "https://github.com/torch/compat",
   license = "BSD"
}

dependencies = {
   "lua >= 5.1",
   "torch >= 7.0",
}

build = {
   type = "builtin",
   modules = {
      ["compat.init"] = "init.lua"
   }
}
