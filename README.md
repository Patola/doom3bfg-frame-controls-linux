# Doom 3 BFG Steam Frame controls for Linux

Play **Doom 3 BFG Edition: Fully Possessed** with native Steam Frame controllers through WiVRn. This add-on maps the Frame’s D-pad and bumpers for the PDA, makes right X equip weapons, and fixes an XRizer deadlock that could leave the headset black when quitting.

[Download the latest package](https://github.com/Patola/doom3bfg-frame-controls-linux/releases/latest)

## Requirements

- The Steam version of Doom 3 BFG Edition and the 64-bit [Fully Possessed v0.21j Alpha mod](https://github.com/NPi2Loup/DOOM-3-BFG-VR/releases/tag/v0.021j-Alpha). Install the mod first. `Doom3BFGVR.exe` and `Doom3BFG.exe` should be beside each other, with a `Fully Possessed/` subdirectory.
- An x86-64 Linux PC with Steam and Proton. This package was tested on Arch Linux with Proton 11.0. The prebuilt XRizer library requires glibc 2.43 or newer. On older distributions, see [Build from source](#build-from-source).
- A working WiVRn connection to the Steam Frame. The host OpenXR runtime must expose the native Frame controller profile. The add-on runs on the Linux PC.
- An existing OpenVR paths file, usually `~/.config/openvr/openvrpaths.vrpath`. If SteamVR or an OpenVR runtime installer has never created one, use [XRizer’s OpenVR paths example](https://github.com/Supreeeme/xrizer/blob/a6c27cc4dc614f9d60c0745f47fa4ead4cf2f64b/README.md#openvrpathsvrpath).

## Install

1. In Steam, open **Doom 3 BFG Edition → Properties → Installed Files → Browse** to find the game directory. Close the game before copying any files. Save your current Steam launch options and back up `vr_openvr.cfg` if it exists. It is under your Steam library’s `steamapps/compatdata/208200/pfx/drive_c/users/steamuser/Saved Games/id Software/DOOM 3 BFG/Fully Possessed/` directory. Doom can save settings from this profile back to that file.
2. Download `doom3bfg-frame-controls-linux-v1.0.0.tar.xz` from the [release](https://github.com/Patola/doom3bfg-frame-controls-linux/releases/tag/v1.0.0). Extract it somewhere writable in your home directory. Keep `launch` next to `xrizer/`. You can verify the extracted files from that directory with `sha256sum -c SHA256SUMS`.
3. Copy `doom3_frame.cfg` into the game’s `Fully Possessed/` subdirectory. Copy `xinput1_3.dll` beside `Doom3BFGVR.exe`. Back up any files already at those destinations first.
4. In **Properties → General → Launch Options**, enter the full path to `launch`, followed by `%command%`. For example, if you extracted the package directly into `/home/alex`:

   ```text
   "/home/alex/doom3bfg-frame-controls-linux-v1.0.0/launch" %command%
   ```

   If the launcher is not executable after extraction, run `chmod +x` on it. The launcher picks the VR executable and the bundled XRizer for this Steam game.
5. Connect the Steam Frame to WiVRn, then launch **Doom 3 BFG Edition** from Steam.

## Controls

| Frame input | In play | In the PDA |
| --- | --- | --- |
| Left stick | Move | Left/right highlights weapons in Inventory |
| Left D-pad | | Left/right highlights weapons in Inventory |
| Right stick | Left/right turns. Up/down cycles to the next/previous weapon | |
| Left bumper | Flashlight | Previous tab |
| Right bumper | Next weapon | Next tab |
| Right X | Reload | Equip highlighted weapon in Inventory |
| Right Y | Open PDA | |
| Right A / B | Use / pause | |
| Left / right stick click | Sprint / teleport | |
| Right grip | Swap weapons at the right hip holster | Go back |
| Trigger / left grip | | Select / go back |

If you use WayVR, hold View or Menu before tapping the bumper on the same controller to use its existing shortcuts.

### A weapon is missing from the cycle

Fully Possessed omits the weapon in the hip holster when cycling in either direction. With the PDA closed and the default right-handed setup, bring your **right controller to your right hip**. Release the grip if it’s already pressed, then squeeze the **right grip button** once while the controller is at your hip. This swaps the weapon in your hand with the holstered weapon. You may feel a short buzz when the controller enters the holster area. You can also equip the weapon through **PDA → Inventory**.

As a diagnostic, `vr_slotDisable 1` in Doom’s console disables the body slots and clears the active holster entry. Set it back to `0` to re-enable the slots. Reload a save from before the test to recover its holster state. [Fully Possessed weapon cycling code](https://github.com/NPi2Loup/DOOM-3-BFG-VR/blob/v0.021j-Alpha/neo/d3xp/Player.cpp#L6415-L6535)

## Build from source

The release includes `xrizer-source.tar.gz`, the complete patched XRizer source, alongside the binaries. Install a Rust toolchain with Rust 2024 edition support, a native C toolchain and Clang/libclang for bindgen, and the x86-64 MinGW-w64 C compiler. In the extracted package directory, run:

```sh
tar -xzf xrizer-source.tar.gz
(
    cd xrizer-source
    XRIZER_VERSION=v0.6-8+a6c27cc-doom3-frame-pda4 \
        cargo build --release --locked --no-default-features --features monado
)
install -Dm755 xrizer-source/target/release/libxrizer.so xrizer/bin/linux64/vrclient.so
x86_64-w64-mingw32-gcc -shared -O2 -o xinput1_3.dll xinput1_3.c xinput1_3.def
```

The [XRizer patch](xrizer-frame-controls.patch) applies to [upstream commit `a6c27cc`](https://github.com/Supreeeme/xrizer/tree/a6c27cc4dc614f9d60c0745f47fa4ead4cf2f64b). It adds the opt-in native Frame mapping for legacy OpenVR and lets a repeated cleanup call return safely while the original call destroys the session. The XInput shim forwards normal XInput calls to Wine and passes right X to Doom as the Xbox X input that Inventory’s Equip action expects. The launcher captures diagnostics in `doom3-console.log` in the extracted package directory.

## Uninstall

Exit Doom and restore the previous Steam launch options and `vr_openvr.cfg` backup. Remove the add-on’s `doom3_frame.cfg` and `xinput1_3.dll` from the game directory, or restore any files those copies replaced. The extracted package can then be removed.

## Licenses

The launcher, Doom config, and this guide use [Apache License 2.0](LICENSE). The modified [XRizer](https://github.com/Supreeeme/xrizer) and the XInput shim use [GPL-3.0-or-later](LICENSE.GPL-3.0). Game and mod files are obtained separately.
