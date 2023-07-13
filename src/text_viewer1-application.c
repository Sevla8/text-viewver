/* text_viewer1-application.c
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

#include "text_viewer1-application.h"
#include "text_viewer1-window.h"

struct _TextViewer1Application
{
  GtkApplication parent_instance;
};

G_DEFINE_TYPE (TextViewer1Application, text_viewer1_application, ADW_TYPE_APPLICATION)

TextViewer1Application *
text_viewer1_application_new (gchar *application_id,
                              GApplicationFlags  flags)
{
  return g_object_new (TEXT_VIEWER1_TYPE_APPLICATION,
                       "application-id", application_id,
                       "flags", flags,
                       NULL);
}

static void
text_viewer1_application_finalize (GObject *object)
{
  TextViewer1Application *self = (TextViewer1Application *)object;

  G_OBJECT_CLASS (text_viewer1_application_parent_class)->finalize (object);
}

static void
text_viewer1_application_activate (GApplication *app)
{
  GtkWindow *window;

  /* It's good practice to check your parameters at the beginning of the
   * function. It helps catch errors early and in development instead of
   * by your users.
   */
  g_assert (GTK_IS_APPLICATION (app));

  /* Get the current window or create one if necessary. */
  window = gtk_application_get_active_window (GTK_APPLICATION (app));
  if (window == NULL)
    window = g_object_new (TEXT_VIEWER1_TYPE_WINDOW,
                           "application", app,
                           NULL);

  /* Ask the window manager/compositor to present the window. */
  gtk_window_present (window);
}


static void
text_viewer1_application_class_init (TextViewer1ApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  object_class->finalize = text_viewer1_application_finalize;

  /*
   * We connect to the activate callback to create a window when the application
   * has been launched. Additionally, this callback notifies us when the user
   * tries to launch a "second instance" of the application. When they try
   * to do that, we'll just present any existing window.
   */
  app_class->activate = text_viewer1_application_activate;
}

static void
text_viewer1_application_show_about (GSimpleAction *action,
                                     GVariant      *parameter,
                                     gpointer       user_data)
{
  TextViewer1Application *self = TEXT_VIEWER1_APPLICATION (user_data);
  GtkWindow *window = NULL;
  const gchar *authors[] = {"Brandon Alves", NULL};

  g_return_if_fail (TEXT_VIEWER1_IS_APPLICATION (self));

  window = gtk_application_get_active_window (GTK_APPLICATION (self));

  gtk_show_about_dialog (window,
                         "program-name", "text-viewer1",
                         "authors", authors,
                         "version", "0.1.0",
                         NULL);
}


static void
text_viewer1_application_init (TextViewer1Application *self)
{
  g_autoptr (GSimpleAction) quit_action = g_simple_action_new ("quit", NULL);
  g_signal_connect_swapped (quit_action, "activate", G_CALLBACK (g_application_quit), self);
  g_action_map_add_action (G_ACTION_MAP (self), G_ACTION (quit_action));

  g_autoptr (GSimpleAction) about_action = g_simple_action_new ("about", NULL);
  g_signal_connect (about_action, "activate", G_CALLBACK (text_viewer1_application_show_about), self);
  g_action_map_add_action (G_ACTION_MAP (self), G_ACTION (about_action));

  gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                         "app.quit",
                                         (const char *[]) {
                                           "<primary>q",
                                           NULL,
                                         });

  gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                         "win.open",
                                         (const char *[]) {
                                           "<Ctrl>o",
                                           NULL,
                                         });
}
