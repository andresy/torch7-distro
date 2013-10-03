package = "gnuplot"
version = "scm-1"

source = {
   url = "git://github.com/andresy/torch7-split.git",
   dir = "torch7-split/rocks/gnuplot"
}

description = {
   summary = "Torch interface to Gnuplot",
   detailed = [[
   ]],
   homepage = "https://github.com/torch/gnuplot",
   license = "BSD"
}

dependencies = {
   "lua >= 5.1",
   "torch >= 7.0",
   "paths >= 1.0",
}

build = {
   type = "builtin",
   modules = {
      ["gnuplot.init"] = "init.lua",
      ["gnuplot.gnuplot"] = "gnuplot.lua",
      ["gnuplot.hist"] = "hist.lua",
   }
}
