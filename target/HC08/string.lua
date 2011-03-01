-- String handling utilities

-- returns an iterator that returns chunks of the string, separated by sep
string.split = function (s, sep)
    local start = 1
    return function()
        if not start then return nil end
        local next, next_end = s:find(sep, start, false)
        if next then
            local first, last = start, next - 1
            start = next_end + 1    -- skip sep
            return s:sub(first, last)
        end
        -- end of string, or last chunk
        start, next = nil, start
        return next and s:sub(next, -1)
    end
end

string.lines = function(s) return s:split "\n" end
string.paras = function(s) return s:split "\n\n+" end   -- two or more newlines!
