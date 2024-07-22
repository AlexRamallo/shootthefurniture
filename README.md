# Shoot The Furniture

Ludum Dare 55 jam game made with [O3DE](https://o3de.org).

* [LDJam page](https://ldjam.com/events/ludum-dare/55/shoot-the-furniture-featuring-s-k-eleton)
* [Itch page](https://skleembof.itch.io/shoot-the-furniture-featuring-sk-eleton)

This is a simple first person shooter game created in 72 hours for the Ludum Dare 55 game jam using
O3DE. The goal is to find all the [beepers](https://en.wikipedia.org/wiki/Pager)
on the map without getting killed by the ~~furniture~~ potted plants that shoot you.

## Overview

* `GhettoParticle`/`GhettoParticleSystem` is a jerry-rigged particle system because I could not find
  any existing particle solution at the time (except for PopcornFX, which I neither own nor know
  how to use). The hard part was figuring out how material instancing works, but luckily I was able
  to figure it out after digging through o3de sources.

* `PlayerControlComponent`/`FPSCameraComponent` work together to implement the player controller.
  (the jam version had buggy movement, but it's fixed here)

* `Easing.h/.cpp` has some easing functions used to implement animations for the UI and particles.

* There were going to be more enemy types, but in the end there was only time for one.

* `build_sublime.sh`/`LD55.sublime-project` can be used for building in Sublime Text on Linux.
  Customize as-needed

* `winbuild.txt` contains the command used to create the release build for windows

## License and Attribution

The code was written by [Alex](https://aramallo.com) and made available under the MIT open source license. See [LICENSE.txt](LICENSE.txt) for full license.

All art assets not listed in the section below were created by [Leo](https://skleembof.com) and made available under a [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) license

### Other assets

The following assets were used from freesound.org, opengameart.org, and hdri-haven.com. Visit the links below to find their original source and license information.

* Weapon_Revolver_Shots_Stereo.wav by Nox_Sound -- https://freesound.org/s/541818/ -- License:
  Creative Commons 0 
* 22lr revolver handling clicking foley by serøtōnin -- https://freesound.org/s/677159/ -- License:
  Creative Commons 0
* Carpet Footsteps.wav by 180156 -- https://freesound.org/s/445504/ -- License: Attribution 3.0
* Stepping-on-bones OWI.wav by nayahnaidoo -- https://freesound.org/s/656054/ -- License: Creative
  Commons 0
* Breaking Glass 10.wav by theplax -- https://freesound.org/s/546668/ -- License: Attribution 4.0
* Rock Smash by NeoSpica -- https://freesound.org/s/512243/ -- License: Creative Commons 0
* deathr.wav by Michel88 -- https://freesound.org/s/76961/ -- License: Sampling+
* Alien Grunt 3.wav by FiveBrosStopMosYT -- https://freesound.org/s/554408/ -- License: Attribution
  4.0
* coin clatter 7.wav by FenrirFangs -- https://freesound.org/s/213983/ -- License: Creative Commons
  0
* Air Conditioner by sonically_sound -- https://freesound.org/s/625224/ -- License: Creative Commons
  0
* [Equipment Storage Room HDRI](https://hdri-haven.com/hdri/equipment-storage-room) License:
  Creative Commons 0
* [Pediatric Hospital Room HDRI](https://hdri-haven.com/hdri/pediatric-hospital-room) License:
  Creative Commons 0
* [FPS Arms](https://opengameart.org/content/fps-arms-rigged-only) License:
  Creative Commons 0