package = "cwrap"
version = "scm-1"

source = {
   url = "git://github.com/andresy/torch7-split.git",
   dir = "torch7-split/rocks/cwrap"
}

description = {
   summary = "Advanced automatic wrapper for C functions",
   detailed = [[
   ]],
   homepage = "https://github.com/torch/cwrap",
   license = "BSD"
}

dependencies = {
   "lua >= 5.1",
}

build = {
   type = "builtin",
   modules = {
      ["cwrap.init"] = "init.lua",
      ["cwrap.cinterface"] = "cinterface.lua",
      ["cwrap.types"] = "types.lua",
   },
   install = {
      lua = {
         ["cwrap.README"] = "README.md"
      }
   }
}
