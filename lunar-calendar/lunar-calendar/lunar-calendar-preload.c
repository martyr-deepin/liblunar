/* vi: set sw=4 ts=4: */
/*
 * liblunar-gtk-preload.c: This file is part of liblunar-gtk.
 *
 * Copyright (C) 2009 yetist <yetist@gmail.com>
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
 * */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <dtk/dtk.h>
#include <lunar-calendar/lunar-calendar.h>

GtkWidget* dtk_calendar_new (void)
{
	GtkWidget* calendar;
	DtkCalendarDisplayOptions flags;
	calendar = lunar_calendar_new();
	flags = dtk_calendar_get_display_options(DTK_CALENDAR(calendar));
	flags |= DTK_CALENDAR_SHOW_DETAILS;
	dtk_calendar_set_display_options(DTK_CALENDAR(calendar), flags);
	return calendar;
}

void  dtk_calendar_set_display_options (DtkCalendar *calendar, DtkCalendarDisplayOptions flags)
{
	void *handle;
	char *error;
	void  (* _dtk_calendar_set_display_options) (DtkCalendar *calendar, DtkCalendarDisplayOptions flags) = NULL;

	handle = dlopen("/usr/lib/libdtk-2.0.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	dlerror();

	flags |= DTK_CALENDAR_SHOW_DETAILS;
	*(void **) (&_dtk_calendar_set_display_options) = dlsym(handle, "dtk_calendar_display_options");

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}
	dlclose(handle);

	(*_dtk_calendar_set_display_options) (calendar, flags);
}

void  dtk_calendar_display_options (DtkCalendar *calendar, DtkCalendarDisplayOptions flags)
{
	dtk_calendar_set_display_options (calendar, flags);
}

gpointer g_object_newv (GType object_type, guint n_parameters, GParameter *parameters)
{
	void *handle;
	char *error;
	gpointer (* _g_object_newv) (GType object_type, guint n_parameters, GParameter *parameters) = NULL;

	handle = dlopen("/usr/lib/libgobject-2.0.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	dlerror();

	*(void **) (&_g_object_newv) = dlsym(handle, "g_object_newv");

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}
	dlclose(handle);

	if (g_type_is_a(object_type, g_type_from_name("DtkCalendar")))
		object_type = lunar_calendar_get_type();
	return (*_g_object_newv) (object_type, n_parameters, parameters);
}
/*
vi:ts=4:wrap:ai:
*/
