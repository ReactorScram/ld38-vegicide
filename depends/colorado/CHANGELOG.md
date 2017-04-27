## Changelog for Colorado Graphics Classes

## [10.0.3]

Video encoder: Various bug patches
Prepping this version for LD38
After LD38 I am planning to promote several high-level graphics classes.

## [10.0.2]

Near as I can tell, this version was used for LD36 - Jet Racing 3

## [Unreleased 9.0.0]
### Fixed
- Bug where loading the same mesh twice could corrupt it, if it was 
using a specific texture coordinate setup. Thanks to Byte Raider for detecting 
this bug, Byte Raider is the only game that loads a mesh more than once.
- qmake no longer tries to link Qt5 libs with Colorado


### Added
- Colorado now defaults to 4x MSAA on desktop platforms.
- Changelog

## [8.0.0] - 2015-06June-02 

