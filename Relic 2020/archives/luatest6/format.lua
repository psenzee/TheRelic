local data = Pack("&i8&s8&f6.10&z&f", 12, "hi!", 23.44, "hi!", 23.44)
print(data)
local twelve, hi1, floatVal1, hi2, floatVal2 = Unpack("&i8&s8&f6.10&z&f", data)
print(twelve, " | ", hi1, " | ", floatVal1, " | ", hi2, " | ", floatVal2)

function PackAndPrint(...)
  print(Pack(...))
end
