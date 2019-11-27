local text = require "text"
local word_count = require "detectors/helpers/word_count"
local strip_urls = require "detectors/helpers/strip_urls"
local strip_emails = require "detectors/helpers/strip_emails"
local strip_html_urls = require "detectors/helpers/strip_html_urls"

local alpha_en = require "data/alphabet/en"
local alpha_ru = require "data/alphabet/ru"
local digits = require "data/digits"
local punct = require "data/punctuation"
local space = require "data/space"

local digits_punct = digits .. punct
local en_space = alpha_en .. space
local ru_space = alpha_ru .. space

local norm = text.normalize
local strip = text.strip_chars
local trim = text.collapse_whitespace

local strip_html = function(str)
    return text.strip_tags(strip_html_urls(str))
end

local prep_text = function(str)
    return trim(strip(strip_emails(strip_urls(norm(str))), digits_punct))
end

local wc = function(str)
    return {
        all = word_count(str),
        ru = word_count(trim(strip(str, ru_space, true))),
        en = word_count(trim(strip(str, en_space, true))),
    }
end

local index = 0
local last = 705

return function(info)
    if info.language then return end

index = index + 1
if index < last then return end

    local all = prep_text(strip_html(info.file.content))
    local rwc = wc(all)
    local rest = trim(strip(all, alpha_en .. alpha_ru .. space))

    if #rest > 0 then
        print(("\n"):rep(50))
        print(info.file.path)
        trace(all)
        print("all: "..rwc.all.."  en: "..rwc.en.."  ru: "..rwc.ru)
        trace(rest)
        last = index
        print("\nlast = " .. last)
        io.read()
    end

    info.language = info.domain_zone
end
