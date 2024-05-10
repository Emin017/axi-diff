{
  description = "Flake for develop";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs-old.url = "github:NixOS/nixpkgs/336eda0d07dc5e2be1f923990ad9fdb6bc8e28e3";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    nixpkgs-old,
  } @ inputs:
    flake-utils.lib.eachDefaultSystem
    (
      system: let
        pkgs = import nixpkgs {
          inherit system;
          config.packageOverrides = pkgs: {
            graalvm = nixpkgs-old.legacyPackages.${system}.graalvm-ce;
          };
        };
        deps = with pkgs; [
          git
          (mill.override {jre = pkgs.graalvm;})
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
