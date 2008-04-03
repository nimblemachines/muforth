" my customization of mu4 (muFORTH) settings

" expand tabs into spaces
setlocal expandtab

" indent by 3
setlocal shiftwidth=3

" what was used on existing code
setlocal textwidth=75

" so auto-formatting works nicely - but only do it in comments, not in code
setlocal formatoptions=rcoql

" set comment strings
" s = start comment (2 means middle indented by 2 spaces);
" m = middle
" e = end
" b = text must be followed by whitespace
setlocal comments=s2:(\ ,m:\ \ ,e:),b:--

