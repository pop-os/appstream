/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2012-2016 Matthias Klumpp <matthias@tenstral.net>
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

#ifndef __AS_COMPONENT_PRIVATE_H
#define __AS_COMPONENT_PRIVATE_H

#include <glib-object.h>
#include "as-component.h"
#include "as-settings-private.h"

G_BEGIN_DECLS
#pragma GCC visibility push(hidden)

/**
 * AsComponentScope:
 * @AS_COMPONENT_SCOPE_UNKNOWN:		Unknown scope
 * @AS_COMPONENT_SCOPE_USER:		User scope
 * @AS_COMPONENT_SCOPE_SYSTEM:		System scope
 *
 * Scope of the #AsComponent (system-wide or user-scope)
 **/
typedef enum {
	AS_COMPONENT_SCOPE_UNKNOWN,
	AS_COMPONENT_SCOPE_USER,
	AS_COMPONENT_SCOPE_SYSTEM,
	/*< private >*/
	AS_COMPONENT_SCOPE_LAST
} AsComponentScope;

/**
 * AsOriginKind:
 * @AS_ORIGIN_KIND_UNKNOWN:		Unknown origin kind.
 * @AS_ORIGIN_KIND_METAINFO:		Origin was a metainfo file.
 * @AS_ORIGIN_KIND_COLLECTION:		Origin was an AppStream collection file.
 * @AS_ORIGIN_KIND_DESKTOP_ENTRY:	Origin was a .desktop file.
 *
 * Scope of the #AsComponent (system-wide or user-scope)
 **/
typedef enum {
	AS_ORIGIN_KIND_UNKNOWN,
	AS_ORIGIN_KIND_METAINFO,
	AS_ORIGIN_KIND_COLLECTION,
	AS_ORIGIN_KIND_DESKTOP_ENTRY,
	/*< private >*/
	AS_ORIGIN_KIND_LAST
} AsOriginKind;

const gchar		*as_component_scope_to_string (AsComponentScope scope);
AsMergeKind		as_component_scope_from_string (const gchar *scope_str);

typedef guint16		AsTokenType; /* big enough for both bitshifts */

AS_INTERNAL_VISIBLE
gint			as_component_get_priority (AsComponent *cpt);
AS_INTERNAL_VISIBLE
void			as_component_set_priority (AsComponent *cpt,
							gint priority);

void			as_component_complete (AsComponent *cpt,
						gchar *scr_base_url,
						GPtrArray *icon_paths);

GHashTable		*as_component_get_name_table (AsComponent *cpt);
GHashTable		*as_component_get_summary_table (AsComponent *cpt);
GHashTable		*as_component_get_description_table (AsComponent *cpt);
GHashTable		*as_component_get_developer_name_table (AsComponent *cpt);
GHashTable		*as_component_get_keywords_table (AsComponent *cpt);
AS_INTERNAL_VISIBLE
GHashTable		*as_component_get_languages_table (AsComponent *cpt);

void			as_component_set_bundles_array (AsComponent *cpt,
							GPtrArray *bundles);

AS_INTERNAL_VISIBLE
GHashTable		*as_component_get_urls_table (AsComponent *cpt);

gboolean		as_component_has_package (AsComponent *cpt);
gboolean		as_component_has_install_candidate (AsComponent *cpt);

void			as_component_add_provided_item (AsComponent *cpt,
							AsProvidedKind kind,
							const gchar *item);

const gchar		*as_component_get_architecture (AsComponent *cpt);
void			 as_component_set_architecture (AsComponent *cpt,
							const gchar *arch);

void			 as_component_create_token_cache (AsComponent *cpt);
GHashTable		*as_component_get_token_cache_table (AsComponent *cpt);
void			 as_component_set_token_cache_valid (AsComponent *cpt,
							     gboolean valid);

guint			as_component_get_sort_score (AsComponent *cpt);
void			as_component_set_sort_score (AsComponent *cpt,
							guint score);

void			as_component_set_ignored (AsComponent *cpt,
						  gboolean ignore);

AsComponentScope	as_component_get_scope (AsComponent *cpt);
void			as_component_set_scope (AsComponent *cpt,
						AsComponentScope scope);

AsOriginKind		as_component_get_origin_kind (AsComponent *cpt);
void			as_component_set_origin_kind (AsComponent *cpt,
							AsOriginKind okind);

gboolean		as_component_merge (AsComponent *cpt,
					    AsComponent *source);
void			as_component_merge_with_mode (AsComponent *cpt,
							AsComponent *source,
							AsMergeKind merge_kind);

#pragma GCC visibility pop
G_END_DECLS

#endif /* __AS_COMPONENT_PRIVATE_H */
