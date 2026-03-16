#ifndef _GUI_H
#define _GUI_H

#define GUI_PADDING 10

#define HANDLE_SIZE 20
#define HANDLE_MIN_SIZE 6
#define HANDLE_LENGTH_MULTIPLIER 6

#include "editor.h"

#include <raygui.h>

// A set of GUI helper functions
//
Color get_selection_color();

void gui_editor_inspector_flags(Rectangle rect, char *flag_names[], int *flags, int num_flags);

void gui_editor_handles(EditorState *state, Vector2 pos, Vector2 *attribute, Vector2 scale);

void gui_editor_inspector(EditorState *state, Rectangle inspector);
void gui_editor_inspector_player(EditorState *state, Rectangle inspector);
void gui_editor_inspector_entity(EditorState *state, Rectangle inspector);
void gui_editor_inspector_level(EditorState *state, Rectangle inspector);

#endif
