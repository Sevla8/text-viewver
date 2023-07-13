/* text_viewer1-window.h
 *
 * Copyright 2022 Brandon Alves
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define TEXT_VIEWER1_TYPE_WINDOW (text_viewer1_window_get_type())

G_DECLARE_FINAL_TYPE (TextViewer1Window, text_viewer1_window, TEXT_VIEWER1, WINDOW, GtkApplicationWindow)

G_END_DECLS
