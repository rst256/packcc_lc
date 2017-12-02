
  print('test2h.lua')
function onFunctionDefined(fn, ...)
  print("onFunctionDefined:", '`'..fn.result..'` '..fn.name..' (`'..table.concat(fn, '`, `')..'`)')
end