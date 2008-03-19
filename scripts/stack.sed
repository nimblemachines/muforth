# change the names around...

# T -> TOP
s/([^a-zA-Z])T([^a-zA-Z])/\1TOP\2/g

# SND -> ST1
s/([^a-zA-Z])SND([^a-zA-Z])/\1ST1\2/g

# TRD -> ST2
s/([^a-zA-Z])TRD([^a-zA-Z])/\1ST2\2/g

# SP[2] -> ST3
s/([^a-zA-Z])SP\[2\]/\1ST3/g


# NIP; -> NIPN(1);
s/([^a-zA-Z])NIP;/\1NIPN\(1\);/g

# NIP2; -> NIPN(2);
s/([^a-zA-Z])NIP2;/\1NIPN\(2\);/g

# NIPN(n); -> NIP(n);
s/([^a-zA-Z])NIPN(\(-?[0-9]+\);)/\1NIP\2/g


# DROP; -> DROPN(1);
s/([^a-zA-Z])DROP;/\1DROPN\(1\);/g

# DROP2; -> DROPN(2);
s/([^a-zA-Z])DROP2;/\1DROPN\(2\);/g

# DROPN(n); -> DROP(n);
s/([^a-zA-Z])DROPN(\(-?[0-9]+\);)/\1DROP\2/g


