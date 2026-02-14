#pragma once
#include <string>
#include <codecvt>

namespace Structs::Mono {
    typedef struct _String
    {
        void* klass;
        void* monitor;
        int length;
        char16_t chars[1];

        int GetLength()
        {
            return length;
        }

        std::string GetString()
        {
            std::u16string u16 = std::u16string(chars);
            std::string u8_conv = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
            return u8_conv;
        }
    }String;
}