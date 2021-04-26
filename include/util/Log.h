/*
util/Log.h: Logging methods.  Redirect to wxWidgets logging in normal execution

Copyright 2021 Tracy Beck

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once

#ifdef INTEGRATION_TEST
#define LogDebug(...) fprintf(stderr, __VA_ARGS__)
#else // INTEGRATION_TEST
#include <wx/wx.h>
#define LogDebug(...) wxLogDebug(__VA_ARGS__)
#endif // INTEGRATION_TEST

