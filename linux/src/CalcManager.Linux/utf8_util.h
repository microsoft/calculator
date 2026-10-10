// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Shared UTF-8 <-> std::wstring helpers for the Linux C ABI shims.
// On Linux wchar_t is UTF-32, so conversion is direct scalar-value coding
// (no deprecated <codecvt>, no iconv link dependency).
#pragma once

#include <cstring>
#include <string>

namespace calc_linux
{
    inline void AppendUtf8(std::string& out, unsigned int cp)
    {
        if (cp < 0x80)
        {
            out.push_back(static_cast<char>(cp));
        }
        else if (cp < 0x800)
        {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp < 0x10000)
        {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else
        {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }

    inline std::string ToUtf8(const std::wstring& wide)
    {
        std::string out;
        out.reserve(wide.size());
        for (wchar_t wc : wide)
        {
            unsigned int cp = static_cast<unsigned int>(wc);
            if (cp >= 0xD800 && cp <= 0xDFFF)
            {
                cp = 0xFFFD; // lone surrogate: never valid from the engine, stay safe
            }
            AppendUtf8(out, cp);
        }
        return out;
    }

    inline std::wstring ToWide(const std::string& utf8)
    {
        std::wstring out;
        out.reserve(utf8.size());
        size_t i = 0;
        while (i < utf8.size())
        {
            unsigned char c = static_cast<unsigned char>(utf8[i]);
            unsigned int cp = 0xFFFD;
            size_t len = 1;
            if (c < 0x80)
            {
                cp = c;
                len = 1;
            }
            else if ((c >> 5) == 0x6 && i + 1 < utf8.size())
            {
                cp = ((c & 0x1F) << 6) | (utf8[i + 1] & 0x3F);
                len = 2;
            }
            else if ((c >> 4) == 0xE && i + 2 < utf8.size())
            {
                cp = ((c & 0x0F) << 12) | ((utf8[i + 1] & 0x3F) << 6) | (utf8[i + 2] & 0x3F);
                len = 3;
            }
            else if ((c >> 3) == 0x1E && i + 3 < utf8.size())
            {
                cp = ((c & 0x07) << 18) | ((utf8[i + 1] & 0x3F) << 12) | ((utf8[i + 2] & 0x3F) << 6) | (utf8[i + 3] & 0x3F);
                len = 4;
            }
            out.push_back(static_cast<wchar_t>(cp));
            i += len;
        }
        return out;
    }

    // Copies UTF-8 text into a caller buffer (NUL-terminated, truncated on a
    // code-point boundary). Returns the bytes needed INCLUDING the NUL.
    // With buffer==NULL or capacity==0 nothing is written.
    inline int CopyToBuffer(const std::string& text, char* buffer, int capacity)
    {
        int needed = static_cast<int>(text.size()) + 1;
        if (buffer != nullptr && capacity > 0)
        {
            size_t copy = text.size();
            if (copy > static_cast<size_t>(capacity) - 1)
            {
                copy = static_cast<size_t>(capacity) - 1;
                while (copy > 0 && (static_cast<unsigned char>(text[copy]) >> 6) == 0x2)
                {
                    --copy;
                }
            }
            std::memcpy(buffer, text.data(), copy);
            buffer[copy] = '\0';
        }
        return needed;
    }
} // namespace calc_linux
