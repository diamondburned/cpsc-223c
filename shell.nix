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

	buildSingleCLibrary =
		{ pname, singleHeader ? false, ... }@args:

		pkgs.stdenv.mkDerivation (args // {
			buildPhase = lib.optionalString (!singleHeader) ''
				gcc -shared -fPIC -o lib${pname}.so ${pname}.c
			'';

			installPhase = ''
				if [[ -f lib${pname}.so ]]; then
					install -Dm755 lib${pname}.so $out/lib/lib${pname}.so
				fi
				install -Dm644 ${pname}.h $out/include/${pname}.h
			'';
		});

	libraries = {
		onion = pkgs.stdenv.mkDerivation rec {
			pname = "onion";
			version = "0.8";
	
			src = pkgs.fetchFromGitHub {
				owner = "davidmoreno";
				repo = "onion";
				rev = "v${version}";
				sha256 = "0ikps5ac8nnsdqzcmbhi2qfpg78246xfnjzz8mcxv0jzns6z8dvk";
			};
	
			nativeBuildInputs = with pkgs; [
				cmake
			];

			cmakeFlags = [ "-DONION_EXAMPLES=false" ];

			# Put the .h files into a subdirectory.
			postInstall = ''
				mkdir -p $out/include/onion
				mv $out/include/*.{h,hpp} $out/include/onion/
			'';
		};
		munit = buildSingleCLibrary rec {
			pname = "munit";
			version = "fbbdf146";
	
			src = pkgs.fetchFromGitHub {
				owner = "nemequ";
				repo = "munit";
				rev = "${version}";
				sha256 = "13725v4pps2bpndniksa58nqi9gvx0f0900k0rqvp95bxw5z8vda";
			};
		};
		minctest = buildSingleCLibrary rec {
			pname = "minctest";
			version = "0ab58347";
	
			src = pkgs.fetchFromGitHub {
				owner = "codeplea";
				repo = "minctest";
				rev = "${version}";
				sha256 = "05x0phzq4c5nq9zv17637cw79h0w2fzckvbaqfyywhwhz8ha016a";
			};
	
			singleHeader = true;
		};
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

	buildInputs =
		(with pkgs; [ sqlite json_c ]) ++
		(with libraries; [ onion munit minctest ]);

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
