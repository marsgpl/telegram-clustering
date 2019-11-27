#include "lua_text.h"

LUAMOD_API int luaopen_text(lua_State *L) {
    luaL_newlib(L, __index);
    return 1;
}

static inline int is_alpha(const char *str, size_t len) {
    for (int i = 0; i < len; i++) {
        if (!isalpha(str[i])) {
            return 0;
        }
    }

    return 1;
}

//                    |            | url_len
// schema://user:pass@host.zone:port/path?query#hash
// |                  |        |
// url                start    pos
static int lua_text_parse_url(lua_State *L) {
    size_t url_len;
    const char *url = luaL_checklstring(L, 1, &url_len);

    char *pos, *posx;
    size_t len;
    int start = 0;
    char port[6];

    lua_newtable(L);

    pos = strchr(url, '#');

    if (pos != NULL) {
        len = url_len - (pos - url); // including #
        url_len -= len;

        lua_pushlstring(L, pos + 1, len - 1);
        lua_setfield(L, -2, "hash");
    }

    pos = strchr(url, '?');

    if (pos != NULL && pos - url < url_len) {
        len = url_len - (pos - url); // including ?
        url_len -= len;

        lua_pushlstring(L, pos + 1, len - 1);
        lua_setfield(L, -2, "query");
    }

    pos = strstr(url, "//");

    if (pos == url) {
        // auto-scheme url: //domain.com
        start += 2;
        url_len -= 2;
    } else {
        pos = strstr(url, "://");

        if (pos != NULL && pos - url < url_len) {
            len = pos - url;

            if (is_alpha(url, len)) {
                start += len + 3;
                url_len -= len + 3;

                lua_pushlstring(L, url, len);
                lua_setfield(L, -2, "schema");
            }
        }
    }

    pos = strchr(url + start, '/');

    if (pos != NULL && (pos - url - start) < url_len) {
        len = url_len - (pos - url - start);
        url_len -= len;

        lua_pushlstring(L, pos, len);
        lua_setfield(L, -2, "path");
    }

    pos = strchr(url + start, '@');

    if (pos != NULL && (pos - url - start) < url_len) {
        len = pos - url - start; // user:pass
        posx = strchr(url + start, ':');

        if (posx != NULL && posx < pos) {
            lua_pushlstring(L, url + start, posx - url - start);
            lua_setfield(L, -2, "user");

            lua_pushlstring(L, posx + 1, pos - posx - 1);
            lua_setfield(L, -2, "pass");
        } else {
            // no : sep for user/pass
            // treat as long username
            lua_pushlstring(L, url + start, len);
            lua_setfield(L, -2, "user");
        }

        start += len + 1; // + @
        url_len -= len + 1; // + @
    }

    pos = strchr(url + start, ':');

    if (pos != NULL && (pos - url - start) < url_len) {
        len = pos - url - start;

        lua_pushlstring(L, url + start, len);
        lua_setfield(L, -2, "host");

        len = url_len - len - 1; // port len

        if (len > 5) { // do not push invalid port
            // lua_pushlstring(L, pos + 1, len);
            // lua_setfield(L, -2, "port");
        } else {
            memcpy(port, pos + 1, len);

            lua_pushinteger(L, atoi(port));
            lua_setfield(L, -2, "port");
        }

        url_len -= len + 1;
    } else {
        lua_pushlstring(L, url + start, url_len);
        lua_setfield(L, -2, "host");
    }

    posx = NULL;

    while (1) {
        pos = strchr(url + start, '.');
        if (pos == NULL) break;
        len = pos - url - start;
        if (len >= url_len) break;
        start += len + 1;
        url_len -= len + 1;
        posx = pos;
    }

    if (posx != NULL) {
        lua_pushlstring(L, url + start, url_len);
        lua_setfield(L, -2, "zone");
    }

    return 1;
}

// del all <[^>]+>
// collapse \s{2,} -> ' '
// trim
static int lua_text_strip_tags(lua_State *L) {
    const char *string = luaL_checkstring(L, 1);
    char *dst = malloc(strlen(string) + 1);
    int index = 0;
    int opened = 0;

    if (dst == NULL) {
        lua_fail(L, "malloc failed for lua_text_strip_tags", 0);
    }

    for ( ; *string; string++) {
        if ((char)*string == '<') {
            opened = 1;
        } else if ((char)*string == '>') {
            opened = 0;

            dst[index++] = ' ';
        } else if (!opened) {
            if (isspace(*string)) {
                if (index > 0) {
                    dst[index++] = ' ';
                }
            } else {
                dst[index++] = *string;
            }
        }
    }

    dst[index] = 0;

    lua_pushstring(L, dst);

    free(dst);

    return 1;
}

// https://www.w3.org/International/questions/qa-forms-utf-8
static inline int utf8_char_len(const unsigned char *s) {
    if (s[0] == 0) { // end of string
        return 0;
    } else if ( // [\x09\x0A\x0D\x20-\x7E] # ASCII
        (s[0] == 0x09) ||
        (s[0] == 0x0A) ||
        (s[0] == 0x0D) ||
        (0x20 <= s[0] && s[0] <= 0x7E)
    ) {
        return 1;
    } else if ( // [\xC2-\xDF][\x80-\xBF] # non-overlong 2-byte
        (0xC2 <= s[0] && s[0] <= 0xDF) &&
        (0x80 <= s[1] && s[1] <= 0xBF)
    ) {
        return 2;
    } else if ( // \xE0[\xA0-\xBF][\x80-\xBF] # excluding overlongs
        (s[0] == 0xE0) &&
        (0xA0 <= s[1] && s[1] <= 0xBF) &&
        (0x80 <= s[2] && s[2] <= 0xBF)
    ) {
        return 3;
    } else if ( // [\xE1-\xEC\xEE\xEF][\x80-\xBF]{2} # straight 3-byte
        ((0xE1 <= s[0] && s[0] <= 0xEC) || s[0] == 0xEE || s[0] == 0xEF) &&
        ((0x80 <= s[1] && s[1] <= 0xBF)) &&
        ((0x80 <= s[2] && s[2] <= 0xBF))
    ) {
        return 3;
    } else if ( // \xED[\x80-\x9F][\x80-\xBF] # excluding surrogates
        (s[0] == 0xED) &&
        (0x80 <= s[1] && s[1] <= 0x9F) &&
        (0x80 <= s[2] && s[2] <= 0xBF)
    ) {
        return 3;
    } else if ( // \xF0[\x90-\xBF][\x80-\xBF]{2} # planes 1-3
        (s[0] == 0xF0) &&
        (0x90 <= s[1] && s[1] <= 0xBF) &&
        (0x80 <= s[2] && s[2] <= 0xBF) &&
        (0x80 <= s[3] && s[3] <= 0xBF)
    ) {
        return 4;
    } else if ( // [\xF1-\xF3][\x80-\xBF]{3} # planes 4-15
        (0xF1 <= s[0] && s[0] <= 0xF3) &&
        (0x80 <= s[1] && s[1] <= 0xBF) &&
        (0x80 <= s[2] && s[2] <= 0xBF) &&
        (0x80 <= s[3] && s[3] <= 0xBF)
    ) {
        return 4;
    } else if ( // \xF4[\x80-\x8F][\x80-\xBF]{2} # plane 16
        (s[0] == 0xF4) &&
        (0x80 <= s[1] && s[1] <= 0x8F) &&
        (0x80 <= s[2] && s[2] <= 0xBF) &&
        (0x80 <= s[3] && s[3] <= 0xBF)
    ) {
        return 4;
    } else { // invalid seq
        return -1;
    }
}

// arg#1 - string
// arg#2 - alphabet - just a string containing all desired letters/codepoints
// if arg#3 true - all non-alphabet chars will be removed from string
// if arg#3 false - (default) - all alphabet chars will be removed from string
static int lua_text_strip_chars(lua_State *L) {
    const char *string = luaL_checkstring(L, 1);
    const char *alphabet = luaL_checkstring(L, 2);
    int keep = lua_toboolean(L, 3); // default: 0
    char *dst = malloc(strlen(string) + 1); // worst case: all symbols copied
    int index = 0;
    int len;
    int found;
    char symbol[5];

    if (dst == NULL) {
        lua_fail(L, "malloc failed for lua_text_strip_chars", 0);
    }

    while (*string) {
        len = utf8_char_len((unsigned char *)string);

        if (len == 0) { // end of string
            break;
        } else if (len == -1) { // invalid utf8
            len = 1;
            while (len--) dst[index++] = *(string++);
        } else {
            if (len == 1) {
                found = (strchr(alphabet, *string) != NULL);
            } else {
                memcpy(symbol, string, len);
                symbol[len] = 0;
                found = (strstr(alphabet, symbol) != NULL);
            }

            if ((found && keep) || (!found && !keep)) {
                while (len--) dst[index++] = *(string++);
            } else {
                string += len;
            }
        }
    }

    dst[index] = 0;

    lua_pushstring(L, dst);

    free(dst);

    return 1;
}

static int lua_text_collapse_whitespace(lua_State *L) {
    const char *string = luaL_checkstring(L, 1);
    char *dst = malloc(strlen(string) + 1); // worst case: all symbols copied
    int index = 0;
    int last_is_space = 0;

    if (dst == NULL) {
        lua_fail(L, "malloc failed for lua_text_collapse_whitespace", 0);
    }

    for ( ; *string; string++) {
        if (isspace(*string)) {
            if (index > 0 && !last_is_space) {
                dst[index++] = ' ';
                last_is_space = 1;
            }
        } else {
            dst[index++] = *string;
            last_is_space = 0;
        }
    }

    while (isspace(dst[index - 1])) index--;

    dst[index] = 0;

    lua_pushstring(L, dst);

    free(dst);

    return 1;
}

static int lua_text_normalize(lua_State *L) {
    const char *string = luaL_checkstring(L, 1);

    // normalize(collapse) + lowercase
    utf8proc_uint8_t *result = utf8proc_NFKC_Casefold((const utf8proc_uint8_t *)string);

    lua_pushstring(L, (char *)result);

    free(result);

    return 1;
}
