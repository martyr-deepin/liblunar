/*
 * lunar-calendar.c: This file is part of lunar-calendar.
 *
 * Copyright (C) 2009 yetist <yetist@gmail.com>
 * Copyright (C) 2013 Deepin Inc.
 * Copyright (C) 2013 Zhai Xiang <zhaixiang@linuxdeepin.com>
 *
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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <glib/gi18n-lib.h>
#include <gdk/gdk.h>
#include <lunar-date/lunar-date.h>
#include <string.h>
#include <stdlib.h>
#include "lunar-calendar.h"

/**
 * SECTION:lunar-calendar
 * @Short_description: Chinese Lunar Calendar widget for DLTK+
 * @Title: LunarCalendar
 *
 * The #LunarDate provide Chinese lunar Calendar Wieget for DLTK+ .
 */

enum {
	LAST_SIGNAL
};

enum {
	PROP_0,
};

#define LUNAR_CALENDAR_GET_PRIVATE(obj)  (G_TYPE_INSTANCE_GET_PRIVATE((obj), LUNAR_TYPE_CALENDAR, LunarCalendarPrivate))

#define DAY_DETAIL_HOLIDAY_FG_COLOR "#625DDA"

struct _LunarCalendarPrivate
{
	LunarDate   *date;
	GdkColor	*color;
};

static void lunar_calendar_set_property  (GObject          *object,
		guint             prop_id,
		const GValue     *value,
		GParamSpec       *pspec);
static void lunar_calendar_get_property  (GObject          *object,
		guint             prop_id,
		GValue           *value,
		GParamSpec       *pspec);

static void lunar_calendar_month_changed(DLtkCalendar *calendar, 
                                         gpointer user_data);
void  lunar_calendar_day_selected(DLtkCalendar *calendar);
static void lunar_calendar_finalize (GObject *gobject);
static void lunar_calendar_dispose (GObject *gobject);
static void lunar_calendar_init_i18n(void);

static gchar *calendar_detail_cb (DLtkCalendar *gcalendar, 
                                  guint        year, 
                                  guint        month, 
                                  guint        day, 
                                  gpointer     data);

G_DEFINE_TYPE (LunarCalendar, lunar_calendar, DLTK_TYPE_CALENDAR);

static void
lunar_calendar_class_init (LunarCalendarClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);
	DLtkCalendarClass *gcalendar_class = DLTK_CALENDAR_CLASS (class);

	gobject_class->set_property = lunar_calendar_set_property;
	gobject_class->get_property = lunar_calendar_get_property;
    /* TODO: yetist, no comenting destruction, otherwise memory leak :( */
    gobject_class->dispose = lunar_calendar_dispose;
	gobject_class->finalize = lunar_calendar_finalize;
	gcalendar_class->day_selected = lunar_calendar_day_selected;

	g_type_class_add_private (class, sizeof (LunarCalendarPrivate));
}

static void
lunar_calendar_init (LunarCalendar *calendar)
{
	LunarCalendarPrivate *priv;

	priv = LUNAR_CALENDAR_GET_PRIVATE (calendar);
	priv->date = lunar_date_new();
	priv->color = g_new0(GdkColor, 1);
	priv->color->red = 0x0;
	priv->color->green = 0x0;
	priv->color->blue = 0xffff;

	/* FIXME: here we can setup the locale info, 
     *        but it looks like not a good idea 
     *        
     * TODO: but yetist, come on, it is not the issue :)
     */
	lunar_calendar_init_i18n();

	if (dltk_calendar_get_display_options(
        DLTK_CALENDAR(calendar)) & DLTK_CALENDAR_SHOW_DETAILS) {
		dltk_calendar_set_detail_func(
            DLTK_CALENDAR(calendar), calendar_detail_cb, calendar, NULL);
    }
}

static void
lunar_calendar_finalize (GObject *gobject)
{
	LunarCalendar *calendar;

	calendar = LUNAR_CALENDAR(gobject);
	lunar_date_free(calendar->priv->date);
	gdk_color_free(calendar->priv->color);

	G_OBJECT_CLASS(lunar_calendar_parent_class)->finalize(gobject);
}

static void lunar_calendar_dispose(GObject *gobject)
{
	LunarCalendar *calendar;

	calendar = LUNAR_CALENDAR(gobject);
    if (!calendar) 
        return;

    /* FIXME: yetist, why priv is NULL, i will check it ASAP :) */
    if (!calendar->priv) 
        return;

    if (calendar->priv->date) {
        g_object_unref(calendar->priv->date);
        calendar->priv->date = NULL;
    }
    if (calendar->priv->color) {
        g_object_unref(calendar->priv->color);
	    calendar->priv->color = NULL;
	}

    G_OBJECT_CLASS (lunar_calendar_parent_class)->dispose(gobject);
}

static void
lunar_calendar_set_property (GObject      *object,
		guint         prop_id,
		const GValue *value,
		GParamSpec   *pspec)
{
	LunarCalendar *calendar;

	calendar = LUNAR_CALENDAR (object);

	switch (prop_id)
	{

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
lunar_calendar_get_property (GObject      *object,
		guint         prop_id,
		GValue       *value,
		GParamSpec   *pspec)
{
	LunarCalendar *calendar;

	calendar = LUNAR_CALENDAR (object);

	switch (prop_id)
	{

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/**
 * lunar_calendar_new:
 * 
 * Creates a new lunar calendar, with the current date being selected. 
 * 
 * Return value: a newly #LunarCalendar widget
 **/
GtkWidget*
lunar_calendar_new (void)
{
	return g_object_new (LUNAR_TYPE_CALENDAR, NULL);
}

/**
 * lunar_calendar_set_jieri_color:
 * @lunar: a #LunarCalendar
 * @color: the holiday color.
 *
 * Setup the holiday(jieri) color.
 *
 **/
void		lunar_calendar_set_jieri_color		(LunarCalendar *lunar, const GdkColor *color)
{
	LunarCalendarPrivate *priv = LUNAR_CALENDAR_GET_PRIVATE (lunar);
	if (gdk_color_equal(priv->color, color))
		return;
	priv->color = gdk_color_copy(color);
	gtk_widget_queue_draw(GTK_WIDGET(lunar));
}

void  lunar_calendar_day_selected(DLtkCalendar *calendar)
{
	guint year, month, day;
	LunarDate *lunar;
	GError *error = NULL;

	if (getenv("LUNAR_CALENDAR_IGNORE_NON_CHINESE") != NULL)
	{
		const gchar* const * langs =  g_get_language_names();

		if (langs[0] && langs[0][0] != '\0')
			if (!g_str_has_prefix(langs[0], "zh_CN"))
				g_object_set (calendar, "show-details", FALSE, NULL);
	}

	LunarCalendarPrivate *priv = LUNAR_CALENDAR_GET_PRIVATE (calendar);
	dltk_calendar_get_date(calendar, &year, &month, &day);
	lunar_date_set_solar_date(priv->date, year, month + 1, day, 0, &error);
	char *jieri = lunar_date_get_jieri(priv->date, "\n");
	char *format = g_strdup_printf(_("%(year)-%(month)-%(day)\nLunar:%(YUE)Month%(RI)Day\nGanzhi:%(Y60)Year%(M60)Month%(D60)Day\nBazi:%(Y8)Year%(M8)Month%(D8)Day%(H8)Hour\nShengxiao:%(shengxiao)\n<span foreground=\"#6481c8\">%s</span>\n"), jieri);
	char *strtime = lunar_date_strftime(priv->date, format);
	gtk_widget_set_tooltip_markup(GTK_WIDGET(calendar), strtime);
	g_free(format);
	g_free(jieri);
	g_free(strtime);
}

static gchar *calendar_detail_cb(DLtkCalendar *gcalendar, 
                                 guint        year, 
                                 guint        month, 
                                 guint        day, 
                                 gpointer     data)
{
	GError *error = NULL;
	LunarCalendar *calendar = LUNAR_CALENDAR(data);
	LunarCalendarPrivate *priv = LUNAR_CALENDAR_GET_PRIVATE (calendar);
	gboolean show_detail;
	g_object_get(calendar, "show-details", &show_detail, NULL);
	if (!show_detail)
		return NULL;

	lunar_date_set_solar_date(priv->date, year, month + 1, day, 0, &error);
	if (error)
		return NULL;

	if (getenv("LUNAR_CALENDAR_IGNORE_NON_CHINESE")) {
		const gchar* const * langs =  g_get_language_names();

		if (langs[0] && langs[0][0] != '\0') {
			if (!g_str_has_prefix(langs[0], "zh_")) {
				g_object_set (calendar, "show-details", FALSE, NULL);
				return NULL;
			}
        }
	}

	char *buf = NULL;
	gchar *val = NULL;

	if (strlen(buf = lunar_date_strftime(priv->date, "%(jieri)")) > 0) {
		gchar* col = gdk_color_to_string(priv->color);
		val = g_strconcat("<span foreground=\"", 
                          DAY_DETAIL_HOLIDAY_FG_COLOR, 
                          "\">", 
                          buf, 
                          "</span>", 
                          NULL);
		g_free(col);
		g_free(buf);
		return val;
	}
	if (strcmp(buf = lunar_date_strftime(priv->date, "%(ri)"), "1") == 0) {
		g_free(buf);
		return lunar_date_strftime(priv->date, _("%(YUE)Yue"));
	}
	else
		return  lunar_date_strftime(priv->date, "%(RI)");
}

static void lunar_calendar_init_i18n(void)
{
  static gboolean _lunar_calendar_gettext_initialized = FALSE;

  if (!_lunar_calendar_gettext_initialized)
    {
	  bindtextdomain (GETTEXT_PACKAGE, LUNAR_CALENDAR_LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
      bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
      _lunar_calendar_gettext_initialized = TRUE;
    }
}
