# This file is part of muforth: http://muforth.nimblemachines.com/
#
# Copyright (c) 2002-2018 David Frech. (Read the LICENSE for details.)

#
# For each of the generated dictionary chain initializers, we want to keep the
# filename comments and preprocessor #ifdef lines, passing them through
# unchanged.
#

# keep the /* filename.c */ comments
/^\/\* .*\.c/ {
i\
\

s/^\//	\//
p
}

# keep any #if* #else or #endif lines, so we can include optional sections
/^#(if|else|endif)/p
