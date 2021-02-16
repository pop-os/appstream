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

#if !defined (__APPSTREAM_COMPOSE_H) && !defined (ASC_COMPILATION)
#error "Only <appstream-compose.h> can be included directly."
#endif
#pragma once

#include <glib-object.h>
#include <appstream.h>

G_BEGIN_DECLS

AsComponent		*asc_parse_metainfo_data (AscResult *cres,
						  AsMetadata *mdata,
						  GBytes *bytes,
						  const gchar *mi_basename);
AsComponent		*asc_parse_metainfo_data_simple (AscResult *cres,
							 GBytes *bytes,
							 const gchar *mi_basename);

void			asc_validate_metainfo_data_for_component (AscResult *cres,
								  AsValidator *validator,
								  AsComponent *cpt,
								  GBytes *bytes,
								  const gchar *mi_basename);

G_END_DECLS
