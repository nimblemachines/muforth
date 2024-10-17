-- This file is part of muforth: https://muforth.dev/

-- Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)

-- Code to convert multi-line comments to a new style.

-- We could use the contents of leader as a flag whether we are in a
-- comment or not, but for clarity I am using a separate variable,
-- in_comment.
function convert()
    local comment_type = "none"     -- outside a comment
    local leader, leader_match, before_lparen, after_lparen
    local token

    for l in io.lines() do          -- read stdin line by line
        if comment_type == "none" then
            -- Check for "comment" followed by a token. This begins a
            -- token-style comment.
            c, token = l:match "^%s*(%S+)%s+(%S+)"
            if c and c == "comment" then
                comment_type = "token"
                before_lparen = ""      -- treat as if "(" in leftmost column
            else
                -- Maybe we have "comment" by itself on a line and the
                -- token on the following line?
                c = l:match "^%s*(%S+)%s*$"
                if c and c == "comment" then
                    -- We have to get the token from the following
                    -- line!
                    comment_type = "expecting a token"
                else
                    -- No "comment" found. Check for a left paren. This
                    -- begins a paren-style comment.
                    before_lparen, after_lparen = l:match "^(%s*)%((%s+)"
                    if before_lparen then
                        -- We found a left paren, but this could be a one-line
                        -- comment, so let's look for a right paren before doing
                        -- anything else.
                        if not l:match "%)" then
                            -- No right paren; this must be a multi-line comment.
                            comment_type = "paren"
                            leader       =         before_lparen .. "|" .. after_lparen
                            leader_match = "^(" .. before_lparen .. " " .. after_lparen .. ")"
                        end
                    end
                end
            end
        elseif comment_type == "expecting a token" then
            comment_type = "token"
            before_lparen = ""      -- treat as if "(" in leftmost column

            -- Sometimes the token is an ascii horizontal line of
            -- chars, and the closing token is meant to align with it.
            -- Since we are going to be indenting every line with two
            -- chars - "| " - which will move the closing over two
            -- spaces, let's move the *opening* token over two spaces
            -- as well!
            local sp
            sp, token = l:match "^(%s*)(%S+)"
            l = sp .. "  " .. token
        else
            -- in comment; modify line
            -- comment_type should be "paren" or "token"

            -- The empty line test works for both paren and token comment
            -- styles.
            if l:match "^%s*$" then
                -- Empty line or only whitespace; replace with indent and
                -- vertical bar.
                l = before_lparen .. "|"
            elseif comment_type == "paren" then
                local changes
                l, changes = l:gsub(leader_match, leader)
                if changes == 0 then
                    -- l was not changed. Most likely the line doesn't have
                    -- enough white space before the text for leader_match
                    -- to match, or it was already converted.

                    -- Try matching bare | or ) and fixing its indent.
                    local bare = l:match "^%s*([|)])%s*$"
                    if bare then
                        l = before_lparen .. bare
                    elseif not l:match("^" .. leader) then
                        -- If bare | or ) did *not* match *and* comment
                        -- line *not* already in new format, let's change
                        -- it. Match whatever leading white space there is
                        -- - including none at all! - and replace it with
                        -- the leader text.
                        l = l:gsub("^(%s*)", leader)
                    end
                end
                if l:match "%)" then
                    comment_type = "none"
                end
            elseif comment_type == "token" then
                for w in l:gmatch "%S+" do      -- tokenize line
                    if w == token then
                        -- found token in the text of the line
                        comment_type = "none"
                    end
                end
                -- Prefix text with vertical bar.
                l = "| " .. l
            else
                out(string.format("Comment type is %s\n", comment_type))
                os.exit(1)
            end
        end
        -- Whether or not the line was changed, write it to stdout.
        io.write(l .. "\n")
    end
end

convert()
