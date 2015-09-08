/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2012-2014 Matthias Klumpp <matthias@tenstral.net>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the license, or
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

#include "ascli-validate-actions.h"

#include <config.h>
#include <locale.h>
#include <glib/gi18n-lib.h>
#include <appstream.h>

/**
 * importance_to_print_string:
 **/
static gchar*
importance_location_to_print_string (AsIssueImportance importance, const gchar *location, gboolean pretty)
{
	gchar *str;

	switch (importance) {
		case AS_ISSUE_IMPORTANCE_ERROR:
			str = g_strdup_printf ("E - %s", location);
			break;
		case AS_ISSUE_IMPORTANCE_WARNING:
			str = g_strdup_printf ("W - %s", location);
			break;
		case AS_ISSUE_IMPORTANCE_INFO:
			str = g_strdup_printf ("I - %s", location);
			break;
		case AS_ISSUE_IMPORTANCE_PEDANTIC:
			str = g_strdup_printf ("P - %s", location);
			break;
		default:
			str = g_strdup_printf ("U - %s", location);
	}

	if (pretty) {
		switch (importance) {
			case AS_ISSUE_IMPORTANCE_ERROR:
				return g_strdup_printf ("%c[%d;1m%s%c[%dm", 0x1B, 31, str, 0x1B, 0);
			case AS_ISSUE_IMPORTANCE_WARNING:
				return g_strdup_printf ("%c[%d;1m%s%c[%dm", 0x1B, 33, str, 0x1B, 0);
			case AS_ISSUE_IMPORTANCE_INFO:
				return g_strdup_printf ("%c[%d;1m%s%c[%dm", 0x1B, 32, str, 0x1B, 0);
			case AS_ISSUE_IMPORTANCE_PEDANTIC:
				return g_strdup_printf ("%c[%d;1m%s%c[%dm", 0x1B, 37, str, 0x1B, 0);
			default:
				return g_strdup_printf ("%c[%d;1m%s%c[%dm", 0x1B, 35, str, 0x1B, 0);
		}
	} else {
		return str;
	}

	g_free (str);
}

/**
 * print_report:
 **/
static gboolean
process_report (GList *issues, gboolean pretty, gboolean pedantic)
{
	GList *l;
	AsValidatorIssue *issue;
	AsIssueImportance importance;
	gboolean no_errors = TRUE;
	gchar *header;

	for (l = issues; l != NULL; l = l->next) {
		issue = (AsValidatorIssue*) l->data;
		importance = as_validator_issue_get_importance (issue);

		/* if there are errors or warnings, we consider the validation to be failed */
		if ((importance == AS_ISSUE_IMPORTANCE_ERROR) || (importance == AS_ISSUE_IMPORTANCE_WARNING))
			no_errors = FALSE;

		/* skip pedantic issues if we should not show them */
		if ((!pedantic) && (importance == AS_ISSUE_IMPORTANCE_PEDANTIC))
			continue;

		header = importance_location_to_print_string (importance,
								as_validator_issue_get_location (issue),
								pretty);
		g_print ("%s\n    %s\n\n",
				header,
				as_validator_issue_get_message (issue));
		g_free (header);
	}

	return no_errors;
}

/**
 * ascli_validate_file:
 **/
gboolean
ascli_validate_file (gchar *fname, gboolean pretty, gboolean pedantic)
{
	GFile *file;
	gboolean ret;
	gboolean errors_found = FALSE;
	AsValidator *validator;
	GList *issues;

	file = g_file_new_for_path (fname);
	if (!g_file_query_exists (file, NULL)) {
		g_print ("File '%s' does not exist.", fname);
		g_print ("\n");
		g_object_unref (file);
		return FALSE;
	}

	validator = as_validator_new ();
	ret = as_validator_validate_file (validator, file);
	if (!ret)
		errors_found = TRUE;
	issues = as_validator_get_issues (validator);

	ret = process_report (issues, pretty, pedantic);
	if (!ret)
		errors_found = TRUE;

	g_list_free (issues);
	g_object_unref (file);
	g_object_unref (validator);

	return !errors_found;
}

/**
 * ascli_validate_files:
 */
gint
ascli_validate_files (gchar **argv, gint argc, gboolean no_color, gboolean pedantic)
{
	gint i;
	gboolean ret;

	if (argc < 1) {
		g_print ("%s\n", _("You need to specify a file to validate!"));
		return 1;
	}

	for (i = 0; i < argc; i++) {
		gboolean tmp_ret;
		tmp_ret = ascli_validate_file (argv[i], !no_color, pedantic);
		if (!tmp_ret)
			ret = FALSE;
	}

	if (ret) {
		g_print ("%s\n", _("Validation was successful."));
	} else {
		g_print ("%s\n", _("Validation failed."));
		return 3;
	}

	return 0;
}

/**
 * ascli_validate_tree:
 */
gint
ascli_validate_tree (const gchar *root_dir, gboolean no_color, gboolean pedantic)
{
	gboolean no_errors = TRUE;
	AsValidator *validator;
	GList *issues;

	if (root_dir == NULL) {
		g_print ("%s\n", _("You need to specify a root directory to start validation!"));
		return 1;
	}

	validator = as_validator_new ();
	as_validator_validate_tree (validator, root_dir);
	issues = as_validator_get_issues (validator);

	no_errors = process_report (issues, !no_color, pedantic);

	g_list_free (issues);
	g_object_unref (validator);

	if (no_errors) {
		g_print ("%s\n", _("Validation was successful."));
	} else {
		g_print ("%s\n", _("Validation failed."));
		return 3;
	}

	return 0;
}
