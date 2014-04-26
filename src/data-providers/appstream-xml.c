/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2012-2014 Matthias Klumpp <matthias@tenstral.net>
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

#include "appstream-xml.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>

#include "../as-utils.h"
#include "../as-settings-private.h"
#include "../as-metadata.h"
#include "../as-metadata-private.h"
#include "../as-menu-parser.h"

struct _AsProviderAppstreamXMLPrivate
{
	GList* system_categories;
};

const gchar* AS_APPSTREAM_XML_PATHS[4] = {AS_APPSTREAM_BASE_PATH "/xmls",
										"/var/cache/app-info/xmls",
										"/var/lib/app-info/xmls",
										NULL};

static gpointer as_provider_appstream_xml_parent_class = NULL;

#define AS_PROVIDER_APPSTREAM_XML_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), AS_PROVIDER_TYPE_APPSTREAM_XML, AsProviderAppstreamXMLPrivate))

static gboolean		as_provider_appstream_xml_real_execute (AsDataProvider* base);
static void			as_provider_appstream_xml_finalize (GObject* obj);

AsProviderAppstreamXML*
as_provider_appstream_xml_construct (GType object_type)
{
	AsProviderAppstreamXML * self = NULL;
	GList *syscat;
	guint i;
	guint len;
	gchar **wfiles;
	self = (AsProviderAppstreamXML*) as_data_provider_construct (object_type);

	/* cache this for performance reasons */
	syscat = as_get_system_categories ();
	self->priv->system_categories = syscat;

	len = G_N_ELEMENTS (AS_APPSTREAM_XML_PATHS);
	wfiles = g_new0 (gchar *, len + 1);
	for (i = 0; i < len+1; i++) {
		if (i < len)
			wfiles[i] = g_strdup (AS_APPSTREAM_XML_PATHS[i]);
		else
			wfiles[i] = NULL;
	}
	as_data_provider_set_watch_files ((AsDataProvider*) self, wfiles);
	g_strfreev (wfiles);

	return self;
}


AsProviderAppstreamXML*
as_provider_appstream_xml_new (void)
{
	return as_provider_appstream_xml_construct (AS_PROVIDER_TYPE_APPSTREAM_XML);
}

static gboolean
as_provider_appstream_xml_process_single_document (AsProviderAppstreamXML* self, const gchar* xmldoc_str)
{
	xmlDoc* doc;
	xmlNode* root;
	xmlNode* iter;
	AsMetadata *metad;
	AsComponent *cpt;
	GError *error = NULL;

	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (xmldoc_str != NULL, FALSE);

	doc = xmlParseDoc ((xmlChar*) xmldoc_str);
	if (doc == NULL) {
		fprintf (stderr, "Could not parse XML!");
		return FALSE;
	}

	root = xmlDocGetRootElement (doc);
	if (doc == NULL) {
		fprintf (stderr, "The XML document is empty.");
		return FALSE;
	}

	if (g_strcmp0 ((gchar*) root->name, "components") != 0) {
		fprintf (stderr, "XML file does not contain valid AppStream data!");
		return FALSE;
	}

	metad = as_metadata_new ();
	as_metadata_set_parser_mode (metad, AS_PARSER_MODE_DISTRO);

	for (iter = root->children; iter != NULL; iter = iter->next) {
		/* discard spaces */
		if (iter->type != XML_ELEMENT_NODE)
			continue;

		if (g_strcmp0 ((gchar*) iter->name, "component") == 0) {
			cpt = as_metadata_parse_component_node (metad, iter, &error);
			if (error != NULL) {
				as_data_provider_log_warning ((AsDataProvider*) metad, error->message);
				g_error_free (error);
				error = NULL;
			} else if (cpt != NULL) {
				as_data_provider_emit_application ((AsDataProvider*) self, cpt);
				g_object_unref (cpt);
			}
		}
	}
	xmlFreeDoc (doc);
	g_object_unref (metad);

	return TRUE;
}

gboolean
as_provider_appstream_xml_process_compressed_file (AsProviderAppstreamXML* self, GFile* infile)
{
	GFileInputStream* src_stream;
	GMemoryOutputStream* mem_os;
	GConverterOutputStream* conv_stream;
	GZlibDecompressor* zdecomp;
	guint8* data;
	gboolean ret;

	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (infile != NULL, FALSE);

	src_stream = g_file_read (infile, NULL, NULL);
	mem_os = (GMemoryOutputStream*) g_memory_output_stream_new (NULL, 0, g_realloc, g_free);
	zdecomp = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_GZIP);
	conv_stream = (GConverterOutputStream*) g_converter_output_stream_new ((GOutputStream*) mem_os, (GConverter*) zdecomp);
	g_object_unref (zdecomp);

	g_output_stream_splice ((GOutputStream*) conv_stream, (GInputStream*) src_stream, 0, NULL, NULL);
	data = g_memory_output_stream_get_data (mem_os);
	ret = as_provider_appstream_xml_process_single_document (self, (const gchar*) data);

	g_object_unref (conv_stream);
	g_object_unref (mem_os);
	g_object_unref (src_stream);
	return ret;
}


gboolean
as_provider_appstream_xml_process_file (AsProviderAppstreamXML* self, GFile* infile)
{
	gboolean ret;
	gchar* xml_doc;
	gchar* line = NULL;
	GFileInputStream* ir;
	GDataInputStream* dis;

	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (infile != NULL, FALSE);

	xml_doc = g_strdup ("");
	ir = g_file_read (infile, NULL, NULL);
	dis = g_data_input_stream_new ((GInputStream*) ir);
	g_object_unref (ir);

	while (TRUE) {
		gchar *str;
		gchar *tmp;

		line = g_data_input_stream_read_line (dis, NULL, NULL, NULL);
		if (line == NULL) {
			break;
		}

		str = g_strconcat (line, "\n", NULL);
		g_free (line);
		tmp = g_strconcat (xml_doc, str, NULL);
		g_free (str);
		g_free (xml_doc);
		xml_doc = tmp;
	}

	ret = as_provider_appstream_xml_process_single_document (self, xml_doc);
	g_object_unref (dis);
	g_free (xml_doc);
	return ret;
}


static gboolean
as_provider_appstream_xml_real_execute (AsDataProvider* base)
{
	AsProviderAppstreamXML * self;
	GPtrArray* xml_files;
	guint i;
	guint len;
	gchar *path;
	GFile *infile;

	self = (AsProviderAppstreamXML*) base;
	xml_files = g_ptr_array_new_with_free_func (g_free);

	len = G_N_ELEMENTS (AS_APPSTREAM_XML_PATHS);
	for (i = 0; i < len; i++) {
		GPtrArray *xmls;
		guint j;

		path = g_strdup (AS_APPSTREAM_XML_PATHS[i]);
		if (path == NULL)
			continue;
		if (!g_file_test (path, G_FILE_TEST_EXISTS)) {
			g_free (path);
			continue;
		}
		xmls = as_utils_find_files_matching (path, "*.xml*", FALSE);
		if (xmls == NULL)
			continue;
		for (j = 0; j < xmls->len; j++) {
			const gchar *val;
			val = (const gchar *) g_ptr_array_index (xmls, j);
			g_ptr_array_add (xml_files, g_strdup (val));
		}

		g_free (path);
		g_ptr_array_unref (xmls);
	}

	if (xml_files->len == 0) {
		g_ptr_array_unref (xml_files);
		return FALSE;
	}

	for (i = 0; i < xml_files->len; i++) {
		gchar *fname;
		fname = (gchar*) g_ptr_array_index (xml_files, i);
		infile = g_file_new_for_path (fname);
		if (!g_file_query_exists (infile, NULL)) {
			fprintf (stderr, "File '%s' does not exist.", fname);
			g_object_unref (infile);
			continue;
		}

		if (g_str_has_suffix (fname, ".xml")) {
			as_provider_appstream_xml_process_file (self, infile);
		} else if (g_str_has_suffix (fname, ".xml.gz")) {
			as_provider_appstream_xml_process_compressed_file (self, infile);
		}
		g_object_unref (infile);
	}
	g_ptr_array_unref (xml_files);

	return TRUE;
}

static void
as_provider_appstream_xml_class_init (AsProviderAppstreamXMLClass * klass)
{
	as_provider_appstream_xml_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (AsProviderAppstreamXMLPrivate));
	AS_DATA_PROVIDER_CLASS (klass)->execute = as_provider_appstream_xml_real_execute;
	G_OBJECT_CLASS (klass)->finalize = as_provider_appstream_xml_finalize;
}


static void
as_provider_appstream_xml_instance_init (AsProviderAppstreamXML * self)
{
	self->priv = AS_PROVIDER_APPSTREAM_XML_GET_PRIVATE (self);
}


static void
as_provider_appstream_xml_finalize (GObject* obj)
{
	AsProviderAppstreamXML * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, AS_PROVIDER_TYPE_APPSTREAM_XML, AsProviderAppstreamXML);
	g_object_unref (self->priv->system_categories);
	G_OBJECT_CLASS (as_provider_appstream_xml_parent_class)->finalize (obj);
}


GType
as_provider_appstream_xml_get_type (void)
{
	static volatile gsize as_provider_appstream_xml_type_id__volatile = 0;
	if (g_once_init_enter (&as_provider_appstream_xml_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (AsProviderAppstreamXMLClass),
										(GBaseInitFunc) NULL,
										(GBaseFinalizeFunc) NULL,
										(GClassInitFunc) as_provider_appstream_xml_class_init,
										(GClassFinalizeFunc) NULL,
										NULL, sizeof (AsProviderAppstreamXML),
										0,
										(GInstanceInitFunc) as_provider_appstream_xml_instance_init,
										NULL
		};
		GType as_provider_appstream_xml_type_id;
		as_provider_appstream_xml_type_id = g_type_register_static (AS_TYPE_DATA_PROVIDER, "AsProviderAppstreamXML", &g_define_type_info, 0);
		g_once_init_leave (&as_provider_appstream_xml_type_id__volatile, as_provider_appstream_xml_type_id);
	}
	return as_provider_appstream_xml_type_id__volatile;
}
