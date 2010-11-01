# convert
#   cr " File description " file[#      and
#   loading" File description"          to
#   loading( File description)
# and delete #]file

s/^cr +" +(.*) +" +file\[#/loading \1/
s/^loading" +(.*) +"/loading \1/
/^#\]file/d
