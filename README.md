# CSZB Scoreboard

[User Manual](doc/users.md) --- [Developer Guide](doc/developers.md)

## Elevator Pitch

Cszb-scoreboard is an application to manage scoreboards for theatrical sports-themed events.
Notably, this application was inspired to be used in a ComedySportz\[^1\] match format.

\[^1\]: This application is not endorsed by, nor sponsored by CSz worldwide or my employer. All work
is my own or other contributors to this codebase and I hope it proves useful to anyone who would
like to use it.

## Vision

There are plenty of free scoreboards which already exist, but they are often designed for strict
time-keeping or don't have the support features which a comedy venue might like, such as the ability
to display random memes on screen at any given time. Other alternatives, such as Janis or the
ComedySportz San Jose Scoreboard have features which are useful, but are also closed source.

I set out in late 2019 to create a scoreboard which would be at least as useful as the existing CSz
themed scoreboards, but which would also be open-source, cross platform, and feature rich.

## Features

Currently, the application is not in a functional state and ready for use. I welcome any prospective
users to follow along with development and I welcome early feedback, as my hope is that this can be
a valuable alternative to existing options. As features in the \[Roadmap\] section are completed,
they will be moved to here.

- A scoreboard monitor may display:
  - Arbitrary text.

## Design Decisions

This project was always intended to be a Windows-first project, but the value of having an
application which builds and runs natively in OSX and Linux was seen as worthy of spending a bit of
extra time and effort on. This should hopefully allow for theaters with a variety of budgets and
technical needs to adopt this software, should it prove useful to them.

For GUI Widgets, [wxWidgets](https://www.wxwidgets.org/) was chosen for this project. Other
frameworks considered either lacked good Windows/Vs19 support (such as gtk) or were not seen as open
enough licenses (such as Qt).

Likewise, [CMake](https://cmake.org/) was chosen for build chain abstraction due to broad support
and lack of strong ties to another framework.

For configuration and storage, [protobuf](https://developers.google.com/protocol-buffers/) was
chosen due to the lightweight nature of implementing the layer, the user-friendliness of text
protobuf as a storage format, and the stict schema matching which some serialization engines lack.

## Roadmap

The following features are planned for inclusion in the scoreboard, where possible I've noted which
ones are considered gating for a 1.0 release.

- ~~Support for both a single monitor for a scoreboard as well as a dual monitor setup. \[Gating for
  release\]~~
- Scoreboard may display:
  - ~~Current score. \[Gating for release\]~~
  - ~~Saved images. \[Gating for release\]~~
  - ~~Images from clipboard.~~
  - Images from a Google image search.
  - Gifs from a Giphy search.
- ~~List mode for managing a guessing game like 5 Things. \[Gating for release\]~~
- Support for a third external monitor for additional information or a third team.
- ~~A saved image library with support for tags and filtering.~~
- ~~Hotkey support for queueing up pre-set images or text.~~
- ~~Hotkey support for increasing scores rapidly.~~
- ~~A countdown timer.~~
- Slide-show functionality for pre-show slides.
- Transition animation support.
- Video playback, with audio.

## License

Copyright 2019-2023 Tracy Beck

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at

```
http://www.apache.org/licenses/LICENSE-2.0
```

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.

## WebView2 License

This software uses Microsoft's WebView2 library, which bears the following license:

Copyright (C) Microsoft Corporation. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this list of conditions and
  the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice, this list of conditions
  and the following disclaimer in the documentation and/or other materials provided with the
  distribution.
- The name of Microsoft Corporation, or the names of its contributors may not be used to endorse or
  promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
