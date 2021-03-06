/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file openttd.h Some generic types. */

#ifndef OPENTTD_H
#define OPENTTD_H

#include "core/enum_type.hpp"

/** Mode which defines the state of the game. */
enum GameMode {
	GM_MENU,
	GM_NORMAL,
	GM_EDITOR,
	GM_BOOTSTRAP
};

/** Mode which defines what mode we're switching to. */
enum SwitchMode {
	SM_NONE,
	SM_NEWGAME,         ///< New Game --> 'Random game'.
	SM_RESTARTGAME,     ///< Restart --> 'Random game' with current settings.
	SM_EDITOR,          ///< Switch to scenario editor.
	SM_LOAD_GAME,       ///< Load game, Play Scenario.
	SM_MENU,            ///< Switch to game intro menu.
	SM_SAVE_GAME,       ///< Save game.
	SM_SAVE_HEIGHTMAP,  ///< Save heightmap.
	SM_GENRANDLAND,     ///< Generate random land within scenario editor.
	SM_LOAD_SCENARIO,   ///< Load scenario from scenario editor.
	SM_START_HEIGHTMAP, ///< Load a heightmap and start a new game from it.
	SM_LOAD_HEIGHTMAP,  ///< Load heightmap from scenario editor.
};

/** Display Options */
enum DisplayOptions {
	DO_SHOW_TOWN_NAMES     = 0, ///< Display town names.
	DO_SHOW_STATION_NAMES  = 1, ///< Display station names.
	DO_SHOW_SIGNS          = 2, ///< Display signs.
	DO_FULL_ANIMATION      = 3, ///< Perform palette animation.
	DO_FULL_DETAIL         = 5, ///< Also draw details of track and roads.
	DO_SHOW_WAYPOINT_NAMES = 6, ///< Display waypoint names.
	DO_SHOW_COMPETITOR_SIGNS = 7, ///< Display signs, station names and waypoint names of opponent companies. Buoys and oilrig-stations are always shown, even if this option is turned off.
};

extern GameMode _game_mode;
extern SwitchMode _switch_mode;
extern bool _exit_game;

#if defined(WIN32)
extern bool _in_event_loop_post_crash;

inline bool InEventLoopPostCrash()
{
	return _in_event_loop_post_crash;
}
#else
inline bool InEventLoopPostCrash()
{
	return false;
}
#endif

/** Modes of pausing we've got */
enum PauseMode : byte {
	PM_UNPAUSED              = 0,      ///< A normal unpaused game
	PM_PAUSED_NORMAL         = 1 << 0, ///< A game normally paused
	PM_PAUSED_SAVELOAD       = 1 << 1, ///< A game paused for saving/loading
	PM_PAUSED_JOIN           = 1 << 2, ///< A game paused for 'pause_on_join'
	PM_PAUSED_ERROR          = 1 << 3, ///< A game paused because a (critical) error
	PM_PAUSED_ACTIVE_CLIENTS = 1 << 4, ///< A game paused for 'min_active_clients'
	PM_PAUSED_GAME_SCRIPT    = 1 << 5, ///< A game paused by a game script
	PM_PAUSED_LINK_GRAPH     = 1 << 6, ///< A game paused due to the link graph schedule lagging

	/** Pause mode bits when paused for network reasons. */
	PMB_PAUSED_NETWORK = PM_PAUSED_ACTIVE_CLIENTS | PM_PAUSED_JOIN,
};
DECLARE_ENUM_AS_BIT_SET(PauseMode)

/** The current pause mode */
extern PauseMode _pause_mode;

enum GameEventFlags : uint32 {
	GEF_COMPANY_DELETE       = 1 << 0, ///< (d) A company has been deleted
	GEF_COMPANY_MERGE        = 1 << 1, ///< (m) A company has been bought by another
	GEF_RELOAD_NEWGRF        = 1 << 2, ///< (n) ReloadNewGRFData() has been called
	GEF_TBTR_REPLACEMENT     = 1 << 3, ///< (t) CMD_TEMPLATE_REPLACE_VEHICLE has been called
	GEF_DISASTER_VEH         = 1 << 4, ///< (D) A disaster vehicle exists or has been created
	GEF_TRAIN_CRASH          = 1 << 5, ///< (c) A train crash has occurred
	GEF_INDUSTRY_CREATE      = 1 << 6, ///< (i) An industry has been created (in game)
	GEF_INDUSTRY_DELETE      = 1 << 7, ///< (j) An industry has been deleted (in game)
	GEF_VIRT_TRAIN           = 1 << 8, ///< (v) A virtual train has been created
};
DECLARE_ENUM_AS_BIT_SET(GameEventFlags)

extern GameEventFlags _game_events_since_load;
extern GameEventFlags _game_events_overall;

inline void RegisterGameEvents(GameEventFlags events)
{
	_game_events_since_load |= events;
	_game_events_overall |= events;
}

char *DumpGameEventFlags(GameEventFlags events, char *b, const char *last);

extern time_t _game_load_time;

void AskExitGame();
void AskExitToGameMenu();

int openttd_main(int argc, char *argv[]);
void HandleExitGameRequest();

void SwitchToMode(SwitchMode new_mode);

#endif /* OPENTTD_H */
