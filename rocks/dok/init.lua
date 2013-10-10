local sd = require 'sundown'

dok = {}

if torch then
   require 'dok.inline'
end

dok.dok2html = sd.render
