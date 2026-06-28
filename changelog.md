# 1.1.0
- Split project into separate categorized files

# 1.0.7
- Added `Use Curly Braces` setting
- Added `Quick Template Insert` setting
- Added manual template insertion (when `Quick Template Insert` is **<cr>off</c>**)
- Added `Supress Warning Popups` setting
- Node IDs is a **<co>required dependency</c>** again
- `CoinCount` variable fixed *(thanks to <cj>cvolton</c> for telling me about an alternative to `getCollectedCoinsForLevel`)*

`CoinCount` was broken due to what I assume was a Geode bug, which broke the `getCollectedCoinsForLevel` method.

# 1.0.6
- Code improvements

# 1.0.5
- Implemented proper string filter for settings
- Removed `settings.json` guide

# 1.0.4
- Added GD 2.2081 & Geode v5 support
- NodeIDs is now an **optional dependency**, rather than a required one
- Added some mod page links

# 1.0.3
- Code improvements
- Added `CoinCount` template replacement
- Improved UI (Inspired by prevter's Comment Emojis Reloaded)

# 1.0.2
- Added `settings.json` guide to the about page

# 1.0.1
- Bug fixes
- Changed template replacement strings, e.g. `{attempts}` to `AttemptCount` (case sensitive)

# 1.0.0
- First release