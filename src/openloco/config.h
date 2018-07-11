#pragma once

#include <cstdint>
#include <string>

namespace openloco::config
{
#pragma pack(push, 1)

    enum flags
    {
        show_height_as_units = (1 << 1),
    };

    enum measurement_format
    {
        imperial = 0,
        metric = 1,
    };

    struct keyboard_shortcut_t
    {
        uint8_t var_0;
        uint8_t var_1;
    };

    struct config_t
    {
        uint32_t flags;             // 0x50AEB4, 0x00
        uint16_t resolution_width;  // 0x50AEB8, 0x04
        uint16_t resolution_height; // 0x50AEBA, 0x06
        uint8_t pad_08[0x28 - 0x08];
        uint8_t measurement_format; // 0x50AEDC, 0x28
        uint8_t pad_29;
        keyboard_shortcut_t keyboard_shortcuts[35]; // 0x2A
        uint8_t pad_70[0xD4 - 0x70];
        char last_host[64]; // 0xD4
        uint8_t var_114;
        uint8_t var_115;
        char preferred_name[256]; // 0x116
    };
    static_assert(offsetof(config_t, keyboard_shortcuts) == 0x2A);
    static_assert(offsetof(config_t, preferred_name) == 0x116);
    static_assert(offsetof(config_t, last_host) == 0xD4);
    static_assert(sizeof(config_t) == 0x216);

    struct new_config
    {
        std::string loco_install_path;
        bool breakdowns_disabled = false;
    };

#pragma pack(pop)

    config_t& get();
    new_config& get_new();

    config_t& read();
    new_config& read_new_config();
    void write();
    void write_new_config();
}
