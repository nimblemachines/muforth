" autodetect Markdown files from .md name, and set filetype
" Bitbucket wiki uses .md extension for Markdown; but Vim's default filetype
" detection thinks this is a Modula-2 file!
au BufRead,BufNewFile *.md		set filetype=markdown
