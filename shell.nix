{ systemPkgs ? import <nixpkgs> {} }:

let lib  = systemPkgs.lib;
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

	onion = pkgs.stdenv.mkDerivation rec {
		pname = "onion";
		version = "0.8";

		src = pkgs.fetchFromGitHub {
			owner = "davidmoreno";
			repo = "onion";
			rev = "v${version}";
			sha256 = "0ikps5ac8nnsdqzcmbhi2qfpg78246xfnjzz8mcxv0jzns6z8dvk";
		};

		outputs = [ "out" "dev" ];

		nativeBuildInputs = with pkgs; [
			cmake
		];
	};

	gitconfig = {
		name  = "Diamond Dinh";
		email = "diamondburned@csu.fullerton.edu";
	};

in pkgs.mkShell {
	name = "cpsc-223c-dev";

	# Poke a PWD hole for our shell scripts to utilize.
	PROJECT_ROOT   = builtins.toString ./.;
	PROJECT_SYSTEM = pkgs.system;

	GIT_COMMITTER_EMAIL = gitconfig.email;
	GIT_COMMITTER_NAME  = gitconfig.name;
	GIT_AUTHOR_EMAIL    = gitconfig.email;
	GIT_AUTHOR_NAME     = gitconfig.name;

	buildInputs = with pkgs; [
		onion
		sqlite
		json_c
	];

	nativeBuildInputs = with pkgs; [
		automake
		autoconf
		curl
		gdb
		git
		objconv
		gettext
		unixtools.xxd
		pkg-config
	] ++ [
		clangd
		clang
	];

	hardeningDisable = [ "fortify" ];
}
