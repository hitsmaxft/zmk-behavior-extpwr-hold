{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    # This pins requirements.txt provided by zephyr-nix.pythonEnv.
    zephyr.url = "github:zephyrproject-rtos/zephyr/v3.7.0";

    zephyr.flake = false;

    # Zephyr sdk and toolchain.
    zephyr-nix.url = "github:urob/zephyr-nix";
    zephyr-nix.inputs.zephyr.follows = "zephyr";
    zephyr-nix.inputs.nixpkgs.follows = "nixpkgs";

    keymap_drawer-nix = {
      url = "github:hitsmaxft/keymap-drawer";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { nixpkgs, zephyr-nix, keymap_drawer-nix, ... }:
    let
      systems =
        [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = nixpkgs.lib.genAttrs systems;
    in {

      devShells = forAllSystems (system:
        let

          pkgs = nixpkgs.legacyPackages.${system};
          zephyr = zephyr-nix.packages.${system};
          zephyrPyEnv = zephyr-nix.packages.${system}.pythonEnv;
        in {
          default = pkgs.mkShellNoCC {
            packages = with pkgs; [
              gcovr
              gcc-arm-embedded
              zephyrPyEnv
              (zephyr.sdk-0_16.override { targets = [ "arm-zephyr-eabi" ]; })

              cmake
              dtc
              ninja

              just
              #pkgs.svgexport

              # poetry build error
              clang-tools
              #ctags

            ];

            shellHook = ''

ZEPHYR_BASE=$(west config 'zephyr.base')

if [[ -n $ZEPHYR_BASE ]] ; then
    echo "found ZEPHYR_BASE: $ZEPHYR_BASE"

    Zephyr_DIR=$ZEPHYR_BASE/share/zephyr-package/cmake/

    LIB_BASE_DIR="$(dirname "$ZEPHYR_BASE")"

    ZMK_SRC_DIR=$LIB_BASE_DIR/zmk/app
    ZMK_BASE=$LIB_BASE_DIR/zmk

    export LIB_BASE_DIR
    export ZMK_SRC_DIR
    export ZMK_BASE
    export Zephyr_DIR
else
    echo "found ZEPHYR_BASE not found: $ZEPHYR_BASE"
    echo "please run just update-config"
fi


            '';
          };
        });
    };
}
