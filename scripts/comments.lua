-- This file is part of muforth: https://muforth.dev/

-- Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)

-- Code to convert multi-line comments to a new style.

-- We could use the contents of leader as a flag whether we are in a
-- comment or not, but for clarity I am using a separate variable,
-- in_comment.
function convert()
    local in_comment = false
    local leader, leader_match, before_lparen, after_lparen
    for l in io.lines() do          -- read stdin line by line
        if not in_comment then
            before_lparen, after_lparen = l:match "^(%s*)%((%s+)"
            if before_lparen then
                -- We found a left paren, but this could be a one-line
                -- comment, so let's look for a right paren before doing
                -- anything else.
                if not l:match "%)" then
                    -- No right paren; this must be a multi-line comment.
                    in_comment = true
                    leader       =         before_lparen .. "|" .. after_lparen
                    leader_match = "^(" .. before_lparen .. " " .. after_lparen .. ")"
                end
            end
        else
            -- in comment; modify line
            if l:match "^%s*$" then
                -- Empty line or only whitespace; replace with indent and
                -- pipe.
                l = before_lparen .. "|"
            else
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
            end
            if l:match "%)" then in_comment = false end
        end
        -- Whether or not the line was changed, write it to stdout.
        io.write(l .. "\n")
    end
end

convert()
