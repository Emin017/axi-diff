{
  description = "Flake for develop";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  } @ inputs:
    flake-utils.lib.eachDefaultSystem
    (
      system: let
        pkgs = import nixpkgs {inherit system;};
        deps = with pkgs; [
          git
          jdk21
          mill
          gnumake
          autoconf
          automake
          cmake
          verilator
          cmake
          openocd
          circt
        ];
      in {
        legacyPackages = pkgs;
        formatter = pkgs.alejandra;
        devShells.default = pkgs.mkShell.override {stdenv = pkgs.clangStdenv;} {
          buildInputs = deps;
          shellHook = ''
            export AXI_DIFF_HOME=`pwd`
            make init
            make dramsim
          '';
        };
      }
    )
    // {inherit inputs;};
}
