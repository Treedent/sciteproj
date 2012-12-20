/**
 * prefs.c - prefs for SciteProj
 *
 *  Copyright 2006 Roy Wood, 2009-2012 Andreas Rönnquist
 *
 * This file is part of SciteProj.
 *
 * SciteProj is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SciteProj is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SciteProj.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <gtk/gtk.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "prefs.h"
#include "string_utils.h"
#include "script.h"
#include "file_utils.h"

/**
 *
 */

gboolean check_for_old_style_config();
int load_lua_config(gchar *filename, gchar *full_string);

/**
 *
 */
sciteproj_prefs prefs;

gchar *prefs_filename;

gchar *default_config_string=(gchar*)"" \
				"-----------------------------\n"
				"-- Configuration for SciteProj\n"
				"-----------------------------\n"
				"\n"
				"-- Window geometry:\n"
				"xpos=40\n"
				"ypos=40\n"
				"width=200\n"
				"height=400\n"
				"\n"
				"-- Search window geometry (-1 on xpos and ypos means default center screen):\n"
				"search_xpos=-1\n"
				"search_ypos=-1\n"
				"search_width=500\n"
				"search_height=400\n"
				"\n"
				"search_alert_file_warnings=TRUE\n"
				"search_match_case=FALSE\n"
				"search_match_whole_words=FALSE\n"
				"search_trim_results=TRUE\n"
				"\n"
				"-- other:\n"
				"give_scite_focus=FALSE\n"
				"search_give_scite_focus=TRUE\n"
				"\n"
				"show_recent=FALSE\n"
				"recent_add_to_bottom=FALSE\n"
				"\n"
				"hide_statusbar=FALSE\n"
				"\n"
				"\n";

/**
 *		Check config string - is it valid?
 */
gboolean check_config_string(gchar *in_config)
{
	gboolean result=FALSE;
	int co=0;
	gdouble tempdouble;

	gchar *tempstring=NULL;

	int pos=-1;

	gchar *value=in_config;

	// clear scite Path

	prefs.scite_path=NULL;

	for (co=0;co<(int)strlen(in_config);co++) {
		if (in_config[co]=='=') pos=co;

		if (pos==-1) {
			value++;
		}
	}

	if (pos!=-1) {
		tempstring=g_strndup(in_config,pos);
		value++;
	}

	if ((tempstring!=NULL) && (value!=NULL)) {

		tempstring=g_strchug(tempstring);
		tempstring=g_strchomp(tempstring);

		value=g_strchug(value);
		value=g_strchomp(value);

		if (g_ascii_strcasecmp(tempstring,"xpos")==0) {
			tempdouble=g_ascii_strtod(value,NULL);
			prefs.xpos=(int) tempdouble;
		}

		if (g_ascii_strcasecmp(tempstring,"ypos")==0) {
			tempdouble=g_ascii_strtod(value,NULL);
			prefs.ypos=(int)tempdouble;
		}

		if (g_ascii_strcasecmp(tempstring,"width")==0) {
			tempdouble=g_ascii_strtod(value,NULL);
			prefs.width=(int)tempdouble;
		}

		if (g_ascii_strcasecmp(tempstring,"height")==0) {
			tempdouble=g_ascii_strtod(value,NULL);
			prefs.height=(int)tempdouble;
		}


		if (g_ascii_strcasecmp(tempstring,"search_xpos")==0) {
			tempdouble=g_ascii_strtod(value,NULL);
			prefs.search_xpos=(int) tempdouble;
		}

		if (g_ascii_strcasecmp(tempstring,"search_ypos")==0) {
			tempdouble=g_ascii_strtod(value,NULL);
			prefs.search_ypos=(int)tempdouble;
		}

		if (g_ascii_strcasecmp(tempstring,"search_width")==0) {
			tempdouble=g_ascii_strtod(value,NULL);
			prefs.search_width=(int)tempdouble;
		}

		if (g_ascii_strcasecmp(tempstring,"search_height")==0) {
			tempdouble=g_ascii_strtod(value,NULL);
			prefs.search_height=(int)tempdouble;
		}

		if (g_ascii_strcasecmp(tempstring,"give_scite_focus")==0) {
			if (g_ascii_strcasecmp(value,"TRUE")==0) {
				prefs.give_scite_focus=TRUE;
			}
		}

		if (g_ascii_strcasecmp(tempstring,"search_give_scite_focus")==0) {
			if (g_ascii_strcasecmp(value,"TRUE")==0) {
				prefs.search_give_scite_focus=TRUE;
			}
		}

		if (g_ascii_strcasecmp(tempstring,"search_alert_file_warnings")==0) {
			if (g_ascii_strcasecmp(value,"FALSE")==0) {
				prefs.search_alert_file_warnings=FALSE;
			}
		}

		if (g_ascii_strcasecmp(tempstring,"search_match_case")==0) {
			if (g_ascii_strcasecmp(value,"TRUE")==0) {
				prefs.search_match_case=TRUE;
			}
		}

		if (g_ascii_strcasecmp(tempstring,"search_match_whole_words")==0) {
			if (g_ascii_strcasecmp(value,"TRUE")==0) {
				prefs.search_match_whole_words=TRUE;
			}
		}

		if (g_ascii_strcasecmp(tempstring,"scite_path")==0) {
			prefs.scite_path=g_strdup_printf("%s",value);
		}

		if (g_ascii_strcasecmp(tempstring,"show_recent")==0) {
			if (g_ascii_strcasecmp(value,"TRUE")==0) {
				prefs.show_recent=TRUE;
			}
		}

		if (g_ascii_strcasecmp(tempstring,"recent_add_to_bottom")==0) {
			if (g_ascii_strcasecmp(value,"TRUE")==0) {
				prefs.recent_add_to_bottom=TRUE;
			}
		}

		if (g_ascii_strcasecmp(tempstring,"search_trim_results")==0) {
			if (g_ascii_strcasecmp(value,"TRUE")==0) {
				prefs.search_trim_results=TRUE;
			}
		}

		if (g_ascii_strcasecmp(tempstring,"hide_statusbar")==0) {
			if (g_ascii_strcasecmp(value,"TRUE")==0) {
				prefs.hide_statusbar=TRUE;
			}
		}
	}

	if (tempstring!=NULL) g_free(tempstring);

	return result;
}



/**
 *		init_prefs
 */
gboolean init_prefs(GError **err)
{
	//FILE *fp;
	//gchar buf[PREFS_BUFSIZE];

	gchar *config_string=NULL;

	gboolean result=TRUE;

	// the list of strings
	gchar **list=NULL;
	
	// Set default values
	prefs.lhs=1;
	prefs.width=200;
	prefs.height=600;
	prefs.verbosity=0; // No informational messages
	prefs.last_file_filter=-1; // All files (my choice)

	prefs.search_xpos=-1;
	prefs.search_ypos=-1;

	prefs.search_width=500;
	prefs.search_height=400;

	prefs.give_scite_focus=FALSE;
	prefs.search_give_scite_focus=TRUE;

	prefs.search_alert_file_warnings=TRUE;

	prefs.search_match_case=FALSE;
	prefs.search_match_whole_words=FALSE;

	prefs.show_recent=FALSE;
	prefs.recent_add_to_bottom=FALSE;

	prefs.scite_path=NULL;

	prefs.search_trim_results=FALSE;

	prefs.hide_statusbar=FALSE;

	gchar *test_prefs_filename=g_build_filename(current_directory,"sciteprojrc.lua", NULL);
	
	if (!g_file_test(test_prefs_filename, G_FILE_TEST_IS_REGULAR)) {

		// the result of g_get_user_config_dir doesn't need to be freed, so we
		// dont need to put it in a pointer of its own.
		prefs_filename=g_build_filename(g_get_user_config_dir(),"sciteprojrc",NULL);

		// Check if a config-file exists
		if (!g_file_test(prefs_filename,G_FILE_TEST_IS_REGULAR)) {
			
			// First, check if ~/.sciteproj exists.
			gchar *old_configfilename=g_build_filename(g_get_home_dir(),".sciteproj",NULL);

			if (g_file_test(old_configfilename,G_FILE_TEST_IS_REGULAR)) {

				// config-file at the "old" position exists, copy its contents to the
				// new file

				/*
				gchar *old_buffer;

				g_file_get_contents(old_configfilename,&old_buffer,NULL,err);

				g_file_set_contents(prefs_filename,old_buffer,-1,err);
				*/

			} else {

				// No config-file exists, create a new one and write default values
				// g_file_set_contents(test_prefs_filename,default_config_string,-1,err);
				
				prefs_filename=g_strdup(test_prefs_filename);
			}
		}
	
	} else {
		prefs_filename=g_strdup(test_prefs_filename);
	}

	g_free(test_prefs_filename);

	// Load preferences from config

	if (!g_file_get_contents(prefs_filename,&config_string,NULL,err)) {

		result=FALSE;
		goto ERROR;
	}
	
	// Check if it is an old-style config, or a new LUA one
	if (check_for_old_style_config(config_string)) {
		
		gchar **savedlist=NULL;

		debug_printf("Old style config\n");
		// split out the lines, and add each to the list of strings
		list=g_strsplit(config_string,"\n",-1);

		savedlist=list;
			
		gchar *temp=NULL;

		do {

			temp=*list;

			if (temp!=NULL) {

				if ((temp[0]!='#') && (strcmp(temp,"")!=0)) {
					// We got a valid string:
					// no starting #, and not an empty string.

					check_config_string(temp);
				}
				list++;
			}

		} while (temp!=NULL);

		g_strfreev(savedlist);
	} else {
		// ----- New style (LUA) config

		
		
		//if (!load_lua_config(config_string)) {
		if (load_lua_config(prefs_filename, config_string)!=0) {
			printf("error loading LUA config!\n");
		}
	}

ERROR:

	g_free(config_string);

	return result;
}

/**
 *
 */
void done_prefs()
{
	g_free(prefs_filename);
}


/**
 *
 */
gboolean check_for_old_style_config(const gchar *teststring)
{
	gboolean result=FALSE;
	int co=0;
	
	// We satisfy it by checking for the default header and assume that if
	// that is there, we have an old-styled (non-LUA) config file

	if (g_str_has_prefix(teststring,
								"# ---------------------------\n"
								"# Configuration for SciteProj\n"
								"# ---------------------------\n")
	) {
		result=TRUE;
	}
	
	// Another way to check is to check for lines starting with # - as a comment
	// LUA uses "--", so this is should work to identify oldstyle config.

	for (co=0;co<strlen(teststring);co++) {
		if (teststring[co]=='\n') {
			//printf("Tecken: %c\n", teststring[co+1]);
			
			if (teststring[co+1]=='#') {
				result=TRUE;
			}
		}
	}		
	
	return result;
}


/**
 *
 */
int load_lua_config(gchar *filename, gchar *full_string)
{
	lua_State *lua;
	lua=init_script();
	
	//if (load_script_buffer(lua, config_string)!=0) {
	if (load_script_buffer(lua, full_string)!=0) {
		printf("Error loading file: %s\n", filename);
		return FALSE;
	}

	run_script(lua);
	
	prefs.xpos=lua_get_number(lua, "xpos");
	prefs.ypos=lua_get_number(lua, "ypos");
	
	prefs.width=lua_get_number(lua, "width");
	prefs.height=lua_get_number(lua, "height");
	
	prefs.search_xpos = (int)lua_get_number(lua, "search_xpos");
	prefs.search_ypos = (int)lua_get_number(lua, "search_ypos");
	prefs.search_width = lua_get_number(lua, "search_width");
	prefs.search_height = (int)lua_get_number(lua, "search_height");
	
	prefs.search_alert_file_warnings = lua_get_boolean(lua, "search_alert_file_warnings");
	
	prefs.search_trim_results = lua_get_boolean(lua, "search_trim_results");
	
	prefs.give_scite_focus = lua_get_boolean(lua, "give_scite_focus");
	prefs.search_give_scite_focus = lua_get_boolean(lua, "search_give_scite_focus");

	prefs.show_recent=(gboolean)lua_get_boolean(lua, "show_recent");
	prefs.recent_add_to_bottom=(gboolean)lua_get_boolean(lua, "recent_add_to_bottom");
	
	prefs.hide_statusbar = lua_get_boolean(lua, "hide_statusbar");

	done_script(lua);
	
	return 0;
}
