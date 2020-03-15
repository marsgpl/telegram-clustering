local text = require "text"

local strip_urls = require "detectors/helpers/strip_urls"
local strip_emails = require "detectors/helpers/strip_emails"

local chars = {
    alpha_en = require "chars/alphabet/en",
    alpha_ru = require "chars/alphabet/ru",
    currency = require "chars/currency",
    digits = require "chars/digits",
    punct = require "chars/punctuation",
    space = require "chars/space",
    ua_vs_ru = require "chars/ua_vs_ru",
}

local grams = {
    en2 = require "en2grams",
    ru2 = require "ru2grams",
}

chars.currency_digits_punct = chars.currency .. chars.digits .. chars.punct

-- local INTERACTIVE = true
-- local INTERACTIVE_INDEX = 0
-- local INTERACTIVE_LAST = 42

return function(info)
    if info.language then return end

    -- if INTERACTIVE then
    --     INTERACTIVE_INDEX = INTERACTIVE_INDEX + 1
    --     if INTERACTIVE_INDEX < INTERACTIVE_LAST then return end
    -- end

    local target = info.file.content

    target = text.strip_tags(target)
    target = text.normalize(target)
    target = strip_urls(target)
    target = strip_emails(target)
    target = text.strip_chars(target, chars.currency_digits_punct)

    local letters = text.strip_whitespace(target)
    local only_ru = text.strip_chars(letters, chars.alpha_ru, true)
    local only_en = text.strip_chars(letters, chars.alpha_en, true)
    local is_cyr = #only_ru > (#letters) / 2
    local is_latin = #only_en > (#letters) / 2

    if is_cyr or is_latin then
        local words = text.collapse_whitespace(target, true)
        local words_grams, word_grams_count = text.split_2grams(words, 200)
        local grams_found = text.find_2grams(is_latin and grams.en2 or grams.ru2, words_grams, 20)
        local score = grams_found / word_grams_count
        -- local ua_entries = is_cyr and text.count_chars(words, chars.ua_vs_ru) or 0

        -- if is_cyr and INTERACTIVE then
        --     io.write("words: ", trace.str(words))
        --     io.write("charset: ", trace.str(is_cyr and "cyr" or "lat"))
        --     io.write("word_grams_count: ", trace.str(word_grams_count))
        --     io.write("grams_found: ", trace.str(grams_found))
        --     io.write("score: ", trace.str(score))
        --     io.write("ua_entries: ", trace.str(ua_entries))

        --     INTERACTIVE_LAST = INTERACTIVE_INDEX
        --     print("\nINTERACTIVE_LAST = " .. INTERACTIVE_LAST)
        --     io.read()
        -- end

        if grams_found >= 10 or score >= .05 then
            info.language = is_latin and "en" or "ru"
        end
    end
end
