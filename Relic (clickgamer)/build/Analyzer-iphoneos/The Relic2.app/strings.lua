function wrap(str, limit, indent, indent1)
  indent = indent or ""
  indent1 = indent1 or indent
  limit = limit or 72
  local here = 1-#indent1
  return indent1..str:gsub("(%s+)()(%S+)()",
                          function(sp, st, word, fi)
                            if fi-here > limit then
                              here = st - #indent
                              return "\n"..indent..word
                            end
                          end)
end

function reflow(str, limit, indent, indent1)
  return (str:gsub("%s*\n%s+", "\n")
             :gsub("%s%s+", " ")
             :gsub("[^\n]+",
                   function(line)
                     return wrap(line, limit, indent, indent1)
                   end))
end
