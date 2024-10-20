-- This file is part of muforth: https://muforth.dev/

-- Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)

-- Code to convert multi-line comments to a new style.

-- conv_dashdash and conv_brackets will only work after all paren-delimited
-- block comments have been converted to vertical bars.

function conv_dashdash()
    return function(l)
        local before, after = l:match "^(%s*)|(%s.-)$"
        if before then
            -- This is a line from a block comment; only whitespace
            -- precedes the "|". Rewrite any "--" as "-". It's probably
            -- meant as an endash.

            -- If a "--" is the first thing after the "|", or if at least
            -- two spaces precede the "--", then it is probably meant to
            -- comment out the rest of the line, so let's replace it with a
            -- "|".
            after = after:gsub("^(%s+)%-%-(%s)", "%1|%2", 1)
                         :gsub("(%s%s)%-%-(%s)", "%1|%2", 1)

            -- Let's assume that any other appearances of "--" are acting
            -- as an endash, and replace them with a "-".
            after = after:gsub("(%s)%-%-(%s)", "%1-%2")
                         :gsub("(%s)%-%-$", "%1-")  -- match at EOL too

            return before .. "|" .. after
        end

        local before, after = l:match "^(.-)|(.-)$"
        if before then
            -- Non-blank text precedes the "|".
            -- XXX what do we want to do? Anything?
        else
            -- Line with no vertical bar.
            -- If "--" inside parens, it's probably a stack comment;
            -- rewrite "--" as "-".
            -- Otherwise, rewrite all "--" as "|", being careful to also
            -- match "--" at the start of the line.
            l = l:gsub("(%([^)]-)%s%-%-%s([^(]-%))", "%1 - %2")
                 :gsub("(%s)%-%-(%s)", "%1|%2")
                 :gsub("^%-%-%s", "| ")
        end
        return l
    end
end

-- This function only pays attention to lines that are wholly commented out
-- by a "|" at the beginning of the line.
function conv_brackets()
    return function(l)
        if l:match "^%s*| " then
            -- Because I sometimes used square brackets [ ] in place of parens
            -- (because otherwise the closing paren would end the comment), let's
            -- rewrite square-bracket-enclosed text as paren-enclosed!

            -- Since the opening and closing bracket may be on different
            -- lines, we have to match them separately.

            l = l:gsub("([%s%p])%[(%S)", "%1(%2")   -- space or punct before [ non-space after
                 :gsub("^%[(%S)", "(%1")            -- [ at start of line, non-space after
                 :gsub("(%S)%]([%s%p])", "%1)%2")   -- non-space before ] space or punct after
                 :gsub("(%S)%]$", "%1)")            -- non-space before ] at end of line
        end
        return l
    end
end

-- If we find something like ") (", elide the (. It is only there to
-- "restart" the comment, which is no longer necessary.
function conv_parens()
    return function(l)
        if l:match "%s*| " then
            -- in block comment
            l = l:gsub("%)(%s+)%(%s", ")%1")
        end
        return l
    end
end

-- We could use the contents of leader as a flag whether we are in a
-- comment or not, but for clarity I am using a separate variable,
-- in_comment.
function conv_block()
    local comment_type = "none"     -- outside a comment
    local leader, leader_match, before_lparen, after_lparen
    local token

    return function(l)
        if comment_type == "none" then
            -- Check for "comment" followed by a token. This begins a
            -- token-style comment.
            c, token = l:match "^%s*(%S+)%s+(%S+)"
            if c and c == "comment" then
                comment_type = "token"
                before_lparen = ""      -- treat as if "(" in leftmost column
                l = "| " .. l           -- comment out line in which "comment" appears!
            else
                -- Maybe we have "comment" by itself on a line and the
                -- token on the following line?
                c = l:match "^%s*(%S+)%s*$"
                if c and c == "comment" then
                    -- We have to get the token from the following
                    -- line!
                    comment_type = "expecting a token"
                    l = "| " .. l       -- comment out line in which "comment" appears!
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

                            -- Rewrite line. Change opening lparen to vertical bar.
                            l = l:gsub("%(", "|", 1)
                        end
                    end
                end
            end
        elseif comment_type == "expecting a token" then
            comment_type = "token"
            before_lparen = ""      -- treat as if "(" in leftmost column

            token = l:match "^%s*(%S+)"
            -- Comment out token line!
            l = "| " .. token
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
                l, changes = l:gsub(leader_match, leader, 1)
                if changes == 0 then
                    -- l was not changed. Most likely the line doesn't have
                    -- enough white space before the text for leader_match
                    -- to match, or it was already converted.

                    -- Try matching bare |. If found, fix its indent.
                    if l:match "^%s*|%s*$" then
                        l = before_lparen .. "|"

                    -- Try matching bare ). Print a blank line and end the
                    -- block comment.
                    elseif l:match "^%s*%)%s*$" then
                        l = ""
                        comment_type = "none"
                    elseif not l:match("^" .. leader) then
                        -- If bare | or ) did *not* match *and* comment
                        -- line *not* already in new format, let's change
                        -- it. Match whatever leading white space there is
                        -- - including none at all! - and replace it with
                        -- the leader text.
                        l = l:gsub("^(%s*)", leader, 1)
                    end
                end
                -- Try to remove a ) at the end of the line, and if
                -- successful, end the comment.
                l, changes = l:gsub("%)$", "", 1)
                if changes == 1 then
                    -- We found one. End the comment.
                    comment_type = "none"

                    -- There are three cases in which we want to put the
                    -- ")" back:
                    -- (1) if the line now ends with whitespace;
                    -- (2) if the line now contains a mangled smiley;
                    -- (3) if the line now contains an unmatched left
                    --     parenthesis
                    l = l:gsub("(%s)$", "%1)", 1)       -- replace ) after whitespace
                         :gsub("([:;]%-)$", "%1)", 1)   -- replace ) after mangled smiley
                         :gsub("(%([^)]-)$", "%1)", 1)  -- replace ) if unmatched (
                end

            elseif comment_type == "token" then
                for w in l:gmatch "%S+" do      -- tokenize line
                    if w == token then
                        -- found token in the text of the line
                        comment_type = "none"
                    end
                end
                -- If not already done, prefix text with vertical bar.
                if not l:match "^|" then
                    l = "| " .. l
                end
            else
                return l, string.format("Comment type is %s", comment_type)
            end
        end
        return l
    end
end

-- Read file line-by-line and call transform function.
function process(fn)
    local fail

    for l in io.lines() do          -- read stdin line by line
        l, fail = fn(l)
        if fail then
            print(fail)
            os.exit(1)
        else
            print(l)
        end
    end
end

lookup = {
    dashdash = conv_dashdash,
    brackets = conv_brackets,
    parens = conv_parens,
    block = conv_block,
}

-- First argument should be dashdash, brackets, or block
process(lookup[arg[1]]())
