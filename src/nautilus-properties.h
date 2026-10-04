
/*
 * SPDX-FileCopyrightText: 2000 Eazel, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Authors: Darin Adler <darin@bentspoon.com>
 */

#pragma once

#include "nautilus-types.h"

#include <gtk/gtk.h>

GtkWindow *
nautilus_properties_present_window (NautilusFileList *files,
                                    const char       *startup_id);
