/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2014 Richard Hughes <richard@hughsie.com>
 * Copyright (C) 2014 Matthias Klumpp <matthias@tenstral.net>
 *
 * Licensed under the GNU Lesser General Public License Version 3
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

/**
 * SECTION:as-release
 * @short_description: Object representing a single upstream release
 * @include: appstream.h
 *
 * This object represents a single upstream release, typically a minor update.
 * Releases can contain a localized description of paragraph and list elements
 * and also have a version number and timestamp.
 *
 * Releases can be automatically generated by parsing upstream ChangeLogs or
 * .spec files, or can be populated using MetaInfo files.
 *
 * See also: #AsComponent
 */

#include "as-release.h"

#include <stdlib.h>

#include "as-utils.h"

typedef struct _AsReleasePrivate	AsReleasePrivate;
struct _AsReleasePrivate
{
	gchar			*version;
	gchar			*description;
	guint64			 timestamp;
};

G_DEFINE_TYPE_WITH_PRIVATE (AsRelease, as_release, G_TYPE_OBJECT)

#define GET_PRIVATE(o) (as_release_get_instance_private (o))

/**
 * as_release_finalize:
 **/
static void
as_release_finalize (GObject *object)
{
	AsRelease *release = AS_RELEASE (object);
	AsReleasePrivate *priv = GET_PRIVATE (release);

	g_free (priv->version);
	g_free (priv->description);

	G_OBJECT_CLASS (as_release_parent_class)->finalize (object);
}

/**
 * as_release_init:
 **/
static void
as_release_init (AsRelease *release)
{
}

/**
 * as_release_class_init:
 **/
static void
as_release_class_init (AsReleaseClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = as_release_finalize;
}

/**
 * as_release_get_version:
 * @release: a #AsRelease instance.
 *
 * Gets the release version.
 *
 * Returns: string, or %NULL for not set or invalid
 **/
const gchar *
as_release_get_version (AsRelease *release)
{
	AsReleasePrivate *priv = GET_PRIVATE (release);
	return priv->version;
}

/**
 * as_release_get_timestamp:
 * @release: a #AsRelease instance.
 *
 * Gets the release timestamp.
 *
 * Returns: timestamp, or 0 for unset
 **/
guint64
as_release_get_timestamp (AsRelease *release)
{
	AsReleasePrivate *priv = GET_PRIVATE (release);
	return priv->timestamp;
}

/**
 * as_release_get_description:
 * @release: a #AsRelease instance.
 *
 * Gets the release description markup for a given locale.
 *
 * Returns: markup, or %NULL for not set or invalid
 **/
const gchar *
as_release_get_description (AsRelease *release)
{
	AsReleasePrivate *priv = GET_PRIVATE (release);
	return priv->description;
}

/**
 * as_release_set_version:
 * @release: a #AsRelease instance.
 * @version: the version string.
 *
 * Sets the release version.
 **/
void
as_release_set_version (AsRelease *release, const gchar *version)
{
	AsReleasePrivate *priv = GET_PRIVATE (release);
	priv->version = g_strdup (version);
}

/**
 * as_release_set_timestamp:
 * @release: a #AsRelease instance.
 * @timestamp: the timestamp value.
 *
 * Sets the release timestamp.
 **/
void
as_release_set_timestamp (AsRelease *release, guint64 timestamp)
{
	AsReleasePrivate *priv = GET_PRIVATE (release);
	priv->timestamp = timestamp;
}

/**
 * as_release_set_description:
 * @release: a #AsRelease instance.
 * @description: the description markup.
 *
 * Sets the description release markup.
 **/
void
as_release_set_description (AsRelease *release, const gchar *description)
{
	AsReleasePrivate *priv = GET_PRIVATE (release);
	priv->description = g_strdup (description);
}

/**
 * as_release_new:
 *
 * Creates a new #AsRelease.
 *
 * Returns: (transfer full): a #AsRelease
 **/
AsRelease *
as_release_new (void)
{
	AsRelease *release;
	release = g_object_new (AS_TYPE_RELEASE, NULL);
	return AS_RELEASE (release);
}
