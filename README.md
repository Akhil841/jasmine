# jasmine
Jasmine is a Game Boy emulator written in C++.

## Required dependencies 
(all of these should be in the `std` of any C++ compiler worth its salt):

- `fstream`
- `iostream`
- `string`
- `cmath`

Also, [SDL2](https://www.libsdl.org/download-2.0.php) is required for sound and graphics.

## Optional dependencies:

* `Makefile` to make compilation and testing easier
    * The Makefile commands only work in UNIX-like shells, so if you're using Windows, you don't need Makefile.
* [g++](https://en.wikipedia.org/wiki/GNU_Compiler_Collection) is used as the compiler. You can use any C++ compiler, but the Makefile's commands use g++.

## Compiling Jasmine

Clone the repository, and enter its root folder. Type `make` into the shell.

On Windows, compile the code using any compiler you like.

## Running Jasmine

First, either put your own copy of the Game Boy BIOS in the `bios` folder, or run `tools/makebios.cpp`. If you used the Makefile, then this should have already been written for you. Otherwise, compile and run `tools/makebios.cpp` in the repository's root directory.

Type in `./jasmine` (or the name of the EXE file you got when compiling in Windows) followed by your ROM name.

ROMs are copyrighted and illegal to share. Dump your own GB cartridges or download some ROMs at your own risk.

A display with the game you chose will appear.

## Controls

WASD: Up, Left, Down, Right

K: A

L: B

Enter: START

Shift: SELECT

## Current features
* Support for the following memory mappers (the links tell you which Game Boy games use which memory mappers):
    * [MBC1](https://gbhwdb.gekkio.fi/cartridges/mbc1.html)
        * MBC1M support is undefined. I haven't implemented anything in the code for it in particular since I don't know how it works, but from [what I can find](https://niwanetwork.org/wiki/MBC1_(Game_Boy_mapper)), the difference between MBC1 and MBC1M is entirely in wiring and not via a different chip so it may work in theory. If you want to run a multi-game compilation with guaranteed success, make it MMM01 (see [planned features](#planned-features-in-order-of-when-i-will-implement-them)) rather than MBC1M.
    * [MBC2](https://gbhwdb.gekkio.fi/cartridges/mbc2.html)
    * [MBC5](https://gbhwdb.gekkio.fi/cartridges/mbc5.html)
* RAM viewer

## Planned features, in order of when I will implement them
* DMG-style display
* Keyboard input
* [MBC3](https://gbhwdb.gekkio.fi/cartridges/mbc3.html) support. Many of my favorite GB games use an MBC3, so this is a must for me. Also, the implementation of an RTC sounds kind of fun.
* Sound because duh
* [HuC1](https://gbhwdb.gekkio.fi/cartridges/huc1.html) support. Only 5 games, but they're all cool as heck. Bomberman? The Pokemon TCG games? Sign me up.
* [MMM01](https://gbhwdb.gekkio.fi/cartridges/mmm01.html) support. It should be easy enough because the MMM01 is just multiple MBC1s. However, considering that only 2 games use the MMM01 chip, it might not be worth the effort.
    * M161 support *may* come in the future, but it's kind of unlikely since only one game uses an M161 chip. The reason I'm even considering it is because an M161 is just an MMM01 that's limited to 1 bankswitch.
* [Game Boy Color](https://en.wikipedia.org/wiki/Game_Boy_Color) support. It shouldn't be difficult considering that I got the base Game Boy working, and the Color only has a couple upgrades to that.

## Unlikely to ever be supported, in order of when I would hypothetically implement them
* [MBC6](https://gbhwdb.gekkio.fi/cartridges/mbc6.html) support. Only one game, Net de Get - Minigame @ 100, solely released in Japan, uses this chip. The game works by installing minigames from the Web using the [Mobile Game Boy Adapter](https://nintendo.fandom.com/wiki/Mobile_Game_Boy_Adapter), whose servers shut down in 2002. Anyone who wants to actually play this game needs to use a time machine, because the effort doesn't justify the payoff in my eyes.
* [MBC7](https://gbhwdb.gekkio.fi/cartridges/mbc7.html) support. First off, only a couple games use it, so it's not like the populace is exactly clamoring for this feature. Secondly, every game that uses this mapper also uses an accelerometer, and this software doesn't have accelerometer support, since I am never porting it to a system that has an accelerometer.
* [Wisdom Tree](https://en.wikipedia.org/wiki/Wisdom_Tree) mapper support. These games are unlicensed and use a custom mapper. However, this mapper is extremely simple to implement, so I might implement it just for the fun of it.
* Linking between 2-4 instances of this program. Sorry Pokemon fans, use a different emulator instead.
    * By extension, the [Game Boy Printer](https://en.wikipedia.org/wiki/Game_Boy_Printer) will likely never be supported.
* [Super Game Boy](https://en.wikipedia.org/wiki/Super_Game_Boy) borders. Properly implementing this would basically require me to write a SNES emulator. If I ever do decide to do that, then I might implement this.
* Linking between 2-4 instances of this program running on different computers. I'm not hosting a server for that.

## Will NEVER be supported, in order of easiest to hardest to implement

* [Game Boy Camera](https://en.wikipedia.org/wiki/Game_Boy_Camera), [Tamagotchi](https://en.wikipedia.org/wiki/Tamagotchi#Video_games), and [HuC3](https://gbhwdb.gekkio.fi/cartridges/huc3.html) support. Lack of documentation :(
* Support for games that use a custom chip, other than Wisdom Tree games since they have an extremely simple mapper. This includes (but is not limited to) [EMS](http://www.noiseandroid.com/lang-en/herramientas/36-ems-usb-gameboy-smart-card-64m-lsdj.html) and [Bung](https://www.noveltysouth.tech/archive/Bung-Gb-Xchanger-Mgd3-gameboy-Flash-Linker-W-16m-Game-Boy-Cartridge/woy5a8bcd594b58.html) games.
    * Why on earth would you use a flash cart on an emulator? Use it on original hardware lol
* Rumble support. At least in the case of MBC6/7, I can implement the actual memory mapper and pat myself on the back. With this, there is nothing to do since I am not porting this to a system that has rumble.
