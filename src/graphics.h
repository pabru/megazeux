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

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include "SDL.h"
#include "configure.h"

typedef enum
{
  cursor_mode_underline,
  cursor_mode_solid,
  cursor_mode_invisible
} cursor_mode_types;

#define CURSOR_BLINK_RATE 115

typedef struct
{
  Uint16 char_value;
  Uint8 bg_color;
  Uint8 fg_color;
} char_element;

typedef struct
{
  SDL_Surface *screen;
  SDL_Overlay *overlay;
  Uint32 screen_mode;
  char_element text_video[80 * 25 * 2];
  Uint8 charset[14 * 256 * 16];
  SDL_Color palette[256];
  SDL_Color intensity_palette[256];
  SDL_Color backup_palette[256];
  Uint32 current_intensity[256];
  Uint32 saved_intensity[256];
  Uint32 backup_intensity[256];

  cursor_mode_types cursor_mode;
  Uint32 fade_status;
  Uint32 cursor_x;
  Uint32 cursor_y;
  Uint32 mouse_width_mul;
  Uint32 mouse_height_mul;
  Uint32 mouse_status;
  Uint32 fullscreen;
  Uint32 resolution_width;
  Uint32 resolution_height;
  Uint32 window_width;
  Uint32 window_height;
  Uint32 bits_per_pixel;
  Uint32 allow_resize;
  Uint32 height_multiplier;
  Uint32 cursor_timestamp;
  Uint32 cursor_flipflop;
  Uint32 default_smzx_loaded;
  char *gl_filter_method;

  Uint8 default_charset[14 * 256];
  Uint8 blank_charset[14 * 256];
  Uint8 smzx_charset[14 * 256];
  Uint8 ascii_charset[14 * 256];

  Uint32 flat_intensity_palette[256];

  int  (*init_video)       (config_info*);
  int  (*check_video_mode) (int, int, int, int);
  int  (*set_video_mode)   (int, int, int, int, int);
  void (*update_screen)    (void);
  void (*update_colors)    (SDL_Color *, Uint32);
  void (*resize_screen)    (int, int);
  void (*remap_charsets)   (void);
} graphics_data;

void color_string(char *string, Uint32 x, Uint32 y, Uint8 color);
void write_string(char *string, Uint32 x, Uint32 y, Uint8 color,
 Uint32 tab_allowed);
void write_line(char *string, Uint32 x, Uint32 y, Uint8 color,
 Uint32 tab_allowed);
void color_line(Uint32 length, Uint32 x, Uint32 y, Uint8 color);
void fill_line(Uint32 length, Uint32 x, Uint32 y, Uint8 chr,
 Uint8 color);
void draw_char(Uint8 chr, Uint8 color, Uint32 x, Uint32 y);
void draw_char_linear(Uint8 color, Uint8 chr, Uint32 offset);
void draw_char_nocolor(Uint8 chr, Uint32 x, Uint32 y);

void color_string_ext(char *string, Uint32 x, Uint32 y,
 Uint8 color, Uint32 offset, Uint32 c_offset);
void write_string_ext(char *string, Uint32 x, Uint32 y,
 Uint8 color, Uint32 tab_allowed, Uint32 offset,
 Uint32 c_offset);
void write_line_ext(char *string, Uint32 x, Uint32 y,
 Uint8 color, Uint32 tab_allowed, Uint32 offset,
 Uint32 c_offset);
void color_line_ext(Uint32 length, Uint32 x, Uint32 y,
 Uint8 color, Uint32 offset, Uint32 c_offset);
void fill_line_ext(Uint32 length, Uint32 x, Uint32 y,
 Uint8 chr, Uint8 color, Uint32 offset, Uint32 c_offset);
void draw_char_ext(Uint8 chr, Uint8 color, Uint32 x,
 Uint32 y, Uint32 offset, Uint32 c_offset);
void draw_char_linear_ext(Uint8 color, Uint8 chr,
 Uint32 offset, Uint32 offset_b, Uint32 c_offset);
void draw_char_nocolor_ext(Uint8 chr, Uint32 x, Uint32 y,
 Uint32 offset, Uint32 c_offset);
void write_line_mask(char *str, Uint32 x, Uint32 y,
 Uint8 color, Uint32 tab_allowed);
void write_string_mask(char *str, Uint32 x, Uint32 y,
 Uint8 color, Uint32 tab_allowed);

Uint8 get_char(Uint32 x, Uint32 y);
Uint8 get_color(Uint32 x, Uint32 y);
Uint8 get_char_linear(Uint32 offset);
Uint8 get_color_linear(Uint32 offset);
void clear_screen(Uint8 chr, Uint8 color);
void clear_screen_no_update(Uint8 chr, Uint8 color);

void cursor_underline(void);
void cursor_solid(void);
void cursor_off(void);
void move_cursor(Uint32 x, Uint32 y);
void set_cursor_mode(cursor_mode_types mode);
cursor_mode_types get_cursor_mode();

void init_video(config_info *conf);
int set_video_mode(void);
void toggle_fullscreen(void);
void resize_screen(Uint32 w, Uint32 h);
void update_screen(void);
void set_screen(char_element *src);
void get_screen(char_element *dest);

void ec_change_byte(Uint8 chr, Uint8 byte, Uint8 new_value);
Uint8 ec_read_byte(Uint8 chr, Uint8 byte);
void ec_read_char(Uint8 chr, char *matrix);
void ec_change_char(Uint8 chr, char *matrix);
void ec_init(void);
Sint32 ec_load_set(char *name);
Sint32 ec_load_set_var(char *name, Uint8 pos);
Sint32 ec_load_set_ext(char *name, Uint8 pos);
void ec_mem_load_set(Uint8 *chars);
void ec_mem_save_set(Uint8 *chars);
void ec_save_set(char *name);
void ec_save_set_var(char *name, Uint8 offset, Uint32 size);
void ec_load_mzx(void);
void ec_load_smzx(void);
void ec_load_blank(void);
void ec_load_ascii(void);
void ec_load_char_mzx(Uint32 char_number);
void ec_load_char_smzx(Uint32 char_number);
void ec_load_char_ascii(Uint32 char_number);
Sint32 ec_load_set_secondary(char *name, Uint8 *dest);

void update_palette();
void load_palette(char *fname);
void load_palette_mem(char mem_pal[][3], int count);
void save_palette(char *fname);
void save_palette_mem(char mem_pal[][3], int count);
void init_smzx_mode();
void smzx_palette_loaded(int val);
void set_screen_mode(Uint32 mode);
Uint32 get_screen_mode();
void init_palette();
void set_palette_intensity(Uint32 percent);
void set_color_intensity(Uint32 color, Uint32 percent);
void set_rgb(Uint32 color, Uint32 r, Uint32 g, Uint32 b);
void set_red_component(Uint32 color, Uint32 r);
void set_green_component(Uint32 color, Uint32 g);
void set_blue_component(Uint32 color, Uint32 b);
Uint32 get_color_intensity(Uint32 color);
void get_rgb(Uint32 color, Uint8 *r, Uint8 *g, Uint8 *b);
Uint32 get_red_component(Uint32 color);
Uint32 get_green_component(Uint32 color);
Uint32 get_blue_component(Uint32 color);
Uint32 get_fade_status();
void set_fade_status(Uint32 fade);
void vquick_fadein(void);
void vquick_fadeout(void);
void insta_fadein(void);
void insta_fadeout(void);
void default_palette(void);

void m_hide(void);
void m_show(void);
void dump_screen();

void get_screen_coords(int screen_x, int screen_y, int *x, int *y,
 int *min_x, int *min_y, int *max_x, int *max_y);
void set_mouse_mul(int width_mul, int height_mul);

#endif // __GRAPHICS_H