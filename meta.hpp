#pragma once
#include <string>
#include <string_view>
#include <type_traits>
#include "mem.hpp"
#include "patscanner.hpp"
#include "version.hpp"

namespace Meta {
    template<Patch V>
    struct Property;

    template<Patch V>
    struct Class;

    template<Patch V>
    struct ContainerI;

    template<Patch V>
    struct MapI;

    template<Patch V>
    using ClassList =
        std::conditional_t<(V < Patch::V11_6),
                Mem::Ptr<Mem::StdVector<Mem::Ptr<Class<V>>>>,
                Mem::Ptr<Mem::RiotVector<Mem::Ptr<Class<V>>>>
                >;

    template<size_t SIZE>
    using Vtable = std::array<Mem::Ptr<std::array<uint8_t, 8>>, SIZE>;

    enum class PropertyType : uint8_t {};

    template<Patch V>
    struct ContainerI {
        Mem::Ptr<Vtable<10>> vtable;
        PropertyType type;
        size_t elemSize;

        int32_t fixedSize() const noexcept {
            auto const vdata = *vtable;
            auto const fnptr = vdata[V < Patch::V5_8 ? 7 : 8];
            auto const result = InvokeContainerI(fnptr.ptr);
            auto const data = *fnptr;
            if (data[0] == 0xB8 && data[5] == 0xC3) {
                // count of elements in fixed sized array
                return static_cast<int32_t>(data[1])
                    | (static_cast<int32_t>(data[2]) << 8)
                    | (static_cast<int32_t>(data[3]) << 16)
                    | (static_cast<int32_t>(data[4]) << 24);
            } else if (data[0] == 0x83 && data[1] == 0xC8 && data[2] == 0xFF && data[3] == 0xC3) {
                // Dynamic sized std::vector
                return 0;
            }

            if (result == -1) {
                return 0;
            } else if(result > -1) {
                return result;
            }

            return -1;
        }
    };

    template<Patch V>
    struct MapI {
        Mem::Ptr<Vtable<4>> vtable;
        PropertyType key;
        PropertyType value;

        // FIXME: doesn't work for Patchs prior to: V6.6.137.4261 (project 0.0.1.65)
        // The compiler stores frame pointers before that Patch 
        int32_t storage() const noexcept {
            auto const vdata = *vtable;
            auto const fnptr = vdata[1];
            auto const result = InvokeMapI(fnptr.ptr);
            auto const data = *fnptr;
            if (true 
                && data[0] == 0x8Bu 
                && (data[1] & 0xF0u) == 0x40u 
                && data[2] == 0x24u 
                && data[3] == 0x04u 
                && data[4] == 0x8Bu 
                && (data[5] & 0xF0u) == 0x40u) {
                if (data[6] == 0x04u && data[7] == 0xC2u) {
                    // std::map
                    return 0;
                } else if (data[6] == 0x08u && data[7] == 0xC2u) {
                    // std::unordered_map
                    return 1;
                } else if (data[6] == 0x04u && data[7] == 0x2Bu) {
                    // RiotVectorMap
                    return 2;
                }
            }

            if (result == 0x78000000) {
                return 0;
            } else if (result == 1) {
                return 1;
            } else if (result > 0x7000) {
                return 2;
            }

            return -1;
        }
    };

    template<>
    struct Property<Patch::V0> {
        static inline constexpr Patch V = Patch::V0;
        Mem::PtrAsHash<Class<V>> otherClass;
        uint32_t hash;
        int32_t offset;
        static inline constexpr uint8_t bitmask = {};
        PropertyType type;
        Mem::Ptr<ContainerI<V>> containerI;
        static inline constexpr Mem::Ptr<MapI<V>> mapI = {};
    };

    template<>
    struct Property<Patch::V5_8> {
        static inline constexpr Patch V = Patch::V5_8;
        Mem::PtrAsHash<Class<V>> otherClass;
        uint32_t hash;
        int32_t offset;
        static inline constexpr uint8_t bitmask = {};
        PropertyType type;
        Mem::Ptr<ContainerI<V>> containerI;
        Mem::Ptr<MapI<V>> mapI = {};
    };

    template<>
    struct Property<Patch::V5_21> {
        static inline constexpr Patch V = Patch::V5_21;
        Mem::PtrAsHash<Class<V>> otherClass;
        uint32_t hash;
        int32_t offset;
        static inline constexpr uint8_t bitmask = {};
        PropertyType type;
        Mem::Ptr<ContainerI<V>> containerI;
        Mem::Ptr<MapI<V>> mapI = {};
    };

    template<>
    struct Property<Patch::V6_20> {
        static inline constexpr Patch V = Patch::V6_20;
        Mem::PtrAsHash<Class<V>> otherClass;
        uint32_t hash;
        int32_t offset;
        static inline constexpr uint8_t bitmask = {};
        PropertyType type;
        Mem::Ptr<ContainerI<V>> containerI;
        Mem::Ptr<MapI<V>> mapI = {};
    };

    template<>
    struct Property<Patch::V7_15> {
        static inline constexpr Patch V = Patch::V7_15;
        Mem::PtrAsHash<Class<V>> otherClass;
        uint32_t hash;
        int32_t offset;
        uint8_t bitmask = {};
        PropertyType type;
        Mem::Ptr<ContainerI<V>> containerI;
        Mem::Ptr<MapI<V>> mapI = {};
    };

    template<>
    struct Property<Patch::V12_10> {
        static inline constexpr Patch V = Patch::V12_10;
        Mem::PtrAsHash<Class<V>> otherClass;
        uint32_t hash;
        int32_t offset;
        uint8_t bitmask = {};
        PropertyType type;
        Mem::Ptr<ContainerI<V>> containerI;
        Mem::Ptr<MapI<V>> mapI = {};
    };
    
    template<>
    struct Class<Patch::V0> {
        static inline constexpr Patch V = Patch::V0;
        static inline constexpr uintptr_t upcastSecondary = {};
        uint32_t hash;
        uintptr_t constructor;
        uintptr_t destructor;
        uintptr_t inplaceconstructor;
        uintptr_t inplacedestructor;
        uintptr_t initfunction;
        Mem::PtrAsHash<Class<V>> parentClass;
        size_t classSize;
        size_t alignment;
        bool isPropertyBase;
        bool isInterface;
        static inline constexpr bool isValue = {};
        static inline constexpr bool isSecondaryBase = {};
        static inline constexpr bool isUnk5 = {};
        Mem::StdVector<Property<V>> properties;
        static inline constexpr Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryBases = {};
        static inline constexpr Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryChildren = {};
    };

    template<>
    struct Class<Patch::V5_8> {
        static inline constexpr Patch V = Patch::V5_8;
        static inline constexpr uintptr_t upcastSecondary = {};
        uint32_t hash;
        uintptr_t constructor;
        uintptr_t destructor;
        uintptr_t inplaceconstructor;
        uintptr_t inplacedestructor;
        uintptr_t initfunction;
        Mem::PtrAsHash<Class<V>> parentClass;
        size_t classSize;
        size_t alignment;
        bool isPropertyBase;
        bool isInterface;
        static inline constexpr bool isValue = {};
        static inline constexpr bool isSecondaryBase = {};
        static inline constexpr bool isUnk5 = {};
        Mem::StdVector<Property<V>> properties;
        static inline constexpr Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryBases = {};
        static inline constexpr Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryChildren = {};
    };

    template<>
    struct Class<Patch::V5_21> {
        static inline constexpr Patch V = Patch::V5_21;
        static inline constexpr uintptr_t upcastSecondary = {};
        uint32_t hash;
        uintptr_t constructor;
        uintptr_t destructor;
        uintptr_t inplaceconstructor;
        uintptr_t inplacedestructor;
        uintptr_t initfunction;
        Mem::PtrAsHash<Class<V>> parentClass;
        size_t classSize;
        size_t alignment;
        bool isPropertyBase;
        bool isInterface;
        bool isValue;
        static inline constexpr bool isSecondaryBase = {};
        static inline constexpr bool isUnk5 = {};
        Mem::StdVector<Property<V>> properties;
        static inline constexpr Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryBases = {};
        static inline constexpr Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryChildren = {};
    };

    template<>
    struct Class<Patch::V6_20> {
        static inline constexpr Patch V = Patch::V6_20;
        uintptr_t upcastSecondary = {};
        uint32_t hash;
        uintptr_t constructor;
        uintptr_t destructor;
        uintptr_t inplaceconstructor;
        uintptr_t inplacedestructor;
        uintptr_t initfunction;
        Mem::PtrAsHash<Class<V>> parentClass;
        size_t classSize;
        size_t alignment;
        bool isPropertyBase;
        bool isInterface;
        bool isValue;
        bool isSecondaryBase;
        static inline constexpr bool isUnk5 = {};
        Mem::StdVector<Property<V>> properties;
        Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryBases;
        Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryChildren;
    };

    template<>
    struct Class<Patch::V7_15> {
        static inline constexpr Patch V = Patch::V7_15;
        uintptr_t upcastSecondary = {};
        uint32_t hash;
        uintptr_t constructor;
        uintptr_t destructor;
        uintptr_t inplaceconstructor;
        uintptr_t inplacedestructor;
        uintptr_t initfunction;
        Mem::PtrAsHash<Class<V>> parentClass;
        size_t classSize;
        size_t alignment;
        bool isPropertyBase;
        bool isInterface;
        bool isValue;
        bool isSecondaryBase;
        static inline constexpr bool isUnk5 = {};
        Mem::StdVector<Property<V>> properties;
        Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryBases;
        Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryChildren;
    };

    template<>
    struct Class<Patch::V10_11> {
        static inline constexpr Patch V = Patch::V7_15;
        uintptr_t upcastSecondary = {};
        uint32_t hash;
        uintptr_t constructor;
        uintptr_t destructor;
        uintptr_t inplaceconstructor;
        uintptr_t inplacedestructor;
        uintptr_t initfunction;
        Mem::PtrAsHash<Class<V>> parentClass;
        size_t classSize;
        size_t alignment;
        bool isPropertyBase;
        bool isInterface;
        bool isValue;
        bool isSecondaryBase;
        bool isUnk5;
        Mem::StdVector<Property<V>> properties;
        Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryBases;
        Mem::StdVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryChildren;
    };

    template<>
    struct Class<Patch::V11_6> {
        static inline constexpr Patch V = Patch::V7_15;
        uintptr_t upcastSecondary = {};
        uint32_t hash;
        uintptr_t constructor;
        uintptr_t destructor;
        uintptr_t inplaceconstructor;
        uintptr_t inplacedestructor;
        uintptr_t initfunction;
        Mem::PtrAsHash<Class<V>> parentClass;
        size_t classSize;
        size_t alignment;
        bool isPropertyBase;
        bool isInterface;
        bool isValue;
        bool isSecondaryBase;
        bool isUnk5;
        Mem::RiotVector<Property<V>> properties;
        Mem::RiotVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryBases;
        Mem::RiotVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryChildren;
    };

    template<>
    struct Class<Patch::V12_10> {
        static inline constexpr Patch V = Patch::V12_10;
        uintptr_t upcastSecondary = {};
        uint32_t hash;
        uintptr_t constructor;
        uintptr_t destructor;
        uintptr_t inplaceconstructor;
        uintptr_t inplacedestructor;
        uintptr_t initfunction;
        Mem::PtrAsHash<Class<V>> parentClass;
        size_t classSize;
        size_t alignment;
        static inline constexpr bool isPropertyBase = false;
        static inline constexpr bool isInterface = false;
        bool isValue;
        bool isSecondaryBase;
        bool isUnk5;
        Mem::RiotVector<Property<V>> properties;
        Mem::RiotVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryBases;
        Mem::RiotVector<std::pair<Mem::PtrAsHash<Class<V>>, uint32_t>> secondaryChildren;
    };


    // json serializers

    template<Patch V>
    inline void to_json(json& j, ContainerI<V> const& v) {
        j = {
            { "vtable", UnBase(v.vtable.ptr) },
            { "type", v.type },
            { "elemSize", v.elemSize },
            { "fixedSize", v.fixedSize() }
        };
    }

    template<Patch V>
    inline void to_json(json& j, MapI<V> const& v) {
        j = {
            { "vtable", UnBase(v.vtable.ptr) },
            { "key", v.key },
            { "value", v.value },
            { "storage", v.storage() }
        };
    }
    
    template<Patch V>
    inline void to_json(json& j, Property<V> const& v) {
        j = {
            { "otherClass", v.otherClass },
            { "hash", v.hash },
            { "offset", v.offset },
            { "bitmask", v.bitmask },
            { "type", v.type },
            { "containerI", v.containerI },
            { "mapI", v.mapI },
        };
    }

    template<Patch V>
    inline void to_json(json& j, Class<V> const& v) {
        j = {
            { "upcastSecondary", UnBase(v.upcastSecondary) },
            { "hash", v.hash },
            { "constructor", UnBase(v.constructor) },
            { "destructor", UnBase(v.destructor) },
            { "inplaceconstructor", UnBase(v.inplaceconstructor) },
            { "inplacedestructor", UnBase(v.inplacedestructor) },
            { "initfunction", UnBase(v.initfunction) },
            { "parentClass", v.parentClass },
            { "classSize", v.classSize },
            { "alignment", v.alignment },
            { "isPropertyBase", v.isPropertyBase },
            { "isInterface", v.isInterface || !v.constructor },
            { "isValue", v.isValue },
            { "isSecondaryBase", v.isSecondaryBase },
            { "isUnk5", v.isUnk5 },
            { "properties", *v.properties },
            { "secondaryBases", *v.secondaryBases },
            { "secondaryChildren", *v.secondaryChildren },
        };
    }

    template<Patch V>
    inline json Dump(std::vector<uint8_t> const& data) {
        constexpr auto const pat1 = Pattern<
            0x83, 0xC8, 0x01,
            0xC7, 0x05, Any, Any, Any, Any, 0x00, 0x00, 0x00, 0x00,
            0x68, Any, Any, Any, Any,
            0xA3, Any, Any, Any, Any,
            0xC7, 0x05, Any, Any, Any, Any, 0x00, 0x00, 0x00, 0x00,
            0xC7, 0x05, Any, Any, Any, Any, 0x00, 0x00, 0x00, 0x00,
            0xC6, 0x05, Cap<Any>, Any, Any, Any, 0x00,
            0xE8
        >();
        constexpr auto const pat2 = Pattern<
            0x83, 0x3D, Any, Any, Any, Any, 0xFF,
            0x75, 0xE4,
            0x68, Any, Any, Any, Any,
            0xC7, 0x05, Any, Any, Any, Any, 0x00, 0x00, 0x00, 0x00,
            0xC7, 0x05, Any, Any, Any, Any, 0x00, 0x00, 0x00, 0x00,
            0xC7, 0x05, Any, Any, Any, Any, 0x00, 0x00, 0x00, 0x00,
            0xC6, 0x05, Cap<Any>, Any, Any, Any, 0x00,
            0xE8
        >();
        constexpr auto const pat3 = Pattern<
            0x83, 0xC8, 0x01,
            0xA3, Any, Any, Any, Any,
            0x33, 0xC0,
            0x68, Any, Any, Any, Any,
            0xA3, Any, Any, Any, Any,
            0xA3, Any, Any, Any, Any,
            0xA3, Any, Any, Any, Any,
            0xA2, Cap<Any>, Any, Any, Any,
            0xE8
        >();
        uintptr_t offset = {};
        if (auto const result = pat1(data.data(), data.size()); result[0]) {
            offset = *reinterpret_cast<uintptr_t const*>(result[1]) - 12;
        } else if (auto const result = pat2(data.data(), data.size()); result[0]) {
            offset = *reinterpret_cast<uintptr_t const*>(result[1]) - 12;
        } else if (auto const result = pat3(data.data(), data.size()); result[0]) {
            offset = *reinterpret_cast<uintptr_t const*>(result[1]) - 12;
        } else {
            return {};
        }
        auto meta = **ClassList<V>{ offset };
        return meta;
    }

    inline json Dump(std::vector<uint8_t> const& data, std::string_view version) {
        auto const versionNumber = Version::Parse(version);
        json meta;
        if (versionNumber < Patch::V5_8) {
            meta = Dump<Patch::V0>(data);
        } else if (versionNumber < Patch::V5_2) {
            meta = Dump<Patch::V5_8>(data);
        } else if (versionNumber < Patch::V6_20) {
            meta = Dump<Patch::V5_21>(data);
        } else if (versionNumber < Patch::V7_15) {
            meta = Dump<Patch::V6_20>(data);
        } else if (versionNumber < Patch::V10_11) {
            meta = Dump<Patch::V7_15>(data);
        } else if (versionNumber < Patch::V11_6) {
            meta = Dump<Patch::V10_11>(data);
        } else if (versionNumber < Patch::V12_10) {
            meta = Dump<Patch::V11_6>(data);
        } else {
            meta = Dump<Patch::V12_10>(data);
        }
        return {
            { "classes", meta },
            { "version", version },
        };
    }
}
