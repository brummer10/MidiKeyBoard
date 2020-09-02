# Mamba
Virtual Midi Keyboard and Midi file player/recorder for Jack Audio Connection Kit

![Mamba](https://github.com/brummer10/Mamba/raw/master/Mamba.png)


## Features

- Virtual Midi Keyboard for [Jack Audio Connection Kit](https://jackaudio.org/)
- Including [NSM](https://linuxaudio.github.io/new-session-manager/) support
- Including [gettext](https://www.gnu.org/software/gettext/) localization support
- Including [fluidsynth](https://github.com/FluidSynth/fluidsynth) support
- Soundfont loader for fluidsynth
- Channel selector
- Bank and Program selector
- Keyboard mapping for qwertz, qwerty, azerty(fr) and azerty(be) selectable from menu
- Keymap Editor to setup a custom Keymap
- PC Keyboard mapping selector from C0 to C4
- Pitchbend, Balance, Modwheel, Detune, Expression, Attack, Release, Volume and Velocity controllers
- Sustain and Sostenuto switches
- Connection management Menu
- Support Midi-file load, save, record and play in loop
- BPM controller for playback speed
- Support Midi Beat Clock for playback speed
- Midi Through: forward midi input to output
- Midi input highlighting
- Resizable to a full range 127 key view
- Load Midi-files on command-line
- Support jack_transport to start/stop Midi-Loops
- Keyboard Shortcuts
- `ctrl + r` toggle Record Button
- `ctrl + p` toggle Play Button
- `ctrl + l` open load file dialog
- `ctrl + s` open save file dialog
- `ctrl + a` show info box
- `ctrl + k` show Keymap Editor
- `ctrl + q` quit
- `ctrl + c` quit

## Dependencies

- libfluidsynth-dev
- libc6-dev
- libsmf-dev
- libcairo2-dev
- libx11-dev
- liblo-dev
- libsigc++-2.0-dev
- libjack-(jackd2)-dev

## Build

- git submodule init
- git submodule update
- make
- sudo make install # will install into /usr/bin

## Build with localization support

- git submodule init
- git submodule update
- make nls
- sudo make install # will install into /usr/bin

