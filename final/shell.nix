{ systemPkgs ? import <nixpkgs> {} }:

let
	lib  = systemPkgs.lib;
	pkgs = systemPkgs;

	# clangd hack.
	llvmPackages = pkgs.llvmPackages_latest;
	clang-unwrapped = llvmPackages.clang-unwrapped;
	clang  = llvmPackages.clang;
	clangd = pkgs.writeScriptBin "clangd" ''
	    #!${pkgs.stdenv.shell}
		export CPATH="$(${clang}/bin/clang -E - -v <<< "" \
			|& ${pkgs.gnugrep}/bin/grep '^ /nix' \
			|  ${pkgs.gawk}/bin/awk 'BEGIN{ORS=":"}{print substr($0, 2)}' \
			|  ${pkgs.gnused}/bin/sed 's/:$//')"
		export CPLUS_INCLUDE_PATH="$(${clang}/bin/clang++ -E - -v <<< "" \
			|& ${pkgs.gnugrep}/bin/grep '^ /nix' \
			|  ${pkgs.gawk}/bin/awk 'BEGIN{ORS=":"}{print substr($0, 2)}' \
			|  ${pkgs.gnused}/bin/sed 's/:$//')"
	    ${clang-unwrapped}/bin/clangd
	'';

in pkgs.mkShell {
	name = "cpsc-223c-final-dev";

	# Poke a PWD hole for our shell scripts to utilize.
	PROJECT_ROOT   = builtins.toString ./.;
	PROJECT_SYSTEM = pkgs.system;

	buildInputs = with pkgs; [ json_c ];

	nativeBuildInputs = with pkgs; [
		automake
		autoconf
		curl
		gdb
		man-db
		pkg-config
	] ++ [
		clangd
		clang
	];

	hardeningDisable = [ "fortify" ];
}
