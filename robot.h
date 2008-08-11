/* $Id$
 * MegaZeux
 *
 * Copyright (C) 2004 Gilead Kutnick
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef ROBOT_H
#define ROBOT_H

#include <stdio.h>

typedef struct _Board Board;
typedef struct _World World;

// Let's not let a robot's stack get larger than 64k right now.
// The value is a bit arbitrary, but it's mainly there to prevent MZX from
// crashing when under infinite recursion.

#define ROBOT_START_STACK 4
#define ROBOT_MAX_STACK   65536

typedef struct
{
  // Point this to the name in the robot
  char *name;
  int position;
  // Set to 1 if zapped
  int zapped;
} Label;

typedef struct
{
  int program_length;
  char *program;                  // Pointer to robot's program
  char robot_name[15];
  unsigned char robot_char;
  // Location of start of line (pt to FF for none)
  int cur_prog_line;
  int pos_within_line;   					// Countdown for GO and WAIT
  int robot_cycle;
  int cycle_count;
  char bullet_type;
  char is_locked;
  char can_lavawalk;     					// Can always travel on fire
  char walk_dir;         					// 1-4, of course
  char last_touch_dir;   					// 1-4, of course
  char last_shot_dir;    					// 1-4, of course

  // Used for IF ALIGNED "robot", THISX/THISY, PLAYERDIST,
  // HORIZPLD, VERTPLD, and others. Keep udpated at all
  // times. Set to -1/-1 for global robot.
  int xpos, ypos;

  // 0 = Un-run yet, 1=Was run but only END'd, WAIT'd, or was
  // inactive, 2=To be re-run on a second robot-run due to a
  // rec'd message
  char status;

  // This is deprecated. Probably remove.
  char used;                      // Set to 1 if used onscreen, 0 if not

  // Loop count. Loops go back to first seen LOOP
  // START, loop at first seen LOOP #, and an ABORT
  // LOOP jumps to right after first seen LOOP #.
  int loop_count;

  int num_labels;
  Label **label_list;

  int stack_size;
  int stack_pointer;
  int *stack;

  // Local counters - store in save file (ignore "blank" in world file)
  int local[32];
} Robot;

typedef struct
{
	int num_lines;

  // Pointer to scroll's message
  char *mesg;
  int mesg_size;

  // Set to 1 if used onscreen, 0 if not
  char used;
} Scroll;

typedef struct
{
  char sensor_name[15];
  char sensor_char;
  char robot_to_mesg[15];

  // Set to 1 if used onscreen, 0 if not
  char used;
} Sensor;

extern int commands;

Robot *load_robot_allocate(FILE *fp, int savegame);
void load_robot(Robot *cur_robot, FILE *fp, int savegame);
Scroll *load_scroll_allocate(FILE *fp, int savegame);
void load_scroll(Scroll *cur_scroll, FILE *fp, int savegame);
Sensor *load_sensor_allocate(FILE *fp, int savegame);
void load_sensor(Sensor *cur_sensor, FILE *fp, int savegame);
void save_robot(Robot *cur_robot, FILE *fp, int savegame);
void save_scroll(Scroll *cur_scroll, FILE *fp, int savegame);
void save_sensor(Sensor *cur_sensor, FILE *fp, int savegame);
void clear_robot(Robot *cur_robot);
void clear_scroll(Scroll *cur_scroll);
void clear_sensor(Sensor *cur_sensor);
void clear_robot_id(Board *src_board, int id);
void clear_scroll_id(Board *src_board, int id);
void clear_sensor_id(Board *src_board, int id);
void reallocate_scroll(Scroll *scroll, int size);
void reallocate_robot(Robot *robot, int size);
Label **cache_robot_labels(Robot *robot, int *num_labels);
void clear_label_cache(Label **label_list, int num_labels);
int find_robot(Board *src_board, char *name, int *first, int *last);
void send_sensor_command(World *mzx_world, int id, int command);
void send_robot(World *mzx_world, char *name, char *mesg,
 int ignore_lock);
int send_robot_id(World *mzx_world, int id, char *mesg, int ignore_lock);
void send_robot_def(World *mzx_world, int robot_id, int mesg_id);
void set_robot_position(Robot *cur_robot, int position);
int find_zapped_label_position(Robot *cur_robot, char *name);
int find_label_position(Robot *cur_robot, char *name);
Label *find_label(Robot *cur_robot, char *name);
Label *find_zapped_label(Robot *cur_robot, char *name);
int send_robot_direct(Robot *cur_robot, char *mesg, int ignore_lock);
void send_robot_all(World *mzx_world, char *mesg);
void send_sensors(World *mzx_world, char *name, char *mesg);
int move_dir(Board *src_board, int *x, int *y, int dir);
void prefix_first_last_xy(World *mzx_world, int *fx, int *fy,
 int *lx, int *ly, int robotx, int roboty);
void prefix_mid_xy(World *mzx_world, int *mx, int *my, int x, int y);
void prefix_last_xy_var(World *mzx_world, int *lx, int *ly,
 int robotx, int roboty, int width, int height);
void prefix_mid_xy_var(World *mzx_world, int *mx, int *my,
 int robotx, int roboty, int width, int height);
void prefix_first_xy_var(World *mzx_world, int *fx, int *fy,
 int robotx, int roboty, int width, int height);
int fix_color(int color, int def);
int restore_label(Robot *cur_robot, char *label);
int zap_label(Robot *cur_robot, char *label);
int next_param(char *ptr, int pos);
char *next_param_pos(char *ptr);
int parse_param(World *mzx_world, char *robot, int id);
void robot_box_display(World *mzx_world, char *program,
 char *label_storage, int id);
int robot_box_down(char *program, int pos, int count);
int robot_box_up(char *program, int pos, int count);
void robot_frame(World *mzx_world, char *program, int id);
void display_robot_line(World *mzx_world, char *program, int y, int id);
void push_sensor(World *mzx_world, int id);
void step_sensor(World *mzx_world, int id);
char *tr_msg(World *mzx_world, char *mesg, int id, char *buffer);
void add_robot_name_entry(Board *src_board, Robot *cur_robot, char *name);
void remove_robot_name_entry(Board *src_board, Robot *cur_robot, char *name);
void change_robot_name(Board *src_board, Robot *cur_robot, char *new_name);
void robot_stack_push(Robot *cur_robot, int value);
int robot_stack_pop(Robot *cur_robot);
int find_free_robot(Board *src_board);
int find_free_scroll(Board *src_board);
int find_free_sensor(Board *src_board);
void duplicate_robot_direct(Robot *cur_robot, Robot *copy_robot,
 int x, int y);
int duplicate_robot(Board *src_board, Robot *cur_robot, int x, int y);
void replace_robot(Board *src_board, Robot *src_robot, int dest_id);
void replace_scroll(Board *src_board, Scroll *src_scroll, int dest_id);
void replace_sensor(Board *src_board, Sensor *src_sensor, int dest_id);
void duplicate_scroll_direct(Scroll *cur_scroll, Scroll *copy_scroll);
int duplicate_scroll(Board *src_board, Scroll *cur_scroll);
void duplicate_sensor_direct(Sensor *cur_sensor, Sensor *copy_sensor);
int duplicate_sensor(Board *src_board, Sensor *cur_sensor);
int get_sensor_board_offset(Board *src_board, Sensor *cur_sensor);
int get_scroll_board_offset(Board *src_board, Scroll *cur_scroll);
int get_robot_board_offset(Board *src_board, Robot *cur_robot);
void optimize_null_objects(Board *src_board);
void create_blank_robot_direct(Robot *cur_robot, int x, int y);
Robot *create_blank_robot(int x, int y);
Scroll *create_blank_scroll();
void create_blank_scroll_direct(Scroll *cur_croll);
Sensor *create_blank_sensor();
void create_blank_sensor_direct(Sensor *cur_sensor);
void clear_robot_contents(Robot *cur_robot);
void clear_scroll_contents(Scroll *cur_scroll);
int get_robot_id(Board *src_board, char *name);

// These are part of runrobo2.cpp
void magic_load_mod(World *mzx_world, char *filename);
void save_player_position(World *mzx_world, int pos);
void restore_player_position(World *mzx_world, int pos);
void calculate_blocked(World *mzx_world, int x, int y, int id, int bl[4]);
int place_at_xy(World *mzx_world, int id, int color, int param, int x, int y);
int place_under_xy(Board *src_board, int id, int color, int param, int x,
 int y);
int place_dir_xy(World *mzx_world, int id, int color, int param, int x, int y,
 int direction);
int place_player_xy(World *mzx_world, int x, int y);
int get_random_range(int min_value, int max_value);
int send_self_label_tr(World *mzx_world, char *param, int id);
void run_robot(World *mzx_world, int id, int x, int y);
void split_colors(int color, int *fg, int *bg);
int check_at_xy(Board *src_board, int id, int fg, int bg, int param,
 int offset);
int check_under_xy(Board *src_board, int id, int fg, int bg, int param,
 int offset);
int check_dir_xy(Board *src_board, int id, int color, int param,
 int x, int y, int direction);
void copy_xy_to_xy(World *mzx_world, int src_x, int src_y,
 int dest_x, int dest_y);
void copy_board_to_board_buffer(Board *src_board, int x, int y,
 int width, int height, char *dest_id, char *dest_param,
 char *dest_color, char *dest_under_id, char *dest_under_param,
 char *dest_under_color, Board *dest_board);
void copy_board_buffer_to_board(Board *src_board, int x, int y,
 int width, int height, char *src_id, char *src_param,
 char *src_color, char *src_under_id, char *src_under_param,
 char *src_under_color);
void copy_board_to_layer(Board *src_board, int x, int y,
 int width, int height, char *dest_char, char *dest_color,
 int dest_width);
void copy_layer_to_board(Board *src_board, int x, int y,
 int width, int height, char *src_char, char *src_color,
 int src_width, int convert_id);
void copy_layer_to_buffer(int x,  int y, int width, int height,
 char *src_char, char *src_color, char *dest_char,
 char *dest_color, int layer_width);
void copy_buffer_to_layer(int x, int y, int width, int height,
 char *src_char, char *src_color, char *dest_char,
 char *dest_color, int layer_width);
void clear_layer_block(int src_x, int src_y, int width,
 int height, char *dest_char, char *dest_color, int dest_width);
void clear_board_block(Board *src_board, int x, int y,
 int width, int height);
void setup_overlay(Board *src_board, int mode);
void replace_player(World *mzx_world);

#endif
