/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2016-2021 Matthias Klumpp <matthias@tenstral.net>
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

/**
 * SECTION:asc-globals
 * @short_description: Set and read a bunch of global settings used across appstream-compose
 * @include: appstream-compose.h
 */

#include "config.h"
#include "asc-globals-private.h"

#include "as-utils-private.h"
#include "as-validator-issue-tag.h"
#include "asc-resources.h"

#define ASC_TYPE_GLOBALS (asc_globals_get_type ())
G_DECLARE_DERIVABLE_TYPE (AscGlobals, asc_globals, ASC, GLOBALS, GObject)

struct _AscGlobalsClass
{
	GObjectClass parent_class;
};

typedef struct
{
	gboolean	use_optipng;
	gchar		*optipng_bin;
	gchar		*tmp_dir;

	GMutex		pangrams_mutex;
	GPtrArray	*pangrams_en;

	GMutex		hint_tags_mutex;
	GHashTable	*hint_tags;
} AscGlobalsPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AscGlobals, asc_globals, G_TYPE_OBJECT)
#define GET_PRIVATE(o) (asc_globals_get_instance_private (o))

static AscGlobals *g_globals = NULL;

/**
 * asc_compose_error_quark:
 *
 * Return value: An error quark.
 **/
GQuark
asc_compose_error_quark (void)
{
	static GQuark quark = 0;
	if (!quark)
		quark = g_quark_from_static_string ("AscComposeError");
	return quark;
}

static GObject*
asc_globals_constructor (GType type, guint n_construct_properties, GObjectConstructParam *construct_properties)
{
	if (g_globals)
		return g_object_ref (G_OBJECT (g_globals));
	else
		return G_OBJECT_CLASS (asc_globals_parent_class)->constructor (type, n_construct_properties, construct_properties);
}

static void
asc_globals_finalize (GObject *object)
{
	AscGlobals *globals = ASC_GLOBALS (object);
	AscGlobalsPrivate *priv = GET_PRIVATE (globals);

	g_free (priv->tmp_dir);
	g_free (priv->optipng_bin);

	g_mutex_clear (&priv->pangrams_mutex);
	if (priv->pangrams_en != NULL)
		g_ptr_array_unref (priv->pangrams_en);

	g_mutex_clear (&priv->hint_tags_mutex);
	if (priv->hint_tags != NULL)
		g_hash_table_unref (priv->hint_tags);

	G_OBJECT_CLASS (asc_globals_parent_class)->finalize (object);
}

static void
asc_globals_init (AscGlobals *globals)
{
	AscGlobalsPrivate *priv = GET_PRIVATE (globals);
	g_assert (g_globals == NULL);
	g_globals = globals;

	priv->tmp_dir = g_build_filename (g_get_tmp_dir (), "as-compose", NULL);

	priv->optipng_bin = g_find_program_in_path ("optipng");
	if (priv->optipng_bin != NULL)
		priv->use_optipng = TRUE;

	g_mutex_init (&priv->pangrams_mutex);
}

static void
asc_globals_class_init (AscGlobalsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->constructor = asc_globals_constructor;
	object_class->finalize = asc_globals_finalize;
}

static AscGlobalsPrivate*
asc_globals_get_priv (void)
{
	return GET_PRIVATE (g_object_new (ASC_TYPE_GLOBALS, NULL));
}

/**
 * asc_globals_get_tmp_dir:
 *
 * Get temporary directory used by appstream-compose.
 **/
const gchar*
asc_globals_get_tmp_dir (void)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	return priv->tmp_dir;
}

/**
 * asc_globals_get_tmp_dir_create:
 *
 * Get temporary directory used by appstream-compose
 * and try to create it if it does not exist.
 **/
const gchar*
asc_globals_get_tmp_dir_create (void)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	g_mkdir_with_parents (priv->tmp_dir, 0700);
	return priv->tmp_dir;
}

/**
 * asc_globals_set_tmp_dir:
 *
 * Set temporary directory used by appstream-compose.
 **/
void
asc_globals_set_tmp_dir (const gchar *path)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	g_free (priv->tmp_dir);
	priv->tmp_dir = g_strdup (path);
}

/**
 * asc_globals_get_use_optipng:
 *
 * Get whether images should be optimized using optipng.
 **/
gboolean
asc_globals_get_use_optipng (void)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	return priv->use_optipng;
}

/**
 * asc_globals_set_use_optipng:
 *
 * Set whether images should be optimized using optipng.
 **/
void
asc_globals_set_use_optipng (gboolean enabled)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	if (enabled && priv->optipng_bin == NULL) {
		g_critical ("Refusing to enable optipng: not found in $PATH");
		priv->use_optipng = FALSE;
		return;
	}
	priv->use_optipng = enabled;
}

/**
 * asc_globals_get_optipng_binary:
 *
 * Get path to the "optipng" binary we should use.
 **/
const gchar*
asc_globals_get_optipng_binary (void)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	return priv->optipng_bin;
}

/**
 * asc_globals_set_optipng_binary:
 *
 * Set path to the "optipng" binary we should use.
 **/
void
asc_globals_set_optipng_binary (const gchar *path)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	g_free (priv->optipng_bin);
	priv->optipng_bin = g_strdup (path);
	if (priv->optipng_bin == NULL)
		priv->use_optipng = FALSE;
}

/**
 * asc_globals_get_pangrams_for:
 *
 * Obtain a list of pangrams for the given language, currently
 * only "en" is supported.
 *
 * Returns: (transfer none) (element-type utf8): List of pangrams.
 */
GPtrArray*
asc_globals_get_pangrams_for (const gchar *lang)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	if ((lang != NULL) && (g_strcmp0 (lang, "en") != 0))
		return NULL;

	/* return cached value if possible */
	if (priv->pangrams_en != NULL)
		return priv->pangrams_en;

	{
		g_autoptr(GBytes) data = NULL;
		g_auto(GStrv) strv = NULL;
		g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&priv->pangrams_mutex);
		/* race protection, just to be sure */
		if (priv->pangrams_en != NULL)
			return priv->pangrams_en;

		/* load array from resources */
		data = g_resource_lookup_data (asc_get_resource (),
						"/org/freedesktop/appstream-compose/pangrams/en.txt",
						G_RESOURCE_LOOKUP_FLAGS_NONE,
						NULL);
		if (data == NULL)
			return NULL;

		strv = g_strsplit (g_bytes_get_data (data, NULL), "\n", -1);
		if (strv == NULL)
			return NULL;
		priv->pangrams_en = g_ptr_array_new_full(g_strv_length (strv), g_free);
		for (guint i = 0; strv[i] != NULL; i++)
			g_ptr_array_add (priv->pangrams_en, g_strdup (strv[i]));
	}

	return priv->pangrams_en;
}

/**
 * asc_globals_create_hint_tag_table:
 *
 * Create hint tag cache.
 * IMPORTANT: This function may only be called while a lock is held
 * on the hint tag table mutex.
 */
static void
asc_globals_create_hint_tag_table ()
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	g_return_if_fail (priv->hint_tags == NULL);

	priv->hint_tags = g_hash_table_new_full (g_str_hash,
						 g_str_equal,
						 (GDestroyNotify) as_ref_string_release,
						 (GDestroyNotify) asc_hint_tag_free);

	/* add compose issue hint tags */
	for (guint i = 0; asc_hint_tag_list[i].tag != NULL; i++) {
		AscHintTag *htag;
		const AscHintTagStatic s = asc_hint_tag_list[i];
		htag = asc_hint_tag_new (s.tag, s.severity, s.explanation);
		gboolean r = g_hash_table_insert (priv->hint_tags,
							g_ref_string_new_intern (asc_hint_tag_list[i].tag),
							htag);
		if (G_UNLIKELY (!r))
			g_critical ("Duplicate compose-hint tag '%s' found in tag list. This is a bug in appstream-compose.", asc_hint_tag_list[i].tag);
	}

	/* add validator issue hint tags */
	for (guint i = 0; as_validator_issue_tag_list[i].tag != NULL; i++) {
		AscHintTag *htag;
		gboolean r;
		AsIssueSeverity severity;
		g_autofree gchar *compose_tag = g_strconcat ("asv-", as_validator_issue_tag_list[i].tag, NULL);
		g_autofree gchar *explanation = g_markup_printf_escaped ("<code>{{location}}</code> - <em>{{hint}}</em><br/>%s",
									 as_validator_issue_tag_list[i].explanation);

		/* any validator issue can not be of type error in as-compose - if the validation issue
		 * is so severe that it renders the compose process impossible, we will throw another issue
		 * of type "error" which will immediately terminate the data genertaion. */
		severity = as_validator_issue_tag_list[i].severity;
		if (severity == AS_ISSUE_SEVERITY_ERROR)
			severity = AS_ISSUE_SEVERITY_WARNING;

		htag = asc_hint_tag_new (compose_tag, severity, explanation);
		r = g_hash_table_insert (priv->hint_tags,
					 g_ref_string_new_intern (compose_tag),
					 htag);
		if (G_UNLIKELY (!r))
			g_critical ("Duplicate issue-tag '%s' found in tag list. This is a bug in appstream-compose.", as_validator_issue_tag_list[i].tag);
	}
}

/**
 * asc_globals_add_hint_tag:
 *
 * Register a new hint tag. If a previous tag with the given name
 * already existed, the existing tag will not be replaced.
 *
 * Returns: %TRUE if the tag was registered and did not exist previously.
 */
gboolean
asc_globals_add_hint_tag (const gchar *tag, AsIssueSeverity severity, const gchar *explanation)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	AscHintTag *htag;
	g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&priv->hint_tags_mutex);
	g_return_val_if_fail (tag != NULL, FALSE);

	if (priv->hint_tags == NULL)
		asc_globals_create_hint_tag_table ();
	if (g_hash_table_contains (priv->hint_tags, tag))
		return FALSE;

	htag = asc_hint_tag_new (tag, severity, explanation);
	g_hash_table_insert (priv->hint_tags,
			     g_ref_string_new_intern (tag),
			     htag);
	return TRUE;
}

/**
 * asc_globals_get_hint_info:
 *
 * Return details for a given hint tag.
 *
 * Returns: (transfer none): Hint tag details.
 */
AscHintTag*
asc_globals_get_hint_tag_details (const gchar *tag)
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&priv->hint_tags_mutex);
	g_return_val_if_fail (tag != NULL, NULL);

	if (priv->hint_tags == NULL)
		asc_globals_create_hint_tag_table ();
	return g_hash_table_lookup (priv->hint_tags, tag);
}

/**
 * asc_globals_get_hint_tags:
 *
 * Retrieve all hint tags that we know.
 *
 * Returns: (transfer full): A list of valid hint tags. Free with %g_strfreev
 */
gchar**
asc_globals_get_hint_tags ()
{
	AscGlobalsPrivate *priv = asc_globals_get_priv ();
	GHashTableIter iter;
	gpointer key;
	gchar **strv;
	guint i = 0;
	g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&priv->hint_tags_mutex);

	if (priv->hint_tags == NULL)
		asc_globals_create_hint_tag_table ();

	/* deep-copy the table keys to a strv */
	strv = g_new0 (gchar*, g_hash_table_size (priv->hint_tags) + 1);
	g_hash_table_iter_init (&iter, priv->hint_tags);
	while (g_hash_table_iter_next (&iter, &key, NULL))
		strv[i++] = g_strdup ((const gchar*) key);

	return strv;
}

/**
 * asc_globals_hint_tag_severity:
 *
 * Retrieve the severity of the given hint tag.
 *
 * Returns: An #AsIssueSeverity or %AS_ISSUE_SEVERITY_UNKNOWN if the tag did not exist
 *          or has an unknown severity.
 */
AsIssueSeverity
asc_globals_hint_tag_severity (const gchar *tag)
{
	AscHintTag *htag = asc_globals_get_hint_tag_details (tag);
	if (htag == NULL)
		return AS_ISSUE_SEVERITY_UNKNOWN;
	return htag->severity;
}

/**
 * asc_globals_hint_tag_explanation:
 *
 * Retrieve the explanation template of the given hint tag.
 *
 * Returns: An explanation template, or %NULL if the tag was not found.
 */
const gchar*
asc_globals_hint_tag_explanation (const gchar *tag)
{
	AscHintTag *htag = asc_globals_get_hint_tag_details (tag);
	if (htag == NULL)
		return NULL;
	return htag->explanation;
}
