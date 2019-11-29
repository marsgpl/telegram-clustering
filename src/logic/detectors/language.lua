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
}

local grams = {
    en2 = require "en2grams",
    ru2 = require "ru2grams",
}

chars.currency_digits_punct = chars.currency .. chars.digits .. chars.punct

local INTERACTIVE = true
local INTERACTIVE_INDEX = 0
local INTERACTIVE_LAST = 0

return function(info)
    if info.language then return end

    if INTERACTIVE then
        INTERACTIVE_INDEX = INTERACTIVE_INDEX + 1
        if INTERACTIVE_INDEX < INTERACTIVE_LAST then return end
    end

    local target = info.file.content

    target = text.strip_tags(target)
    target = text.normalize(target)
    target = strip_urls(target)
    target = strip_emails(target)
    target = text.strip_chars(target, chars.currency_digits_punct)

    local only_letters = text.strip_whitespace(target)
    local only_ru = text.strip_chars(only_letters, chars.alpha_ru, true)
    local only_en = text.strip_chars(only_letters, chars.alpha_en, true)
    local is_cyr = #only_ru > #only_letters/2
    local is_latin = #only_en > #only_letters/2

    if is_cyr or is_latin then
        local only_words = text.collapse_whitespace(target, true)
        local words_count = #only_words > 0 and #(text.strip_chars(only_words, chars.space, true)) + 1 or 0
        -- local avg_word_len = #only_words / words_count
        local need_grams = words_count > 10 and math.ceil(words_count / 5) or 1
        local grams_found = text.find_grams(only_words, is_latin and grams.en2 or grams.ru2, need_grams)
        local grams_score = grams_found / need_grams

        -- if is_cyr then
        --     if grams_score <= .4 then
        --         io.write("info: ", trace.str(info))
        --         io.write("only_words: ", trace.str(only_words))

        --     end
        -- end

        if grams_score > .4 then
            info.language = is_latin and "en" or "ru"
        end
    end

    if INTERACTIVE then
        INTERACTIVE_LAST = INTERACTIVE_INDEX
        print("\nINTERACTIVE_LAST = " .. INTERACTIVE_LAST)
        io.read()
    end
end
