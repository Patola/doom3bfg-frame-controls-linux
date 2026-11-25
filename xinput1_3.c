/* SPDX-License-Identifier: GPL-3.0-or-later */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xinput.h>
#include <stdint.h>

typedef DWORD (WINAPI *get_state_fn)(DWORD, XINPUT_STATE *);
typedef DWORD (WINAPI *set_state_fn)(DWORD, XINPUT_VIBRATION *);

static INIT_ONCE initialization = INIT_ONCE_STATIC_INIT;
static get_state_fn system_get_state;
static set_state_fn system_set_state;
static volatile const uint8_t *equip_pressed;

static BOOL CALLBACK initialize(PINIT_ONCE once, void *parameter, void **context)
{
    HMODULE module = LoadLibraryExW(L"xinput1_4.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module) {
        system_get_state = (get_state_fn)GetProcAddress(module, "XInputGetState");
        system_set_state = (set_state_fn)GetProcAddress(module, "XInputSetState");
    }

    wchar_t path[2048];
    DWORD len = GetEnvironmentVariableW(L"DOOM3_FRAME_XINPUT_STATE", path, ARRAYSIZE(path));
    if (len == 0 || len >= ARRAYSIZE(path))
        return TRUE;
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return TRUE;
    HANDLE mapping = CreateFileMappingW(file, NULL, PAGE_READONLY, 0, 0, NULL);
    CloseHandle(file);
    if (mapping) {
        equip_pressed = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 1);
        CloseHandle(mapping);
    }
    return TRUE;
}

DWORD WINAPI doom_get_state(DWORD index, XINPUT_STATE *state)
{
    if (!state)
        return ERROR_BAD_ARGUMENTS;
    InitOnceExecuteOnce(&initialization, initialize, NULL, NULL);
    DWORD result = system_get_state ? system_get_state(index, state) : ERROR_DEVICE_NOT_CONNECTED;
    if (index != 0 || !equip_pressed)
        return result;

    // Let Doom keep polling player one even when no physical gamepad is plugged in.
    if (result != ERROR_SUCCESS)
        ZeroMemory(state, sizeof(*state));
    if (*equip_pressed)
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_X;
    return ERROR_SUCCESS;
}

DWORD WINAPI doom_set_state(DWORD index, XINPUT_VIBRATION *vibration)
{
    InitOnceExecuteOnce(&initialization, initialize, NULL, NULL);
    DWORD result = system_set_state ? system_set_state(index, vibration) : ERROR_DEVICE_NOT_CONNECTED;
    if (index == 0 && equip_pressed && result == ERROR_DEVICE_NOT_CONNECTED)
        return ERROR_SUCCESS;
    return result;
}
