_v.2.2_ 
* Added ChaosMod Installer
* Added 2 effects
* Multiple effect fixes and improvements
* Fixes for recent Legacy builds
* Added (almost) full support for Enhanced
  * The only thing not yet ported is shader effects
* (Beta) 3D Sound support, options for the sounds to follow spawned entities
* Added Crossing Challenge component. It allows to set a starting and an ending point; then, once active, it respawns player at the start on death and shows a congratulatory message when reaching the end
* ConfigApp:
  * Split Misc tab into subtabs & rename to Settings
  * Workshop: Made an option to open text and sound files for preview
  * Workshop: Improved search
  * Sound files now show effect names instead of IDs
* Replaced .ini configs with .json configs
* Changed effect weighting logic to be exponential
* Lua Scripts: Multiple fixes
* Major internal code cleanup


_v.2.1.3_  
*   Fixed permanent effects persisting through mod pausing
*   Fixed "Override Timed Effect Type" and "Override Timed Effect Timer Length" options in config utility wrongfully showing up as checked
  
  
_v2.1.2_  
*   Fixed "No Chaos" not working with distance- / displacement-based effect dispatch
*   Fixed timer-related meta effects not properly applying to displacement-based effect dispatch
*   Fixed custom sounds potentially not unloading properly for WINE / Proton users
*   Submissions downloaded from the workshop will be cached in the "workshopcache" directory now
  
  
_v2.1.1_  
*   Fixed regression causing the OBS voting overlay to not work
*   Fixed discord voting causing the voting process to lock up under WINE / Proton
  
  
_v2.1_  
*   Added "Real First Person", "Cocktail Shaker", "Boost Braking", "Hue Shift" and "Use The Force" effects
*   Added sorting options, a search bar, a toggle to sort installed submissions first and a setting to point to a custom workshop server in the Workshop tab of the config utility
*   Added .disablebuiltineffects feature flag
*   FiveM b3095 is now marked as supported by the mod
*   The mod now allows all models to be spawned (allowing workshop effects like "Inception" to work)
*   Fixed "Invalid Twitch Channel" error message popping up on game startup if voting is enabled
*   Meta effects are now excluded from the "Max amount of simultaneously running effects" option
*   Meta effects will not be preemptively stopped by Failsafe anymore
  
  
_v2.0.1_  
*   Fixed effects on rare occasion getting stuck in the effect list with the effect timer overflowing
*   Fixed movement speed modifier effects not working after a while
*   Fixed "Meta" item in Meta config utility tab not showing the count of enabled items
  
  
_v2.0_  
  
NOTE: If you had chat voting enabled beforehand make sure to turn on one of the supported voting platforms in the config utility after updating!  
  
**New Features:**  
  
*   Added 23 new effects  
    \- 0.5x & 5x & 10x Movement Speed  
    \- 1 Wanted Star & 3 Wanted Stars  
    \- AFK  
    \- Bad Boys  
    \- Bouncy Radar  
    \- Fake Fake Teleport  
    \- Detach Random Wheel  
    \- Disassemble Current Vehicle  
    \- ESP  
    \- Fake U-Turn  
    \- Friendly Fire  
    \- Give Everyone A Random Prop  
    \- Maximap  
    \- Muffled Audio  
    \- Not Menendez!  
    \- Pay Respect  
    \- Random Gravity  
    \- Remove Waypoint  
    \- Set Time To System Time  
    \- Smoke Trails  
    \- Solid Props  
    \- You're Toast
*   Implemented a workshop system for custom effects and sounds  
    \- This allows you to easily install collections of custom effects and sounds created by community members in the form of "submissions"  
    \- This feature is accessible through new "Workshop" tab in the config utility  
    \- You can selectively configure effects and disable files in workshop submissions from loading  
    \- Make sure to regularly click on the Refresh button to fetch newly uploaded or updated submissions  
    \- Interested in uploading your own creations? See https://github.com/gta-chaos-mod/ChaosModV/wiki/Uploading-to-the-workshop
*   Added experimental Discord voting - check chaosmod/README/voting/voting.txt for info on how to set it up
*   Added the option to dispatch effects based on distance travelled rather than time
*   Added ability to search for effects in config utility
*   Implemented a debug panel for the mod - check https://github.com/gta-chaos-mod/ChaosModV/wiki/Debug-Panel for more details
*   Added .clearlogfileonreset, .nodumps, .blacklistedcomponents and .blacklistedhooks feature flags - check https://github.com/gta-chaos-mod/ChaosModV/wiki/Feature-Flags for more details
  
  
**Improvements to existing effects:**  
  
*   "Lock Camera" is not jittery anymore
*   "Disable Random Direction" also affects mouse steering
*   Added more phrases to "Realistic Hacking"
*   Custom names will now show up as fake effect names
*   Renamed "Local Time" to "Set Time To System Time"
*   "Random Gravity" is short timed by default now
*   "GTA 2" now has a more dynamic camera, making it more alike the actual GTA 2 one
*   "Ramp Jam" and "Vehicles Shoot Rockets" now use help texts to display info rather than using the effect name for the corresponding keys
*   "Super Stunt" doesn't behave weirdly in certain cases anymore
*   Made sprunk machines in "Quick Sprunk Stop" functional
*   "Ignite Player" extinguishes the fire after a few seconds now
*   "Sideways Gravity" and "Random Gravity" now give everything invincibility for the duration of the effect
*   "Fake Death" now uses more "professional" death texts
  
  
**Changes:**  
  
*   The script parsing logic has been massively improved by using multiple threads & cached native definitions during evaluation  
    \- Having a sizeable amount of lua scripts won't block initial loading & reloading of the mod for a significant time anymore
*   Expanded the range of possible keys for effect shortcuts, allowing (almost) any key to be chosen along with being able to use CTRL and shift keys as modifiers in the config utility
*   The config utility is resizable now 🤯
*   Config files are located in the configs/ subdirectory now  
    \- The config utility will prompt you with a popup which when accepted moves all the corresponding files into that directory  
    \- Config files in the old spot will still be read by the mod, no action is necessary
*   Initial mod startup time has been improved by running hooks in separate thread
*   Significantly reduced stuttering when some effects are activated the first time - game memory scanning is now asynchronous
*   Marked game build b2944 as supported for FiveM
*   Effects now always immediately show their name on the right (with the exception of "Fake" ones)
*   Further changes to Lua scripts  
    \- ScriptInfo and ScriptId have been renamed to EffectInfo and EffectId (with backwards compatibility for the old names of course)  
    \- A whole lot of new functions have been exposed to the Lua runtime, check them here https://github.com/gta-chaos-mod/ChaosModV/wiki/Lua-Scripting#available-functions  
    \- Error messages have been made a lot clearer
  
  
**Fixes:**  
  
*   Dispatching certain effects several times a row will not cause a game crash anymore
*   The TwitchChatVotingProxy process won't use significant amounts of CPU resources anymore & won't cause issues with disconnects from that
*   Improvements to the timer bar & effect timer algorithms
*   Login / channel error messages for the voting system are working again - after 3(!) years
*   Blocked new stub files from game builds b2802 and b2944 from being loaded, preventing crashes caused through those assets
*   Fixed vehicle despawn bypass not working if a save file is reloaded
*   Fixed "Ramp Jam" allowing you to spawn ramps on foot after entering and exiting a vehicle
*   Fixed "Black Hole" causing crashes on certain setups

  
_v1.11.6_  
*   Fixed spawned online vehicles disappearing / kicking you out on newest game build (b2944)
  
*   Blacklisted some newly introduced stub assets in b2944, preventing crashes from accidentally spawning those in through an effect
  
  
_v1.11.5_  
*   Fixed regression causing several physics related effects to not affect peds & objects anymore
  
*   Fixed regression causing Failsafe & online vehicle despawn bypass to not work on b2699 anymore
  
  
_v1.11.4_  
*   Fixed several regressions caused by newer game builds
  
*   Added support for FiveM b2699 and b2802
  
  
_v1.11.3_  
*   New effects:  
    \- A Dark World  
    \- Hot Cougars In Your Area  
    \- Gravity Guns  
    \- Static
  
*   Added shortcut to prevent black screen softlocks (CTRL + SHIFT + K, enabled by default)
  
*   Added option to limit voting to specific Twitch users
  
*   Updated Failsafe to work with the newest game build (2699)
  
*   Fixed "Fake Teleport" not preventing fails on newest game build
  
*   (Hopefully) fixed potential crash related to random vehicles / vehicles upgrades
  
*   Minor changes to existing effects
  
  
_v1.11.2_  
*   Fixed crash when loading / starting a new game after initial load
  
*   Fixed potential crash caused by certain vehicle effects due to stub assets
  
*   Fixed eye blinking effect not working during "I'm So Tired"
  
  
_v1.11.1_  
*   Fixed custom effect sounds not working
  
*   Removed "The Blade Hungers" effect
  
*   Changes to existing effects
  
  
_v1.11.0_  
*   New effects:  
    \- Mower Mates  
    \- Random Waypoint  
    \- Spawn Angry Chimp  
    \- Make Random Stunt Jump  
    \- Speed Boost  
    \- Re-Invoke Previous Effects  
    \- TN Panel  
    \- Goddamn Auto-Rotate  
    \- Warped Camera  
    \- Dimension Warp  
    \- Shattered Screen  
    \- Split Screen Co-op  
    \- Inverted Colors  
    \- Fourth Dimension  
    \- RGB Land  
    \- Textureless  
    \- Mirrored Screen  
    \- Folded Screen  
    \- Potato  
    \- Colorful World  
    \- Arced Screen  
    \- Black Hole
  
*   A lot of changes to existing effects
  
*   Additional functionality exposed to Lua runtime (check the wiki!)
  
*   More minor changes
  
  
_v1.10.4_  
*   Fixed "Dead Eye" effect not working
  
*   Fix "No Chaos" and "What's Happening??" effects hiding the wrong mod UI components
  
*   Added help text to Meta page in config app
  
*   Fixed effect texts not showing up if only meta effects are enabled
  
  
_v1.10.3_  
*   New effects:  
    \- "Prop Hunt" (@Gorakh)  
    \- "Spinning Props" (@Gorakh)
  
*   Updated Failsafe to work in a build proof manner
  
*   Removed "Bloom" effect
  
*   Updated Blimp Strats effect (@Reguas)
  
*   Exposed meta modifiers to Lua runtime
  
*   More minor changes and fixes
  
  
_v1.10.2_  
*   Added new effect "Drought" (@MoneyWasted)
  
*   Added enabled effect counter to config app (@Last0xygen)
  
*   Added ExcludeFromVoting & IsUtility attributes for Lua scripts
  
*   Improved stability of "Doomsday" and other similar effects
  
*   Integrated online vehicle despawning bypass
  
*   Minor changes to effects
  
  
_v1.10.1_  
*   Fix regression which caused FiveM support to break again
  
  
_v1.10.0_  
*   Added new effects  
    \- Gravity Sphere (@Reguas)  
    \- I Feel Sick (@Mo-xii)  
    \- Ghost Town (@Goorakh)  
    \- Sideways Gravity (@Goorakh)  
    \- Mannequins (@Goorakh)  
    \- Delayed Camera (@Goorakh)  
    \- Flip HUD (@Goorakh)  
    \- Teleporter Malfunction (@Goorakh)  
    \- Resurrection Day (@Goorakh)  
    \- Jumpy Props (@Goorakh)  
    \- 2x Animation Speed (@Goorakh)  
    \- Prop Cars (@Goorakh)  
    \- Tiny Vehicles (@Goorakh)
  
*   Minor changes to existing effects
  
*   (Re-)Added support for FiveM
  
*   Added the ability to assign a key to activate an effect (@Last0xygen)
  
*   Added the ability to assign multiple custom sounds to an effect and let the mod variate between them
  
*   Effect texts won't overflow anymore after a certain number of concurrent effects (@Last0xygen)
  
*   Stability improvements for physics-related effects (@Parik27)
  
*   Updated native definitions for the Lua scripting engine
  
*   Updated Failsafe to support the newest game build (2545)
  
  
_v1.9.3_  
*   8 new effects  
    \- Spinning Cam (@ProfessorBiddle)  
    \- Jump! Jump! (@Last0xygen)  
    \- Tanks A Lot (@Last0xygen)  
    \- Everyone Is Bloody (@ubscal)  
    \- Get Towed (@Last0xygen)  
    \- Spawn Quarreling Couple (@juhana)  
    \- U-Turn (@juhana)  
    \- Weird Pitch
  
*   Compatibility with the Rainbomizer mod ([found here](https://github.com/Parik27/V.Rainbomizer))
  
*   Updated Failsafe to work with the latest game build (2372)
  
*   Added more texts to:  
    \- "News Team"  
    \- "Fake Death"  
    \- "Realistic Hacking"
  
*   Replaced "Lock Vehicle Camera" effect with "Lock Camera" which also works on foot (@juhana)
  
*   Increased time between each required kill for the "The Blade Hungers" effect
  
*   "Explosive Zombies" no longer clears the wanted level
  
*   "Rocket Man" now makes the Player invincible
  
*   Changed action names in "Simeon Says" to be more descriptive
  
*   Vending machines spawned by "Quick Sprunk Stop" are now more persistent
  
*   "Now This Is Some Tire Poppin'" now has a slighly longer delay between each "pop"
  
*   "In The Hood" will now apply to hostile peds more consistently
  
*   "Random Traffic" is long timed by default now
  
*   Fixed "Rapid Fire" breaking miniguns (@Last0xygen)
  
*   "High Pitch" and "Low Pitch" are marked as incompatible now
  
*   Fixed effects written in Lua not dispatching from the effect menu properly
  
  
_v1.9.2_  
*   Added 18 new effects!
  
*   Added new options to the config utility  
    \- Option to disable mod on startup by default  
    \- Option to disable effect group weighting behaviour (not recommended!)  
    \- Option to disable the "Failsafe" feature
  
  
*   Removed following effects:  
    \- "Disable Forwards / Backwards Movement" (replaced by another effect)  
    \- "Disable Left / Right Movement" (replaced by another effect)
  
  
*   Several changes to existing effects:  
    \- "Set Player Into Random Vehicle" and "Set Player Into Random Vehicle Set" are less likely to place you into a vehicle with a weird state (appearing to be under the map) now  
    \- Slightly increased weighting multiplier of "Teleportation" and "Player Kill" effect groups  
    \- "Fake Teleport" will now pause all game scripts and should thus not cause any mission fails anymore  
    \- Excluded "Fake Teleport" from the "Teleportation" effect group  
    \- "Random Traffic" now randomizes ALL vehicles (including mission ones!)  
    \- "Random Traffic" is now long timed by default  
    \- "On-Demand TV" now has a much more broader variety & randomizes its location on screen each time  
    \- "Fake Death" and "Fake Teleport" will now both display fake effect names before ending  
    \- Updated the fake vehicle explosion logic to mimic the behaviour seen in "Detonate Current Vehicle" for "Fake Death"  
    \- "Teleport To Random Location" is less likely to place you under the map now  
    \- Made "Simeon Says" meaner  
    \- "Heavy Recoil" is now less jarring  
    \- Added more names and texts to "Realistic Hacking"  
    \- "Rainbow Traffic" now has an offset for primary and secondary colors  
    \- "Dead Eye" won't break sniper rifles anymore  
    \- "One Hit KO" now disables Trevor's special ability  
    \- "Replace Current Vehicle" now also applies random upgrades  
    \- "Spawn Random Vehicle" applies more random upgrades now
  
  
_v1.9.1.3_  
*   Added 3 new effects
  
*   Modified weighting of specific effect groups as per feedback
  
*   In-game overlay voting options will now respect the "Effect Text" color in the config utility and have a background for better readability
  
*   Fixed an issue that could cause effect groups to not show up at all
  
*   Fixed Failsafe also clearing permanent effects
  
*   Added colors to log console!
  
*   Added more stuff to complain about
  
*   Some (more or less) minor changes to effects
  
  
_v1.9.1.2_  
*   Added 4 Effects  
    \- "Spawn Biker"  
    \- "Binoculars"  
    \- "Aimbot"  
    \- "Can't Tie My Shoes"
  
*   Made adjustments to Effect Groups
  
*   Fixed Twitch voting proxy crashing after loading a save
  
  
_v1.9.1.1_  
*   Fixed potential game lock-up caused by Failsafe
  
  
_v1.9.1_  
*   Added 4 effects  
    \- "Roasting"  
    \- "Cruise Control"  
    \- "Teleport To Random Store"  
    \- "Detonate Current Vehicle"
  
  
*   Common effects (like Teleportation or Spawning ones) are grouped together now  
    \- This means they are less likely to appear frequently now while allowing us to add even more of them without tipping the balance to add more variety in the future!
  
  
*   Added Failsafeâ„¢  
    \- Subsequent mission fails within a specific time-span will cause the mod to activate measures depending on the amount of fails (e.g. clearing effects or removing spawned enemies)
  
  
*   Removed following effects  
    \- "Pwned"  
    \- "No Braking Allowed"  
    \- "Player Swap"  
    \- "Everyone Turn Right"  
    \- "Explode Current Vehicle"
  
  
*   Effect specific changes  
    \- Revamped "Roll Credits"; it will no longer block user input and the screen will fade out much slower than before  
    \- "Full Acceleration" doesn't apply any force if brakes are active now  
    \- Mission entities are invincible during "Gravity Field" now, also made the effect more performant  
    \- "Pacifist" doesn't kill you for simply aiming at a ped anymore  
    \- The prompt in "Simeon Says" will now appear again after a dying  
    \- "Realistic Hacking" will now abort the hack if dead  
    \- Fixed "Lag" rotating the camera occasionally and reverting velocity if moving backwards  
    \- (Hopefully) fixed "Spawn Random Vehicle" causing an occasional mission fail if the current vehicle was mission relevant  
    \- "Brake Boosting" excludes helicopters and blimps now  
    \- Slight changes to the color of touched vehicles in "Midas Touch" to make them appear more gold-like  
    \- Renamed "Dance Troop" to "Dance Troupe"  
    \- "Bees" is long timed by default now  
    \- "Help My W Key Is Stuck" and "Spinning Peds" are short timed by default now
  
  
*   Changes to the config utility  
    \- Cleaned up the general layout  
    \- Displays a popup at startup warning you if the application has no write permissions now  
    \- Enabled the ability to change the weight for meta effects  
    \- Modified the default times for meta effects (normal timed: 90s -> 95s, short timed: 60s -> 65s)  
    \- More minor changes
  
  
*   Added the ability to quickly scroll through effects in the effects menu with Arrow Left / Right

_v1.9.0.4_  
*   Made slight modifications to the mod's graphical interface
  
*   Fixed a game crash affecting some users on the newest game build (2215)
  
*   Disabled mission fail prevention for "Fake Teleport" temporarily (cause of the issue mentioned above)
  
*   More minor changes
  
  
_v1.9.0.3_  
*   Fixed effect "Randomize Player Clothes" always choosing the same clothes
  
  
_v1.9.0.2_  
*   Included a new sound which plays every time a new effect is activated! You can either change or disable the sound by replacing / removing the following file: chaosmod/sounds/global\_effectdispatch.mp3
  
*   Added meta effect "No Chaos"
  
*   More Lua related improvements
  
*   Changes for existing effects
  
  
_v1.9.0.1_  
*   Fixed certain meta effects not working properly with Twitch integration
  
*   Further improvements for Lua scripts
  
  
_v1.9_  
*   Added ability to create your own effects in Lua! See [here](https://github.com/gta-chaos-mod/ChaosModV/wiki/Lua-Scripting-(Experimental)) for more information.
  
*   Added meta effects! These will modify the mod itself in various ways (like modifying the timer speed or activating multiple effects at once). Currently these will be active every 600 seconds by default, you can modify that (and other) values in the config utility.
  
*   7 meta effects!
  
*   Added 6 new effects
  
*   Minor effect and stability related changes / fixes
  
  
_v1.8.2.3_  
*   Added effect "High Pitch"
  
*   "x0.2 Gamespeed" and "x0.5 Gamespeed" affect the game audio now
  
*   "Minions" slightly increases the audio's pitch
  
*   "Flying Cars" allows you to adjust your vehicle's pitch now
  
*   Minor changes to "Need For Speed" and "Virtual Reality"
  
  
_v1.8.2.2_  
*   Adjusted some effects to make crashes less likely under game-stressing scenarios
  
*   Added crash dumps (making debugging crashes easier)
  
*   Fixed clowns appearing without the visual effect and not despawning after "Killer Clowns" ends
  
  
_v1.8.2.1_  
*   Fixed a crash when loading a savefile / starting a new game
  
*   Fixed performance issues with "Insane Gravity" and "Invert Gravity"
  
*   Made changes to "Brake Boosting"
  
*   Made "Virtual Reality" and "Nailguns" short-timed by default
  
*   Fixed "Fake Death" and "Suicide" causing the suicide animation to loop in rare occasions
  
  
_v1.8.2_  
*   4 new effects
  
*   Reintroduced change which fixes effect flickering, should fix issues users were experiencing before
  
  
_v1.8.1.2_  
*   Reverted change preventing effects from causing flickering to fix an issue causing a consistent mod crash for some users
  
  
_v1.8.1.1_  
*   Fixed issue with one of the newest effects causing a softlock
  
*   Shortened "Turn Turtle" effect duration
  
  
_v1.8.1_  
*   4 new effects
  
*   Removed "Bullet Time"
  
*   Effects will no longer cause flickering / any other weird behavior when certain other effects are active
  
*   (Hopefully) fixed "Bus Bois" persistently causing a crash for some users
  
*   Some effects will now respect game speed (if applicable)
  
*   More minor changes to existing effects
  
  
_v1.8_  
*   28 new effects! (all of them contributed by the community)
  
*   Fixed "Insane Gravity" and "Invert Gravity" causing crashes on the newest Steam build
  
*   (Hopefully) fixed "Set Everyone Into Random Vehicles" causing crashes
  
*   "Lag" is slightly less annoying now
  
*   Removed "Blind"
  
*   Removed "Chop Vision"
  
*   Renamed "Fog Of War" to "Gunsmoke"
  
*   Adjusted "Mercenaries"
  
*   Made "Fake Death" even more convincing
  
*   Made "Hot Traffic" hotter
  
*   More minor changes to effects
  
*   Effects in the config utility are sorted in alphabetical order now
  
*   Moved list of contributors to seperate file
  
*   Included a new experimental utility for changing the looks of the OBS overlay
  
  
_v1.7.5.1_  
*   Fixed "Vehicles Explode On Impact" not working on the newest (2060) game build
  
*   Added more delays for "Set Everyone Into Random Vehicles", should hopefully prevent the mod / game from crashing by that effect
  
*   Fixed "Deadly Aim" and "Simeon Says" causing infinite explosions if target is invincible
  
*   Improved performance of "Beyblades"
  
  
_v1.7.5_  
*   Added 6 new community-made effects
  
*   "Fake Death" is more convincing now
  
*   "Loose Trigger" now applies to everyone
  
*   "Airstrike Inbound" doesn't clear the wanted level anymore
  
*   Amplified "Heavy Recoil"
  
*   "Mercenaries" spawns them closer to the player and ensures they fight the player quicker now
  
*   "Help My W Key Is Stuck" blocks aiming
  
*   "Deep Fried" also makes vehicle headlights much brighter now
  
*   "Now This Is Some Tire Poppin'" is short timed by default now
  
  
_v1.7.4_  
*   Added 6 new effects (all of them were made by one contributor!)
  
*   "Explosive Bullets" and "Explosive Punches" are one effect now
  
*   "Revive Dead Peds" also gives a random weapon to revived peds now
  
*   Removed "No Weapon Switching" (in favor of a new effect)
  
*   "Doomsday" is more chaotic but also just a short timed effect by default now
  
*   "All Peds Are Cops" and "Peds Riot" will cancel each other out now
  
*   The Epic Games version of the game crashing on "Spawn Random Companion / Enemy" has not been resolved yet in this version, please stand by
  
  
_v1.7.3_  
*   2 new effects
  
*   All peds will now get a weapon during "Peds Riot", resulting in a total mayhem
  
*   Spawned vehicles will be more persistent now (the game may still despawn online vehicles after a few seconds, you can bypass that by installing a mod that patches that)
  
*   "Explosive Peds / Zombies" are even more explosive now!
  
*   Fixed "Explosive Zombies" occasionally still letting a wanted level slip through
  
*   Renamed "Oil Leaks" to "Oil Trails"
  
  
_v1.7.2_  
*   5 new effects! (all by one contributor)
  
*   Made sure peds and objects spawned by mod gets cleaned up once too many exist
  
*   Clearing / Toggling the mod will now remove all spawned enemies and companions too
  
*   Fixed "Peds Are Brainless" not causing any more pedestrians on sidewalks to spawn
  
*   Shotguns now shoot multiple guns if "Catto Guns" is active
  
*   Increased time limit during "Need For Speed" from 6 to 10 seconds
  
*   Fixed player not ragdolling when in a vehicle during "Player Ragdolls When Shot"
  
*   "Portal Guns" will now also set peds into vehicles they shot at
  
*   Fixed "Spawn Random Companion / Enemy" not spawning anything in FiveM SP; it can now use ANY ped model
  
  
_v1.7.1.1_  
*   Fixed "Spawn Impotent Rage" causing the mod to freeze in FiveM SP / older game builds (due to it trying to load a non-existing audio bank)
  
*   Improvements to "Explosive Bullets"
  
  
_v1.7.1_  
*   Added effects menu option (consult the effects\_menu\_readme.txt in the chaosmod folder)
  
*   Added shortcut option to pause the timer
  
*   Fixed occasional crash caused by "Vehicles Explode On Impact"
  
*   Greatly improved "Autopilot" AI, it can even finish missions almost entirely by itself now in some cases!
  
*   Added some more checks to "Add Random Upgrades To Every Vehicle", should hopefully fix crashes experienced by some users
  
*   "Add Random Upgrades To Every Vehicle" also applies random wheel types too
  
*   Fixed "Portal Guns" and "Explosive Bullets" only working on close distance
  
*   "Pwned" is short timed by default now
  
*   "Launch All Nearby Peds Up" also affects peds inside vehicles
  
*   Made "Total Votes" text in the Twitch Voting OBS overlay more visible (as suggested by some users)
  
  
_v1.7.0.1_  
*   Compatibility with FiveM SP (and probably the Epic Games Store version of the game too)!
  
*   Fixed not being able to rename effects properly
  
*   Renamed some effects
  
*   Changed how the "Lag" effect works: instead of slowing down time occasionally, it constantly resets everyones' position to their previous one!
  
*   Fixed "Add Random / Max Upgrades To Every Vehicle" causing an crash at certain times, also applies even more upgrades now
  
  
_v1.7_  
*   2 new effects
  
*   Changes to existing effects (see [here](https://pastebin.com/5Db2RDxN))  
    
*   Twitch Voting:
  
*   Added new OBS overlay option
  
*   Added "Random Effect" voteable option
  
*   Removed no vote round options
  
  
_v1.6.1_  
*   A **LOT** of effect changes (list of effects that were either changed, removed or merged can be seen [here](https://pastebin.com/ggd03TRp))
  
*   3 new effects
  
*   Fixed potential issue where some effects weren't able to be given a custom name (hopefully)
  
  
_v1.6.0.1_  
*   Fixed custom sounds only playing the first time an effect occurs
  
*   Fixed some typos (thanks to juhana)
  
  
_v1.6_  
*   6 new effects!
  
*   Changes to existing effects (see [here](https://pastebin.com/zbJjKVcp))
  
*   Added negative weighting to effects; recent effects will have a much lower chance to appear multiple times in a row
  
*   Added option to rename effects
  
*   Added ability to play a custom mp3 file when a specific effect starts
  
*   Users are now permitted to change their existing vote in Twitch chat voting
  
*   Added experimental option for Twitch votes to affect chances of effects instead of choosing effect directly
  
*   Added notice to config utility if an update is available
  
*   Removed .twitchmode file requirement to access Twitch tab in config utility
  
  
_v1.5.1.1_  
*   Fixed "Suicide" being the only effect occuring if effects.ini wasn't generated  
      
    _v1.5.1_  
    
*   5 new effects!
  
*   Reworked config system. **This will reset your Twitch settings, however all other settings are unaffected.**
  
*   Ability to reload your config in-game (CTRL + L to unload / load the mod, ensure the option for that is enabled in the config utility)
  
*   Changes to existing effects (see [here](https://pastebin.com/JVD0tzhc))
  
*   Option to exclude specific effects from ever being a choice in voting (they can still be chosen randomly by the mod itself though)
  
*   A new splash text containing the names of all the contributors!
  
*   The mod should start up even without any config files being generated now
  
*   Resetting your config in the config utility will now exclude your twitch settings unless specified
  
*   A new tab in the config utility containing links to the GitHub Repository and a place to donate
  
*   The voting system will now create a "currentvoteables.txt" file in your chaosmod folder containing the current voteable effects which can be used to show them through an OBS overlay
  
  
_v1.5_  
*   Around 20 new effects!
  
*   Voting rounds for twitch chat integration use alternated numbers now (either 1,2,3 or 4,5,6)
  
*   Several changes to existing effects (see [here](https://pastebin.com/49fa4mxJ) for a detailed list)
  
  
_v1.4.4_  
*   Removed some of the more harmless effects
  
*   Added 6 new effects (including ones contributed by the community!)
  
*   Fixed on-screen voteables text also showing up on no-vote rounds
  
  
_v1.4.3_  
*   (Twitch Mode) Added option to show voteable effects on screen
  
*   Moved effect text + effect timer bar more to the right
  
*   Fixed spawn blue sultan effect not using the proper sultan variant
  
*   Fixed brainless peds effect causing the player's ped to freeze occasionally
  
  
_v1.4.2_  
*   Added ability to have timed effects permanently on
  
*   2 new effects
  
*   Workaround for screen effects to not get overriden / removed by game
  
*   Fixed poll mode mod effect duration restrictions also applying to normal chat voting mode
  
*   Config app now shows the current mod version
  
*   Fixes and changes to existing effects
  
  
_v1.4.1_  
*   Experimental Twitch Poll voting integration (has certain restrictions, consult _chaosmod/twitch\_integration.txt_)
  
*   17 new effects
  
*   (Reuploaded) Fixes to teleport everything to player and spawn extreme griefer jesus effects
  
  
_v1.4_  
*   Twitch voting integration! Make sure to read _chaosmod/twitch\_integration.txt_ for more information.
  
*   2 new effects
  
*   Fixed lag effect causing a softlock on mission fail
  
*   Reduced amount of stutter in pause menu which occurred for some users
  
*   Effects with higher / lower weight should occur more / less often now
  
*   First Person and Cinematic Cam effects will put you back into normal view afterwards
  
*   And more changes to effects!
  
  
_v1.3.3_  
*   8 new effects (including suggested ones)
  
*   Added option to enable a keyboard shortcut to enable/disable the mod quickly
  
*   New random engine (effects shouldn't get as repeatedly activated anymore)
  
*   More changes to existing effects based on feedback
  
*   Config files will not be shipped anymore to stop users from overriding their existing ones for no reason. Run the config utility once if you installed the mod for the first time!
  
  
_v1.3.2_  
*   Added ability to change duration (normal, short or custom) and "weight" (decreases / increases their chance of occuring) of effects (customizable _per_ effect)
  
*   Added option to clear active effects with a keyboard shortcut
  
*   Added option to prevent same effect from happening twice in a row
  
*   Added option to hide either timer bar or effect texts (or both)
  
*   Changed behavior of some effects based on feedback
  
  
_v1.3.1_  
*   Fixed crashes when an effect affecting all vehicles / objects runs but no vehicles / objects exist (might fix crashing on prologue)
  
*   Fixed "Player Ragdolls When Shot" effect causing big fps drops when running
  
*   Increased frequency and intensity of "Jumpy Vehicles" effect
  
  
_v1.3_  
*   New config format. **No backwards compatibility, so make sure to regenerate your config files!**
  
*   30 new unique effects!
  
*   Changed default timer values (New effects: 60s -> 30s, Timed Effects: 180s -> 90s, Short Timed Effects: 15s -> 30s)
  
*   Fixed effect timers showing as empty for a brief moment on new effect
  
*   More bug fixes and behavior changes of effects (Check out the commits for more information!)
  
*   No chat voting integration yet in this build, will probably come as its own build
  
  
_v1.2.1_  
*   More hotfixes!
  
*   Extended entity pools to 1024 to prevent entities from being skipped
  
*   Fixed zombie and meteor shower effects spawning an endless amount of entities (and causing a game crash!)
  
  
_v1.2_  
_Mostly just a bugfixing update_  
*   10 new effects
  
*   Changes on existing effects based on feedback
  
*   🐛 Fixes!
  
  
_v1.1_  
*   20+ new effects!
  
*   Ability to configure timer bar, text and timed effect timer bar colors to your liking!
  
*   Ability to configure duration for short timed effects
  
*   Added a reset button to configuration utility
  
*   Minor visual changes to configuration utility
  
*   Changes to some existing effects based on suggestions
  
*   Other minor changes
  
  
_v1.0_  
*   Not a beta anymore!
  
*   Added the ability to toggle effects individually
  
*   Fixed timed effect duration setting not working
  
*   Fixed a specific effect crashing the game due to improper configuration
  
*   Fixed timed effects not stopping properly if cancelled by another timed effect
  
*   Config files are now being shipped by default
