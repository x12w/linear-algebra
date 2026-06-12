{
  pkgs ? import <nixpkgs> { },
}:

pkgs.mkShell {
  packages = with pkgs; [
    cmake
    gcc
    gdb
    pkg-config
    opencv
    opencl-headers
    ocl-icd
    pocl
    clinfo
    clang-tools
  ];
}
