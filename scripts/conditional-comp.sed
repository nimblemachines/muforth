# lines that start ": [with]" need to be replaced
/^: \[with\] / {
c\
: .ifdef   \\ .ifdef ;\
: .ifndef  \\ .ifndef ;\
: .def     \\ .def ;
}

# change [with] foo [if]  ->  .ifdef foo
s/\[with\] +([[:graph:]]+) \[if\]/.ifdef \1/g

# change [with] foo [not] [if]  ->  .ifndef foo
s/\[with\] +([[:graph:]]+) \[not\] \[if\]/.ifndef \1/g

# change remaining [with] foo  ->  .def foo
s/\[with\] +([[:graph:]]+)/.def \1/g

# change [if] [else] [then] [and] [or] [not]  ->
#        .if .else .then .and .or .not
s/\[(if|else|then|and|or|not)\]/.\1/g

