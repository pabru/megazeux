/* MegaZeux
 *
 * Copyright (C) 2004 Gilead Kutnick <exophase@adelphia.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "SDL.h"
#include "macro.h"

struct _config_info
{
  // Video options
  int fullscreen;
  int resolution_width;
  int resolution_height;
  int window_width;
  int window_height;
  int allow_resize;
  char video_output[16];
  int height_multiplier;
  int force_32bpp;

  // Audio options
  int output_frequency;
  int buffer_size;
  int oversampling_on;
  int resample_mode;
  int modplug_resample_mode;
  int music_volume;
  int sam_volume;
  int pc_speaker_volume;
  int music_on;
  int pc_speaker_on;

  // Game options
  char startup_file[256];
  char default_save_name[256];
  int mzx_speed;

  // Robot editor options
  char color_codes[32];
  int color_coding_on;
  int disassemble_extras;
  int disassemble_base;
  int default_invalid_status;
  char default_macros[5][64];
  int redit_hhelp;

  // Backup options
  int backup_count;
  int backup_interval;
  char backup_name[256];
  char backup_ext[256];

  // Macro options
  int num_extended_macros;
  int num_macros_allocated;
  ext_macro **extended_macros;

  // Misc option
  int mask_midchars;
};

typedef void (* config_function)(config_info *conf,
 char *name, char *value, char *extended_data);

typedef struct
{
  char option_name[32];
  config_function change_option;
} config_entry;

config_entry *find_option(char *name);
void set_config_from_file(config_info *conf, char *conf_file_name);
void default_config(config_info *conf);
void set_config_from_command_line(config_info *conf, int argc,
 char *argv[]);

#endif