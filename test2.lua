function echo(...)
  print('echo:', ...)
end

function include(hfilename)
  dofile(hfilename)
end

function define(...)
  return "_______DEFINE______("..table.concat({...}, ', ')..')'
end