/* text_viewer1-window.c
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

#include "text_viewer1-config.h"
#include "text_viewer1-window.h"

struct _TextViewer1Window
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkHeaderBar        *header_bar;
  GtkTextView         *main_text_view;
  GtkButton           *open_button;
  GtkLabel            *cursor_pos;
};

G_DEFINE_TYPE (TextViewer1Window, text_viewer1_window, GTK_TYPE_APPLICATION_WINDOW)

static void
text_viewer1_window_class_init (TextViewer1WindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/com/example/TextViewer/text_viewer1-window.ui");
  gtk_widget_class_bind_template_child (widget_class, TextViewer1Window, header_bar);
  gtk_widget_class_bind_template_child (widget_class, TextViewer1Window, main_text_view);
  gtk_widget_class_bind_template_child (widget_class, TextViewer1Window, open_button);
  gtk_widget_class_bind_template_child (widget_class, TextViewer1Window, cursor_pos);
}

static void
open_file_complete (GObject          *source_object,
                    GAsyncResult     *result,
                    TextViewer1Window *self)
{
  GFile *file = G_FILE (source_object);

  g_autofree char *contents = NULL;
  gsize length = 0;

  g_autoptr (GError) error = NULL;

  // Complete the asynchronous operation; this function will either
  // give you the contents of the file as a byte array, or will
  // set the error argument
  g_file_load_contents_finish (file,
                               result,
                               &contents,
                               &length,
                               NULL,
                               &error);

  // Query the display name for the file
  g_autofree char *display_name = NULL;
  g_autoptr (GFileInfo) info =
    g_file_query_info (file,
                       "standard::display-name",
                       G_FILE_QUERY_INFO_NONE,
                       NULL,
                       NULL);

  if (info != NULL)
    {
      display_name =
        g_strdup (g_file_info_get_attribute_string (info, "standard::display-name"));
    }
  else
    {
      display_name = g_file_get_basename (file);
    }

  // In case of error, print a warning to the standard error output
  if (error != NULL)
    {
      g_printerr ("Unable to open “%s”: %s\n",
                  g_file_peek_path (file),
                  error->message);
      return;
    }

  // Ensure that the file is encoded with UTF-8
  if (!g_utf8_validate (contents, length, NULL))
    {
      g_printerr ("Unable to load the contents of “%s”: "
                  "the file is not encoded with UTF-8\n",
                  g_file_peek_path (file));
      return;
    }

  // Retrieve the GtkTextBuffer instance that stores the
  // text displayed by the GtkTextView widget
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (self->main_text_view);

  // Set the text using the contents of the file
  gtk_text_buffer_set_text (buffer, contents, length);

  // Reposition the cursor so it's at the start of the text
  GtkTextIter start;
  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_place_cursor (buffer, &start);

  // Set the title using the display name
  gtk_window_set_title (GTK_WINDOW (self), display_name);
 }

static void
open_file (TextViewer1Window *self,
           GFile            *file)
{
  g_file_load_contents_async (file,
                              NULL,
                              (GAsyncReadyCallback) open_file_complete,
                              self);
}

static void
on_open_response (GtkNativeDialog  *native,
                  int               response,
                  TextViewer1Window *self)
{
  // If the user selected a file...
  if (response == GTK_RESPONSE_ACCEPT)
    {
      GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);

      // ... retrieve the location from the dialog...
      g_autoptr (GFile) file = gtk_file_chooser_get_file (chooser);

      // ... and open it
      open_file (self, file);
    }

  // Release the reference on the file selection dialog now that we
  // do not need it any more
  g_object_unref (native);
}

static void
text_viewer1_window__open_file_dialog (GAction          *action G_GNUC_UNUSED,
                                      GVariant         *parameter G_GNUC_UNUSED,
                                      TextViewer1Window *self)
{
  // Create a new file selection dialog, using the "open" mode
  GtkFileChooserNative *native =
    gtk_file_chooser_native_new ("Open File",
                                 GTK_WINDOW (self),
                                 GTK_FILE_CHOOSER_ACTION_OPEN,
                                 "_Open",
                                 "_Cancel");

  // Connect the "response" signal of the file selection dialog;
  // this signal is emitted when the user selects a file, or when
  // they cancel the operation
  g_signal_connect (native,
                    "response",
                    G_CALLBACK (on_open_response),
                    self);

  // Present the dialog to the user
  gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
}

static void
text_viewer1_window__update_cursor_position (GtkTextBuffer    *buffer,
                                            GParamSpec       *pspec G_GNUC_UNUSED,
                                            TextViewer1Window *self)
{
  int cursor_pos = 0;

  // Retrieve the value of the "cursor-position" property
  g_object_get (buffer, "cursor-position", &cursor_pos, NULL);

  // Construct the text iterator for the position of the cursor
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_offset (buffer, &iter, cursor_pos);

  // Set the new contents of the label
  g_autofree char *cursor_str =
    g_strdup_printf ("Ln %d, Col %d",
                     gtk_text_iter_get_line (&iter) + 1,
                     gtk_text_iter_get_line_offset (&iter) + 1);

  gtk_label_set_text (self->cursor_pos, cursor_str);
}

static void
text_viewer1_window_init (TextViewer1Window *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  g_autoptr (GSimpleAction) open_action =
    g_simple_action_new ("open", NULL);
  g_signal_connect (open_action,
                    "activate",
                    G_CALLBACK (text_viewer1_window__open_file_dialog),
                    self);
  g_action_map_add_action (G_ACTION_MAP (self),
                           G_ACTION (open_action));

  GtkTextBuffer *buffer = gtk_text_view_get_buffer (self->main_text_view);
  g_signal_connect (buffer,
                    "notify::cursor-position",
                    G_CALLBACK (text_viewer1_window__update_cursor_position),
                    self);
}

