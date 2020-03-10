#pragma once
#include <cstddef>
#include <cinttypes>
#include <vector>
#include <optional>
#include "json.hpp"

using json = nlohmann::json;
extern void ReadMemory(uintptr_t src, void* dst, size_t size) noexcept;
extern uintptr_t GetBase() noexcept;
extern size_t GetSize() noexcept;
extern int InvokeContainerI(uintptr_t fnptr) noexcept;
extern int InvokeMapI(uintptr_t fnptr) noexcept;

namespace Mem {
    // Forward definitions
    template<typename T>
    struct Ptr;

    template<typename T>
    struct PtrAsHash;

    template<typename T>
    struct StdVector;

    struct CString;

    struct StdString;

    template<typename T>
    struct StdSharedPtr;

    // Struct definitions
    template<typename T>
    struct Ptr {
        uintptr_t ptr;
        inline T operator*() const noexcept {
            T data{};
            ReadMemory(ptr, reinterpret_cast<void*>(&data), sizeof(data));
            return data;
        }
    };

    template<typename T>
    struct PtrAsHash : Ptr<T> {};

    template<typename T>
    struct StdVector {
        uintptr_t beg;
        uintptr_t end;
        uintptr_t cap;
        inline std::vector<T> operator*() const noexcept {
            std::vector<T> data = {};
            if(beg != end) {
                data.resize((end - beg) / sizeof(T));
                ReadMemory(beg, data.data(), data.size() * sizeof(T));
            }
            return data;
        }
    };

    struct StdString {
        static inline constexpr size_t SSO_BUFFER = 16;
        union {
            uintptr_t ptr;
            char buffer[SSO_BUFFER];
        } data;
        size_t size;
        size_t reserved;

        inline std::string operator*() const noexcept {
            if(reserved >= SSO_BUFFER) {
                std::string result(size, '\0');
                ReadMemory(data.ptr, result.data(), size);
                return result;
            } else {
                return std::string(data.buffer, size);
            }
        }
    };

    struct CString {
        uintptr_t ptr;
        std::string operator *() const noexcept {
            std::string result{};
            if(ptr) {
                for(uintptr_t i = 0; ; i++) {
                    char c;
                    ReadMemory(ptr + i, &c, 1);
                    if(!c) {
                        break;
                    }
                    result.push_back(c);
                }
            }
            return result;
        }
    };

    template<typename T>
    struct StdSharedPtr : Ptr<T> {
        intptr_t ctrl;
    };

    template<typename T>
    inline void to_json(json& j, Ptr<T> const& ptr) {
        if (ptr.ptr == 0) {
            j = nullptr;
        }
        else {
            j = *ptr;
        }
    }

    template<typename T>
    inline void to_json(json& j, PtrAsHash<T> const& ptr) {
        if (ptr.ptr == 0) {
            j = 0u;
        }
        else {
            j = (*ptr).hash;
        }
    }

    template<typename T>
    inline void to_json(json& j, StdVector<T> const& v) {
        j = *v;
    }

    inline void to_json(json& j, StdString const& v) {
        j = *v;
    }

    inline void to_json(json& j, CString const& v) {
        j = *v;
    }

    inline std::vector<uint8_t> Dump() noexcept {
        auto const size = GetSize();
        auto const base = GetBase();
        std::vector<uint8_t> data{};
        data.resize(size);
        for (size_t i = 0; i < data.size(); i += 0x1000) {
            ReadMemory(base + i, data.data() + i, 0x1000);
        }
        return data;
    }
}
