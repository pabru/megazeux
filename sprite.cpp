/* $Id$
 * MegaZeux
 *
 * Copyright (C) 1996 Greg Janson
 * Copyright (C) 1998 Matthew D. Williams - dbwilli@scsn.net
 * Copyright (C) 2002 Gilead Kutnick - exophase@adelphia.net
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

// sprite.cpp, by Exophase

// Global declarations:

#include <stdlib.h>

#include "data.h"
#include "sprite.h"
#include "graphics.h"
#include "idput.h"
#include "error.h"
#include "game.h"
#include "world.h"

// FIXME - Collision rectangles against char backgrounds (ccheck1/2)
// need to be clipped against the edges of the boards...

void plot_sprite(World *mzx_world, Sprite *cur_sprite, int color, int x, int y)
{
  if(((cur_sprite->width) && (cur_sprite->height)))
  {
    cur_sprite->x = x;
    cur_sprite->y = y;

    if(color == 288)
    {
      cur_sprite->flags |= SPRITE_SRC_COLORS;
    }
    else
    {
      cur_sprite->flags &= ~SPRITE_SRC_COLORS;
      cur_sprite->color = color;
    }

    if(!(cur_sprite->flags & SPRITE_INITIALIZED))
    {
      cur_sprite->flags |= SPRITE_INITIALIZED;
      mzx_world->active_sprites++;
    }
  }
}

// For the qsort.

int compare_spr(const void *dest, const void *src);

int compare_spr(const void *dest, const void *src)
{
  Sprite *spr_dest = *((Sprite **)dest);
  Sprite *spr_src = *((Sprite **)src);

  return ((spr_dest->y + spr_dest->col_y) -
   (spr_src->y + spr_src->col_y));
}

void draw_sprites(World *mzx_world)
{
  Board *src_board = mzx_world->current_board;
  int i, i2, i3, i4, i5, i6, st_x, st_y, of_x, of_y;
  int skip, skip2, skip3;
  int dr_w, dr_h, ref_x, ref_y, scr_x, scr_y;
  int bwidth, bheight, use_chars = 1;
  Sprite **sprite_list = mzx_world->sprite_list;
  Sprite *dr_order[MAX_SPRITES];
  Sprite *cur_sprite;
  char ch, color, dcolor;
  int viewport_x = src_board->viewport_x;
  int viewport_y = src_board->viewport_y;
  int viewport_width = src_board->viewport_width;
  int viewport_height = src_board->viewport_height;
  int board_width = src_board->board_width;
  int overlay_mode = src_board->overlay_mode;
  char *overlay = src_board->overlay;
  char *src_chars;
  char *src_colors;

  // see if y sort should be done
  if(mzx_world->sprite_y_order)
  {
    // Switched to stdlib qsort. Hooray!
    // Fill it with the active sprites in the beginning
    // and the inactive sprites in the end.
    for(i = 0, i2 = 0, i3 = MAX_SPRITES - 1; i < MAX_SPRITES; i++)
    {
      if((sprite_list[i])->flags & SPRITE_INITIALIZED)
      {
        dr_order[i2] = sprite_list[i];
        i2++;
      }
      else
      {
        dr_order[i3] = sprite_list[i];
        i3--;
      }
    }

    // Now sort it, using qsort.

    qsort(dr_order, i2, sizeof(Sprite *), compare_spr);
  }

  // draw this on top of the SCREEN window.
  for(i = 0; i < MAX_SPRITES; i++)
  {
    if(mzx_world->sprite_y_order)
    {
      cur_sprite = dr_order[i];
    }
    else
    {
      cur_sprite = sprite_list[i];
    }

    if(cur_sprite->flags & SPRITE_INITIALIZED)
    {
      calculate_xytop(mzx_world, &scr_x, &scr_y);
      ref_x = cur_sprite->ref_x;
      ref_y = cur_sprite->ref_y;
      of_x = 0;
      of_y = 0;
      st_x = (cur_sprite->x + viewport_x);
      st_y = (cur_sprite->y + viewport_y);
      if(!(cur_sprite->flags & SPRITE_STATIC))
      {
        st_x -= scr_x;
        st_y -= scr_y;
      }

      dr_w = cur_sprite->width;
      dr_h = cur_sprite->height;

      // clip draw position against viewport

      if(st_x < viewport_x)
      {
        of_x = viewport_x - st_x;
        dr_w -= of_x;
        if(dr_w < 1) continue;

        st_x = viewport_x;
      }

      if(st_y < viewport_y)
      {
        of_y = viewport_y - st_y;
        dr_h -= of_y;
        if(dr_h < 1) continue;

        st_y = viewport_y;
      }

      if((st_x > (viewport_x + viewport_width)) ||
       (st_y > (viewport_y + viewport_height))) continue;

      if((st_x + dr_w) > (viewport_x + viewport_width))
      {
        dr_w = viewport_x + viewport_width - st_x;
      }
      if((st_y + dr_h) > (viewport_y + viewport_height))
      {
        dr_h = viewport_y + viewport_height - st_y;
      }

      if(cur_sprite->flags & SPRITE_VLAYER)
      {
        use_chars = 1;
        bwidth = mzx_world->vlayer_width;
        bheight = mzx_world->vlayer_height;
        src_chars = mzx_world->vlayer_chars;
        src_colors = mzx_world->vlayer_colors;
      }
      else
      {
        use_chars = 0;
        bwidth = board_width;
        bheight = src_board->board_height;
        src_chars = src_board->level_param;
        src_colors = src_board->level_color;
      }

      if((ref_x + dr_w) > bwidth)
        dr_w = bwidth - ref_x;

      if((ref_y + dr_h) > bheight)
        dr_h = bheight - ref_y;

      i4 = ((ref_y + of_y) * bwidth) + ref_x + of_x;
      i5 = (st_y * 80) + st_x;

      if(overlay_mode == 2)
      {
        i6 = ((cur_sprite->y - scr_y) * board_width) + (cur_sprite->x - scr_x);
      }
      else
      {
        i6 = (cur_sprite->y * board_width) + cur_sprite->x;
      }

      skip = bwidth - dr_w;
      skip2 = 80 - dr_w;
      skip3 = board_width - dr_w;

      switch((cur_sprite->flags & 0x0C) >> 2)
      {
        // natural colors, over overlay
        case 3: // 11
        {
          for(i2 = 0; i2 < dr_h; i2++)
          {
            for(i3 = 0; i3 < dr_w; i3++)
            {
              color = src_colors[i4];
              if(use_chars)
              {
                ch = src_chars[i4];
              }
              else
              {
                ch = get_id_char(src_board, i4);
              }

              if(!(color & 0xF0))
              {
                color = (color & 0x0F) | (get_color_linear(i5) & 0xF0);
              }

              if(ch != 32)
              {
                if(!(cur_sprite->flags & SPRITE_CHAR_CHECK2) || !is_blank(ch))
                {
                  draw_char_linear(color, ch, i5);
                }
              }
              i4++;
              i5++;
            }
            i4 += skip;
            i5 += skip2;
          }
          break;
        }

        // natural colors, under overlay
        case 2: // 10
        {
          for(i2 = 0; i2 < dr_h; i2++)
          {
            for(i3 = 0; i3 < dr_w; i3++)
            {
              color = src_colors[i4];
              if(use_chars)
              {
                ch = src_chars[i4];
              }
              else
              {
                ch = get_id_char(src_board, i4);
              }

              if(!(color & 0xF0))
              {
                color = (color & 0x0F) | (get_color_linear(i5) & 0xF0);
              }

              if((!overlay_mode || (overlay[i6] == 32)) && ch != 32)
              {
                if(!(cur_sprite->flags & SPRITE_CHAR_CHECK2) || !is_blank(ch))
                {
                  draw_char_linear(color, ch, i5);
                }
              }
              i4++;
              i5++;
              i6++;
            }
            i4 += skip;
            i5 += skip2;
            i6 += skip3;
          }
          break;
        }

        // given colors, over overlay
        case 1: // 01
        {
          color = cur_sprite->color;

          for(i2 = 0; i2 < dr_h; i2++)
          {
            for(i3 = 0; i3 < dr_w; i3++)
            {
              if(use_chars)
              {
                ch = src_chars[i4];
              }
              else
              {
                ch = get_id_char(src_board, i4);
              }

              dcolor = color;
              if(!(color & 0xF0))
              {
                dcolor = (color & 0x0F) | (get_color_linear(i5) & 0xF0);
              }

              if(ch != 32)
              {
                if(!(cur_sprite->flags & SPRITE_CHAR_CHECK2) || !is_blank(ch))
                {
                  draw_char_linear(dcolor, ch, i5);
                }
              }
              i4++;
              i5++;
            }
            i4 += skip;
            i5 += skip2;
          }
          break;
        }

        // given colors, under overlay
        case 0: // 00
        {
          color = cur_sprite->color;

          for(i2 = 0; i2 < dr_h; i2++)
          {
            for(i3 = 0; i3 < dr_w; i3++)
            {
              if(use_chars)
              {
                ch = src_chars[i4];
              }
              else
              {
                ch = get_id_char(src_board, i4);
              }

              dcolor = color;
              if(!(color & 0xF0))
              {
                dcolor = (color & 0x0F) | (get_color_linear(i5) & 0xF0);
              }

              if((!overlay_mode || (overlay[i6] == 32)) && ch != 32)
              {
                if(!(cur_sprite->flags & SPRITE_CHAR_CHECK2) || !is_blank(ch))
                {
                  draw_char_linear(dcolor, ch, i5);
                }
              }
              i4++;
              i5++;
              i6++;
            }
            i4 += skip;
            i5 += skip2;
            i6 += skip3;
          }
          break;
        }
      }
    }
  }
}


int sprite_at_xy(Sprite *cur_sprite, int x, int y)
{
  if((x >= cur_sprite->x) && (x < cur_sprite->x + cur_sprite->width) &&
   (y >= cur_sprite->y) && (y < cur_sprite->y + cur_sprite->height)
   && (cur_sprite->flags & SPRITE_INITIALIZED))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int sprite_colliding_xy(World *mzx_world, Sprite *check_sprite, int x, int y)
{
  Board *src_board = mzx_world->current_board;
  int board_width = src_board->board_width;
  Sprite **sprite_list = mzx_world->sprite_list;
  Sprite *cur_sprite;
  int colliding = 0;
  int skip, skip2, i, i2, i3, i4, i5;
  int bwidth;
  int x1, x2, y1, y2;
  int mw, mh;
  int use_chars = 1, use_chars2 = 1;
  unsigned int x_lmask, x_gmask, y_lmask, y_gmask, xl, xg, yl, yg, wl, hl, wg, hg;
  char *vlayer_chars = mzx_world->vlayer_chars;
  char *level_id = src_board->level_id;
  int *collision_list = mzx_world->collision_list;
  int vlayer_width = mzx_world->vlayer_width;
  int board_collide = 0;

  if(!(check_sprite->flags & SPRITE_INITIALIZED)) return -1;

  // Check against the background, will only collide against customblock for now
  //  (id 5)

  if(check_sprite->flags & SPRITE_VLAYER)
  {
    use_chars = 0;
    bwidth = vlayer_width;
  }
  else
  {
    bwidth = board_width;
  }

  // Scan board area

  skip = board_width - check_sprite->col_width;;
  skip2 = bwidth - check_sprite->col_width;

  i3 = ((y + check_sprite->col_y) * board_width) + x + check_sprite->col_x;
  i4 = ((check_sprite->ref_y + check_sprite->col_y) * bwidth) +
   check_sprite->ref_x + check_sprite->col_x;

  for(i = 0; i < check_sprite->col_height; i++)
  {
    for(i2 = 0; i2 < check_sprite->col_width; i2++)
    {
      // First, if ccheck is on, it won't care if the source is 32
      int c;

      if(!use_chars)
      {
        c = vlayer_chars[i4];
      }
      else
      {
        c = get_id_char(src_board, i4);
      }
      if(!(check_sprite->flags & SPRITE_CHAR_CHECK) ||
       (c != 32))
      {
        // if ccheck2 is on and the char is blank, don't trigger.
        if(!(check_sprite->flags & SPRITE_CHAR_CHECK2) ||
         (!is_blank(c)))
        {
          if(level_id[i3] == 5)
          {
            // Colliding against background
            if(board_collide) break;
            collision_list[colliding] = -1;
            colliding++;
            board_collide = 1;
          }
        }
      }
      i3++;
      i4++;
    }
    i3 += skip;
    i4 += skip2;
  }

  for(i = 0; i < MAX_SPRITES; i++)
  {
    cur_sprite = sprite_list[i];
    if((cur_sprite == check_sprite) || !(cur_sprite->flags & SPRITE_INITIALIZED))
      continue;

    if(!(cur_sprite->col_width) || !(cur_sprite->col_height))
     continue;

    x1 = cur_sprite->x + cur_sprite->col_x;
    x2 = x + check_sprite->col_x;
    y1 = cur_sprite->y + cur_sprite->col_y;
    y2 = y + check_sprite->col_y;
    x_lmask = (signed int)(x1 - x2) >> 31;
    x_gmask = ~x_lmask;
    y_lmask = (signed int)(y1 - y2) >> 31;
    y_gmask = ~y_lmask;
    xl = (x1 & x_lmask) | (x2 & x_gmask);
    xg = (x1 & x_gmask) | (x2 & x_lmask);
    yl = (y1 & y_lmask) | (y2 & y_gmask);
    yg = (y1 & y_gmask) | (y2 & y_lmask);
    wl = (cur_sprite->col_width & x_lmask) |
     (check_sprite->col_width & x_gmask);
    hl = (cur_sprite->col_height & y_lmask) |
     (check_sprite->col_height & y_gmask);
    if((((xg - xl) - wl) & ((yg - yl) - hl)) >> 31)
    {
      // Does it require char/char verification?
      if(check_sprite->flags & (SPRITE_CHAR_CHECK | SPRITE_CHAR_CHECK2))
      {
        // The sub rectangle is going to be somewhere within both sprites;
        // It's going to start at the beginning of the component further
        // along (xg/yg) which is an absolute board position; find the
        // i5 both sprites are from this... and you will iterate in a
        // rectangle there that is (xl + wl) - xg... the difference between
        // where the first one ends and the second one begins
        mw = (xl + wl) - xg;
        mh = (yl + hl) - yg;
        // Reuse these.. i5s. For both you must look at PHYSICAL data, that
        // is, where the chars of the sprite itself is.
        x1 = cur_sprite->ref_x + (xg - x1) + cur_sprite->col_x;
        y1 = cur_sprite->ref_y + (yg - y1) + cur_sprite->col_y;
        x2 = check_sprite->ref_x + (xg - x2) + check_sprite->col_x;
        y2 = check_sprite->ref_y + (yg - y2) + check_sprite->col_y;
        // Check to make sure draw area doesn't go over.
        wg = (cur_sprite->col_width & x_gmask) |
         (check_sprite->col_width & x_lmask);
        hg = (cur_sprite->col_height & y_gmask) |
         (check_sprite->col_height & y_lmask);
  
        if((unsigned int)mw > wg)
        {
          mw = wg;
        }
        if((unsigned int)mh > hg)
        {
          mh = hg;
        }

        // Now iterate through the rect; if both are NOT 32 then a collision is
        // flagged.

        if(cur_sprite->flags & SPRITE_VLAYER)
        {
          use_chars2 = 0;
          i4 = (y1 * vlayer_width) + x1;
          skip = vlayer_width - mw;
        }
        else
        {
          i4 = (y1 * board_width) + x1;
          skip = board_width - mw;
        }

        i5 = (y2 * bwidth) + x2;
        skip2 = bwidth - mw;

        // If ccheck mode 2 is on it should do a further strength check.
        if(check_sprite->flags & SPRITE_CHAR_CHECK2)
        {
          for(i2 = 0; i2 < mh; i2++)
          {
            for(i3 = 0; i3 < mw; i3++)
            {
              char c1, c2;
              if(use_chars2)
              {
                c1 = get_id_char(src_board, i4);
              }
              else
              {
                c1 = vlayer_chars[i4];
              }
              if(use_chars)
              {
                c2 = get_id_char(src_board, i5);
              }
              else
              {
                c2 = vlayer_chars[i5];
              }
              if((c1 != 32) && (c2 != 32))
              {
                // Collision.. maybe.
                // Neither char may be blank
                if(!(is_blank(c1) || is_blank(c2)))
                {
                  collision_list[colliding] = i;
                  colliding++;

                  goto next;
                }
              }
              i4++;
              i5++;
            }
            i4 += skip;
            i5 += skip2;
          }
        }
        else
        {
          for(i2 = 0; i2 < mh; i2++)
          {
            for(i3 = 0; i3 < mw; i3++)
            {
              char c1, c2;
              if(use_chars2)
              {
                c1 = get_id_char(src_board, i4);
              }
              else
              {
                c1 = vlayer_chars[i4];
              }
              if(use_chars)
              {
                c2 = get_id_char(src_board, i5);
              }
              else
              {
                c2 = vlayer_chars[i5];
              }
              if((c1 != 32) && (c2 != 32))
              {
                // Collision!
                collision_list[colliding] = i;
                colliding++;

                goto next;
              }
              i4++;
              i5++;
            }
            i4 += skip;
            i5 += skip;
          }
        }
      }
      else
      {
        collision_list[colliding] = i;
        colliding++;
      }

      next:
      continue;
    }
  }

  mzx_world->collision_count = colliding;
  return colliding;
}

int is_blank(char c)
{
  int cp[4];
  int blank = 0;

  cp[3] = 0;
  ec_read_char(c, (char *)cp);

  blank |= cp[0];
  blank |= cp[1];
  blank |= cp[2];
  blank |= cp[3];

  return !blank;
}


