" my customization of mu4 (muFORTH) settings

" expand tabs into spaces
setlocal expandtab

" indent by 3
setlocal shiftwidth=3

" softtabstop makes things better!
" allows the hard tabstop to remain set to 8, but using <BS> and <Tab>
" it feels like real tab are being used.
setlocal softtabstop=3

" smarttab too!
" makes tabbing and BSing at the start of a line feel like using real tabs.
setlocal smarttab

" what I used with emacs - so not _everything_ changes ;-)
setlocal textwidth=75

" so auto-formatting works nicely - but only do it in comments, not in code
" t = text (ie, code other than comments)
" r = put comment start on new lines opened by pressing <Return>
" o = ditto, on lines opened by o or O
setlocal formatoptions=rocql

" set comment strings
" s = start comment
" m = middle
" e = end
" b = text must be followed by whitespace
setlocal comments=s:(\ ,m:\ \ ,ex:),b:--

" Make keyword matching better (so w and friends work better). Since in Forth
" anything can be a keyword, let's let Vim know that. @ means all alphanumeric
" (including international characters); then we add all the printable ASCII,
" including punctuation.
setlocal iskeyword=@,!-~
