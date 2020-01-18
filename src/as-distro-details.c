/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2012-2020 Matthias Klumpp <matthias@tenstral.net>
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the license, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "as-distro-details-private.h"

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>
#include <gio/gio.h>

#include "as-settings-private.h"
#include "as-utils-private.h"

/**
 * SECTION:as-distro-details
 * @short_description: Provides information about the current distribution
 * @include: appstream.h
 *
 * This object abstracts various distribution-specific settings and provides information
 * about the (Linux) distribution which is currently in use.
 * It is used internalls to get information about the icon-store or the 3rd-party screenshot
 * service distributors may want to provide.
 *
 * See also: #AsPool
 */

typedef struct
{
	gchar *id;
	gchar *cid;
	gchar *name;
	gchar *version;
	gchar *homepage;

	GKeyFile* keyf;
} AsDistroDetailsPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AsDistroDetails, as_distro_details, G_TYPE_OBJECT)
#define GET_PRIVATE(o) (as_distro_details_get_instance_private (o))

enum  {
	AS_DISTRO_DETAILS_DUMMY_PROPERTY,
	AS_DISTRO_DETAILS_ID,
	AS_DISTRO_DETAILS_NAME,
	AS_DISTRO_DETAILS_VERSION,
	AS_DISTRO_DETAILS_HOMEPAGE
};

/**
 * as_distro_details_init:
 **/
static void
as_distro_details_init (AsDistroDetails *distro)
{
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);

	priv->id = g_strdup ("unknown");
	as_distro_details_load_data (distro, "/etc/os-release", AS_CONFIG_NAME);
}

/**
 * as_distro_details_finalize:
 **/
static void
as_distro_details_finalize (GObject *object)
{
	AsDistroDetails *distro = AS_DISTRO_DETAILS (object);
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);

	g_free (priv->id);
	g_free (priv->name);
	g_free (priv->version);
	g_free (priv->homepage);
	g_key_file_unref (priv->keyf);

	G_OBJECT_CLASS (as_distro_details_parent_class)->finalize (object);
}

/**
 * as_distro_details_load_data:
 */
void
as_distro_details_load_data (AsDistroDetails *distro, const gchar *os_release_fname, const gchar *as_config_fname)
{
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);
	g_autoptr(GFile) f = NULL;
	g_autoptr(GError) error = NULL;
	gchar *line;

	/* load configuration */
	if (priv->keyf != NULL)
		g_key_file_unref (priv->keyf);

	priv->keyf = g_key_file_new ();
	if (as_config_fname != NULL)
		g_key_file_load_from_file (priv->keyf, as_config_fname, G_KEY_FILE_NONE, NULL);

	/* get details about the distribution we are running on */
	f = g_file_new_for_path (os_release_fname);
	if (g_file_query_exists (f, NULL)) {
		g_autoptr(GDataInputStream) dis = NULL;
		g_autoptr(GFileInputStream) fis = NULL;

		fis = g_file_read (f, NULL, &error);
		if (error != NULL) {
			g_warning ("Unable to read %s file.", os_release_fname);
			return;
		}
		dis = g_data_input_stream_new ((GInputStream*) fis);

		while ((line = g_data_input_stream_read_line (dis, NULL, NULL, &error)) != NULL) {
			g_auto(GStrv) data = NULL;
			g_autofree gchar *dvalue = NULL;
			if (error != NULL) {
				g_warning ("Unable to read line in %s file.", os_release_fname);
				g_free (line);
				return;
			}

			data = g_strsplit (line, "=", 2);
			if (g_strv_length (data) != 2) {
				g_free (line);
				continue;
			}

			dvalue = g_strdup (data[1]);
			if (g_str_has_prefix (dvalue, "\"")) {
				gchar *tmp;
				tmp = g_strndup (dvalue + 1, strlen(dvalue) - 2);
				g_free (dvalue);
				dvalue = tmp;
			}

			if (g_strcmp0 (data[0], "ID") == 0) {
				g_free (priv->id);
				priv->id = g_steal_pointer (&dvalue);
				g_object_notify ((GObject *) distro, "id");

			} else if (g_strcmp0 (data[0], "NAME") == 0) {
				g_free (priv->name);
				priv->name = g_steal_pointer (&dvalue);
				g_object_notify ((GObject *) distro, "name");

			} else if (g_strcmp0 (data[0], "VERSION_ID") == 0) {
				g_free (priv->version);
				priv->version = g_steal_pointer (&dvalue);
				g_object_notify ((GObject *) distro, "version");

			} else if (g_strcmp0 (data[0], "HOME_URL") == 0) {
				g_free (priv->homepage);
				priv->homepage = g_steal_pointer (&dvalue);
				g_object_notify ((GObject *) distro, "homepage");
			}

			g_free (line);
		}
	}
}

/**
 * as_distro_details_get_str:
 */
gchar*
as_distro_details_get_str (AsDistroDetails *distro, const gchar *key)
{
	gchar *value;
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);

	g_return_val_if_fail (key != NULL, NULL);

	value = g_key_file_get_string (priv->keyf, priv->id, key, NULL);
	return value;
}

/**
 * as_distro_details_get_bool:
 */
gboolean
as_distro_details_get_bool (AsDistroDetails *distro, const gchar *key, gboolean default_val)
{
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);
	gboolean value;
	g_autoptr(GError) error = NULL;

	g_return_val_if_fail (key != NULL, FALSE);

	value = g_key_file_get_boolean (priv->keyf, "general", key, &error);
	if (error != NULL) {
		g_error_free (error);
		error = NULL;

		value = g_key_file_get_boolean (priv->keyf, priv->id, key, &error);
		if (error != NULL)
			return default_val;
	}

	return value;
}

/**
 * as_distro_details_get_id:
 */
const gchar*
as_distro_details_get_id (AsDistroDetails *distro)
{
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);
	return priv->id;
}

/**
 * as_distro_details_get_cid:
 */
const gchar*
as_distro_details_get_cid (AsDistroDetails *distro)
{
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);

	if (priv->cid != NULL)
		return priv->cid;
	if (priv->homepage == NULL) {
		priv->cid = g_strdup (priv->id);
		return priv->cid;
	}

	priv->cid = as_utils_dns_to_rdns (priv->homepage, priv->id);
	if (priv->cid == NULL)
		return priv->id;
	return priv->cid;
}

/**
 * as_distro_details_get_name:
 */
const gchar*
as_distro_details_get_name (AsDistroDetails *distro)
{
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);
	return priv->name;
}

/**
 * as_distro_details_get_version:
 */
const gchar*
as_distro_details_get_version (AsDistroDetails *distro)
{
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);
	return priv->version;
}

/**
 * as_distro_details_get_homepage:
 */
const gchar*
as_distro_details_get_homepage (AsDistroDetails *distro)
{
	AsDistroDetailsPrivate *priv = GET_PRIVATE (distro);
	return priv->homepage;
}

/**
 * as_distro_details_get_property:
 */
static void
as_distro_details_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	AsDistroDetails  *distro;
	distro = G_TYPE_CHECK_INSTANCE_CAST (object, AS_TYPE_DISTRO_DETAILS, AsDistroDetails);
	switch (property_id) {
		case AS_DISTRO_DETAILS_ID:
			g_value_set_string (value, as_distro_details_get_id (distro));
			break;
		case AS_DISTRO_DETAILS_NAME:
			g_value_set_string (value, as_distro_details_get_name (distro));
			break;
		case AS_DISTRO_DETAILS_VERSION:
			g_value_set_string (value, as_distro_details_get_version (distro));
			break;
		case AS_DISTRO_DETAILS_HOMEPAGE:
			g_value_set_string (value, as_distro_details_get_homepage (distro));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

/**
 * as_distro_details_class_init:
 **/
static void
as_distro_details_class_init (AsDistroDetailsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = as_distro_details_finalize;
	object_class->get_property = as_distro_details_get_property;

	g_object_class_install_property (object_class,
						AS_DISTRO_DETAILS_ID,
						g_param_spec_string ("id", "id", "id", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	g_object_class_install_property (object_class,
						AS_DISTRO_DETAILS_NAME,
						g_param_spec_string ("name", "name", "name", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	g_object_class_install_property (object_class,
						AS_DISTRO_DETAILS_VERSION,
						g_param_spec_string ("version", "version", "version", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	g_object_class_install_property (object_class,
						AS_DISTRO_DETAILS_HOMEPAGE,
						g_param_spec_string ("homepage", "homepage", "homepage", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
}

/**
 * as_distro_details_new:
 *
 * Creates a new instance of #AsDistroDetails.
 *
 * Returns: (transfer full): a #AsDistroDetails.
 **/
AsDistroDetails*
as_distro_details_new (void)
{
	AsDistroDetails *distro;
	distro = g_object_new (AS_TYPE_DISTRO_DETAILS, NULL);
	return AS_DISTRO_DETAILS (distro);
}


/**
 * as_get_current_distro_component_id:
 *
 * Returns the component-ID of the current distribution based on contents
 * of the `/etc/os-release` file.
 * This function is a shorthand for %as_distro_details_get_cid
 */
gchar*
as_get_current_distro_component_id (void)
{
	g_autoptr(AsDistroDetails) distro = as_distro_details_new ();
	return g_strdup (as_distro_details_get_cid (distro));

}
