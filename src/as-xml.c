/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2012-2017 Matthias Klumpp <matthias@tenstral.net>
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

#include "as-xml.h"

#include <string.h>
#include "as-utils.h"
#include "as-utils-private.h"

/**
 * SECTION:as-xml
 * @short_description: Helper functions to parse AppStream XML data
 * @include: appstream.h
 */

/**
 * as_xml_get_node_value:
 */
gchar*
as_xml_get_node_value (xmlNode *node)
{
	gchar *content;
	content = (gchar*) xmlNodeGetContent (node);
	if (content != NULL)
		g_strstrip (content);

	return content;
}

/**
 * as_xmldata_get_node_locale:
 * @node: A XML node
 *
 * Returns: The locale of a node, if the node should be considered for inclusion.
 * %NULL if the node should be ignored due to a not-matching locale.
 */
gchar*
as_xmldata_get_node_locale (AsContext *ctx, xmlNode *node)
{
	gchar *lang;

	lang = (gchar*) xmlGetProp (node, (xmlChar*) "lang");

	if (lang == NULL) {
		lang = g_strdup ("C");
		goto out;
	}

	if (as_context_get_all_locale_enabled (ctx)) {
		/* we should read all languages */
		goto out;
	}

	if (as_utils_locale_is_compatible (as_context_get_locale (ctx), lang)) {
		goto out;
	}

	/* If we are here, we haven't found a matching locale.
	 * In that case, we return %NULL to indicate that this element should not be added.
	 */
	g_free (lang);
	lang = NULL;

out:
	return lang;
}

/**
 * as_xml_dump_node:
 */
static gboolean
as_xml_dump_node (xmlNode *node, gchar **content, gssize *len)
{
	xmlOutputBufferPtr obuf;

	obuf = xmlAllocOutputBuffer (NULL);
	g_assert (obuf != NULL);

	xmlNodeDumpOutput (obuf, node->doc, node, 0, 0, "utf-8");
	xmlOutputBufferFlush (obuf);

	if (xmlOutputBufferGetSize (obuf) > 0) {
		gssize l = xmlOutputBufferGetSize (obuf);
		if (len)
			*len = l;

		*content = g_strndup ((const gchar*) xmlOutputBufferGetContent (obuf), l);

		xmlOutputBufferClose (obuf);
		return TRUE;
	} else {
		xmlOutputBufferClose (obuf);
		return FALSE;
	}
}

/**
 * as_xml_dump_node_content:
 */
gchar*
as_xml_dump_node_content (xmlNode *node)
{
	g_autofree gchar *content = NULL;
	gchar *tmp;
	gssize len;

	/* discard spaces */
	if (node->type != XML_ELEMENT_NODE)
		return NULL;

	if (!as_xml_dump_node (node, &content, &len))
		return NULL;

	/* remove the encosing root node from the string */
	tmp = g_strrstr_len (content, len, "<");
	if (tmp != NULL)
		tmp[0] = '\0';

	tmp = g_strstr_len (content, -1, ">");
	if (tmp == NULL) {
		g_free (content);
		return NULL;
	}

	return g_strdup (tmp + 1);
}

/**
 * as_xml_dump_node_children:
 */
gchar*
as_xml_dump_node_children (xmlNode *node)
{
	GString *str = NULL;
	xmlNode *iter;

	str = g_string_new ("");
	for (iter = node->children; iter != NULL; iter = iter->next) {
		g_autofree gchar *content = NULL;
		gssize len;

		/* discard spaces */
		if (iter->type != XML_ELEMENT_NODE)
			continue;

		if (!as_xml_dump_node (iter, &content, &len))
			continue;

		if (str->len > 0)
			g_string_append (str, "\n");
		g_string_append_len (str, content, len);
	}

	return g_string_free (str, FALSE);
}

/**
 * as_xml_add_children_values_to_array:
 */
void
as_xml_add_children_values_to_array (xmlNode *node, const gchar *element_name, GPtrArray *array)
{
	xmlNode *iter;

	for (iter = node->children; iter != NULL; iter = iter->next) {
		/* discard spaces */
		if (iter->type != XML_ELEMENT_NODE) {
			continue;
		}
		if (g_strcmp0 ((const gchar*) iter->name, element_name) == 0) {
			gchar *content = as_xml_get_node_value (iter);
			/* transfer ownership of content to array */
			if (content != NULL)
				g_ptr_array_add (array, content);
		}
	}
}

/**
 * as_xml_get_children_as_string_list:
 */
GPtrArray*
as_xml_get_children_as_string_list (xmlNode *node, const gchar *element_name)
{
	GPtrArray *list;

	list = g_ptr_array_new_with_free_func (g_free);
	as_xml_add_children_values_to_array (node,
					     element_name,
					     list);
	return list;
}

/**
 * as_xml_get_children_as_strv:
 */
gchar**
as_xml_get_children_as_strv (xmlNode *node, const gchar *element_name)
{
	g_autoptr(GPtrArray) list = NULL;
	gchar **res;

	list = as_xml_get_children_as_string_list (node, element_name);
	res = as_ptr_array_to_strv (list);
	return res;
}

typedef struct
{
	xmlDoc		*doc;
	xmlNode 	*node;
	AsTag		tag_id;
	gchar		*locale;
	gboolean	localized;
	xmlNode		*d_node;
} AsXMLMarkupParseHelper;

/**
 * as_xml_markup_parse_helper_new: (skip)
 **/
static AsXMLMarkupParseHelper*
as_xml_markup_parse_helper_new (const gchar *markup, const gchar *locale)
{
	g_autofree gchar *xmldata = NULL;
	AsXMLMarkupParseHelper *helper = g_slice_new0 (AsXMLMarkupParseHelper);

	helper->locale = g_strdup (locale);
	xmldata = g_strdup_printf ("<root>%s</root>", markup);
	helper->doc = xmlReadMemory (xmldata, strlen (xmldata),
					NULL,
					"utf-8",
					XML_PARSE_NOBLANKS | XML_PARSE_NONET);
	if (helper->doc == NULL)
		return NULL;

	helper->d_node = NULL;
	helper->node = xmlDocGetRootElement (helper->doc);
	if (helper->node != NULL)
		helper->node = helper->node->children;
	if (helper->node != NULL)
		helper->tag_id = as_xml_tag_from_string ((const gchar*) helper->node->name);

	helper->localized = (locale != NULL) && (g_strcmp0 (locale, "C") != 0);

	return helper;
}

/**
 * as_xml_markup_parse_helper_free: (skip)
 **/
static void
as_xml_markup_parse_helper_free (AsXMLMarkupParseHelper *helper)
{
	if (helper->doc != NULL)
		xmlFreeDoc (helper->doc);
	g_free (helper->locale);
	g_slice_free (AsXMLMarkupParseHelper, helper);
}
G_DEFINE_AUTOPTR_CLEANUP_FUNC(AsXMLMarkupParseHelper, as_xml_markup_parse_helper_free)

/**
 * as_xml_markup_parse_helper_next: (skip)
 *
 * Advance to the next node.
 **/
static gboolean
as_xml_markup_parse_helper_next (AsXMLMarkupParseHelper *helper)
{
	if (helper->node == NULL)
		return FALSE;

	/* if we have a listing, jump into it */
	if ((helper->tag_id == AS_TAG_UL) || (helper->tag_id == AS_TAG_OL)) {
		helper->d_node = helper->node;
		helper->node = helper->node->children;
	} else {
		do {
			helper->node = helper->node->next;
		} while ((helper->node != NULL) && (helper->node->type != XML_ELEMENT_NODE));
	}

	if (helper->node == NULL) {
		if (helper->d_node != NULL) {
			helper->node = helper->d_node;
			helper->d_node = NULL;
			helper->node = helper->node->next;
		}
	}
	if (helper->node == NULL) {
		helper->tag_id = AS_TAG_UNKNOWN;
		return FALSE;
	}

	helper->tag_id = as_xml_tag_from_string ((const gchar*) helper->node->name);

	return TRUE;
}

/**
 * as_xml_markup_parse_helper_export_node: (skip)
 **/
static xmlNode*
as_xml_markup_parse_helper_export_node (AsXMLMarkupParseHelper *helper, xmlNode *parent, gboolean localized)
{
	if ((helper->tag_id == AS_TAG_P) || (helper->tag_id == AS_TAG_LI)) {
		xmlNode *cn = xmlAddChild (parent, xmlCopyNode (helper->node, TRUE));
		if (helper->localized && localized) {
			xmlNewProp (cn,
				    (xmlChar*) "xml:lang",
				    (xmlChar*) helper->locale);
		}
		return cn;
	}

	if ((helper->tag_id == AS_TAG_UL) || (helper->tag_id == AS_TAG_OL)) {
		return xmlNewChild (parent, NULL, helper->node->name, NULL);
	}

	return NULL;
}

/**
 * as_xml_parse_metainfo_description_node:
 */
void
as_xml_parse_metainfo_description_node (AsContext *ctx, xmlNode *node, GHFunc func, gpointer entity)
{
	xmlNode *iter;
	g_autoptr(GHashTable) desc = NULL;

	desc = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	for (iter = node->children; iter != NULL; iter = iter->next) {
		GString *str;
		AsTag tag_id;
		const gchar *node_name = (const gchar*) iter->name;

		/* discard spaces */
		if (iter->type != XML_ELEMENT_NODE)
			continue;
		tag_id = as_xml_tag_from_string (node_name);

		if (tag_id == AS_TAG_P) {
			g_autofree gchar *lang = NULL;
			g_autofree gchar *content = NULL;

			lang = as_xmldata_get_node_locale (ctx, iter);
			if (lang == NULL)
				/* this locale is not for us */
				continue;

			str = g_hash_table_lookup (desc, lang);
			if (str == NULL) {
				str = g_string_new ("");
				g_hash_table_insert (desc, g_strdup (lang), str);
			}

			content = as_xml_dump_node_content (iter);
			if (content != NULL)
				g_string_append_printf (str, "<p>%s</p>\n", content);

		} else if ((tag_id == AS_TAG_UL) || (tag_id == AS_TAG_OL)) {
			GHashTableIter htiter;
			gpointer hvalue;
			xmlNode *iter2;

			/* append listing node tag to every locale string */
			g_hash_table_iter_init (&htiter, desc);
			while (g_hash_table_iter_next (&htiter, NULL, &hvalue)) {
				GString *hstr = (GString*) hvalue;
				g_string_append_printf (hstr, "<%s>\n", node_name);
			}

			for (iter2 = iter->children; iter2 != NULL; iter2 = iter2->next) {
				g_autofree gchar *lang = NULL;
				g_autofree gchar *content = NULL;
				AsTag iter2_tag_id = as_xml_tag_from_string ((const gchar*) iter2->name);

				if (iter2->type != XML_ELEMENT_NODE)
					continue;
				if (iter2_tag_id != AS_TAG_LI)
					continue;

				lang = as_xmldata_get_node_locale (ctx, iter2);
				if (lang == NULL)
					continue;

				/* if the language is new, we add a listing tag first */
				str = g_hash_table_lookup (desc, lang);
				if (str == NULL) {
					str = g_string_new ("");
					g_string_append_printf (str, "<%s>\n", node_name);
					g_hash_table_insert (desc, g_strdup (lang), str);
				}

				content = as_xml_dump_node_content (iter2);
				if (content != NULL)
					g_string_append_printf (str, "  <%s>%s</%s>\n", (gchar*) iter2->name, content, (gchar*) iter2->name);
			}

			/* close listing tags */
			g_hash_table_iter_init (&htiter, desc);
			while (g_hash_table_iter_next (&htiter, NULL, &hvalue)) {
				GString *hstr = (GString*) hvalue;
				g_string_append_printf (hstr, "</%s>\n", node_name);
			}
		}
	}

	g_hash_table_foreach (desc, func, entity);
}

/**
 * as_xml_add_description_collection_mode_helper:
 *
 * Add the description markup for AppStream collection XML to the tree.
 */
static gboolean
as_xml_add_description_collection_mode_helper (xmlNode *parent, const gchar *description_markup, const gchar *lang)
{
	xmlNode *dnode;
	xmlNode *cnode;
	g_autoptr(AsXMLMarkupParseHelper) helper = NULL;

	if (as_str_empty (description_markup))
		return FALSE;

	/* skip cruft */
	if (as_is_cruft_locale (lang))
		return FALSE;

	helper = as_xml_markup_parse_helper_new (description_markup, lang);
	if (helper == NULL)
		return FALSE;

	dnode = xmlNewChild (parent, NULL, (xmlChar*) "description", NULL);
	if (helper->localized) {
		xmlNewProp (dnode,
				(xmlChar*) "xml:lang",
				(xmlChar*) lang);
	}
	cnode = dnode;

	if (helper->node == NULL)
		return FALSE;

	do {
		if ((helper->tag_id == AS_TAG_UL) || (helper->tag_id == AS_TAG_OL)) {
			cnode = as_xml_markup_parse_helper_export_node (helper, dnode, FALSE);
		} else {
			if (helper->tag_id != AS_TAG_LI)
				cnode = dnode;

			as_xml_markup_parse_helper_export_node (helper, cnode, FALSE);
		}
	} while (as_xml_markup_parse_helper_next (helper));

	return TRUE;
}

/**
 * as_xml_add_description_node:
 *
 * Add a description node to the XML document tree.
 */
void
as_xml_add_description_node (AsContext *ctx, xmlNode *root, GHashTable *desc_table)
{
	g_autoptr(GList) keys = NULL;
	keys = g_hash_table_get_keys (desc_table);
	keys = g_list_sort (keys, (GCompareFunc) g_ascii_strcasecmp);

	if (as_context_get_style (ctx) == AS_FORMAT_STYLE_METAINFO) {
		/* for metainfo files, we try to interleave translated and untranslated lines, just like in the original files.
		 * Of course this is imperfect and fails as soon as some lines are not translated, but for a fully translated
		 * file this works well enough */
		AsXMLMarkupParseHelper *c_helper;
		xmlNode *dnode = NULL;
		xmlNode *cnode = NULL;
		g_autoptr(GPtrArray) markup_nodes = g_ptr_array_new_with_free_func ((GDestroyNotify) as_xml_markup_parse_helper_free);

		for (GList *link = keys; link != NULL; link = link->next) {
			const gchar *locale = (const gchar*) link->data;
			const gchar *desc_markup = g_hash_table_lookup (desc_table, locale);
			AsXMLMarkupParseHelper *helper;

			if (as_is_cruft_locale (locale))
				continue;

			helper = as_xml_markup_parse_helper_new (desc_markup, locale);
			if (helper == NULL)
				continue;

			/* unlocalized entries should always be sorted first */
			if (helper->localized)
				g_ptr_array_add (markup_nodes, helper);
			else
				g_ptr_array_insert (markup_nodes, 0, helper);
		}

		/* check if there is something to do */
		if (markup_nodes->len <= 0)
			return;

		/* the first helper in our list is always for the unlocalized entries, unless we have none of these,
		 * in which case we just take the first localization */
		c_helper = (AsXMLMarkupParseHelper *) g_ptr_array_index (markup_nodes, 0);

		dnode = xmlNewChild (root, NULL, (xmlChar*) "description", NULL);
		cnode = dnode;
		do {
			if ((c_helper->tag_id == AS_TAG_UL) || (c_helper->tag_id == AS_TAG_OL)) {
				cnode = as_xml_markup_parse_helper_export_node (c_helper, dnode, TRUE);
			} else {
				if (c_helper->tag_id != AS_TAG_LI)
					cnode = dnode;

				as_xml_markup_parse_helper_export_node (c_helper, cnode, TRUE);
			}

			for (guint i = 1; i < markup_nodes->len; ++i) {
				AsXMLMarkupParseHelper *helper = g_ptr_array_index (markup_nodes, i);
				if (helper->node == NULL)
					continue;
				if (c_helper->tag_id != helper->tag_id)
					continue;
				if ((helper->tag_id != AS_TAG_UL) && (helper->tag_id != AS_TAG_OL))
					as_xml_markup_parse_helper_export_node (helper, cnode, TRUE);
				as_xml_markup_parse_helper_next (helper);
			}
		} while (as_xml_markup_parse_helper_next (c_helper));

		/* Due to imbalances caused by untranslated tags, we just append all the information that we couldn't match
		 * to the end of the file. This isn't great, but the best we can do here, since the original mapping of
		 * untranslated to translated sections is gone at this point */
		for (guint i = 0; i < markup_nodes->len; ++i) {
			AsXMLMarkupParseHelper *helper = g_ptr_array_index (markup_nodes, i);
			if (helper->node == NULL)
				continue;
			do {
				if ((helper->tag_id == AS_TAG_UL) || (helper->tag_id == AS_TAG_OL)) {
					cnode = as_xml_markup_parse_helper_export_node (helper, dnode, TRUE);
				} else {
					if (helper->tag_id != AS_TAG_LI)
						cnode = dnode;

					as_xml_markup_parse_helper_export_node (helper, cnode, TRUE);
				}
			} while (as_xml_markup_parse_helper_next (helper));
		}
	} else {
		/* we have a collection XML file, so write in that format (which is much faster and easier to do) */
		for (GList *link = keys; link != NULL; link = link->next) {
			const gchar *locale = (const gchar*) link->data;
			const gchar *desc_markup = g_hash_table_lookup (desc_table, locale);

			if (as_is_cruft_locale (locale))
				continue;

			as_xml_add_description_collection_mode_helper (root, desc_markup, locale);
		}
	}
}

/**
 * as_xml_add_localized_text_node:
 *
 * Add set of localized XML nodes based on a localization table.
 */
void
as_xml_add_localized_text_node (xmlNode *root, const gchar *node_name, GHashTable *value_table)
{
	g_autoptr(GList) keys = NULL;

	keys = g_hash_table_get_keys (value_table);
	keys = g_list_sort (keys, (GCompareFunc) g_ascii_strcasecmp);
	for (GList *link = keys; link != NULL; link = link->next) {
		xmlNode *cnode;
		const gchar *locale = (const gchar*) link->data;
		const gchar *str = (const gchar*) g_hash_table_lookup (value_table, locale);

		if (as_str_empty (str))
			continue;

		/* skip cruft */
		if (as_is_cruft_locale (locale))
			continue;

		cnode = xmlNewTextChild (root, NULL, (xmlChar*) node_name, (xmlChar*) str);
		if (g_strcmp0 (locale, "C") != 0) {
			xmlNewProp (cnode,
					(xmlChar*) "xml:lang",
					(xmlChar*) locale);
		}
	}
}

/**
 * as_xml_add_node_list_strv:
 *
 * Add node with a list of children containing the strv contents.
 */
xmlNode*
as_xml_add_node_list_strv (xmlNode *root, const gchar *name, const gchar *child_name, gchar **strv)
{
	xmlNode *node;
	guint i;

	/* don't add the node if we have no values */
	if (strv == NULL)
		return NULL;
	if (strv[0] == NULL)
		return NULL;

	if (name == NULL)
		node = root;
	else
		node = xmlNewChild (root,
				    NULL,
				    (xmlChar*) name,
				    NULL);
	for (i = 0; strv[i] != NULL; i++) {
		xmlNewTextChild (node,
				 NULL,
				 (xmlChar*) child_name,
				 (xmlChar*) strv[i]);
	}

	return node;
}

/**
 * as_xml_add_node_list:
 *
 * Add node with a list of children containing the string array contents.
 */
void
as_xml_add_node_list (xmlNode *root, const gchar *name, const gchar *child_name, GPtrArray *array)
{
	xmlNode *node;
	guint i;

	/* don't add the node if we have no values */
	if (array == NULL)
		return;
	if (array->len == 0)
		return;

	if (name == NULL)
		node = root;
	else
		node = xmlNewChild (root,
				    NULL,
				    (xmlChar*) name,
				    NULL);

	for (i = 0; i < array->len; i++) {
		const xmlChar *value = (const xmlChar*) g_ptr_array_index (array, i);
		xmlNewTextChild (node,
				 NULL,
				 (xmlChar*) child_name,
				 value);
	}
}

/**
 * as_xml_add_text_node:
 *
 * Add node if value is not empty
 */
xmlNode*
as_xml_add_text_node (xmlNode *root, const gchar *name, const gchar *value)
{
	if (as_str_empty (value))
		return NULL;

	return xmlNewTextChild (root, NULL, (xmlChar*) name, (xmlChar*) value);
}

/**
 * libxml_generic_error:
 *
 * Catch out-of-context errors emitted by libxml2.
 */
static void
libxml_generic_error (gchar **error_str_ptr, const char *format, ...)
{
	GString *str;
	va_list arg_ptr;
	gchar *error_str;
	static GMutex mutex;
	g_assert (error_str_ptr != NULL);

	error_str = (*error_str_ptr);

	g_mutex_lock (&mutex);
	str = g_string_new (error_str? error_str : "");

	va_start (arg_ptr, format);
	g_string_append_vprintf (str, format, arg_ptr);
	va_end (arg_ptr);

	g_free (error_str);
	*error_str_ptr = g_string_free (str, FALSE);
	g_mutex_unlock (&mutex);
}

/**
 * as_xml_set_out_of_context_error:
 */
static void
as_xml_set_out_of_context_error (gchar **error_msg_ptr)
{
	static GMutex mutex;

	g_mutex_lock (&mutex);
	if (error_msg_ptr == NULL) {
		xmlSetGenericErrorFunc (NULL, NULL);
	} else {
		g_free (*error_msg_ptr);
		(*error_msg_ptr) = NULL;
		xmlSetGenericErrorFunc (error_msg_ptr, (xmlGenericErrorFunc) libxml_generic_error);
	}
	g_mutex_unlock (&mutex);
}

/**
 * as_xmldata_parse_document:
 */
xmlDoc*
as_xml_parse_document (const gchar *data, gssize len, GError **error)
{
	xmlDoc *doc;
	xmlNode *root;
	g_autofree gchar *error_msg_str = NULL;

	if (data == NULL) {
		/* empty document means no components */
		return NULL;
	}

	if (len < 0)
		len = strlen (data);

	as_xml_set_out_of_context_error (&error_msg_str);
	doc = xmlReadMemory (data, len,
			     NULL,
			     "utf-8",
			     XML_PARSE_NOBLANKS | XML_PARSE_NONET | XML_PARSE_BIG_LINES);
	if (doc == NULL) {
		if (error_msg_str == NULL) {
			g_set_error (error,
					AS_METADATA_ERROR,
					AS_METADATA_ERROR_FAILED,
					"Could not parse XML data (no details received)");
		} else {
			g_set_error (error,
					AS_METADATA_ERROR,
					AS_METADATA_ERROR_FAILED,
					"Could not parse XML data: %s", error_msg_str);
		}
		as_xml_set_out_of_context_error (NULL);
		return NULL;
	}
	as_xml_set_out_of_context_error (NULL);

	root = xmlDocGetRootElement (doc);
	if (root == NULL) {
		g_set_error_literal (error,
				     AS_METADATA_ERROR,
				     AS_METADATA_ERROR_FAILED,
				     "The XML document is empty.");
		xmlFreeDoc (doc);
		return NULL;
	}

	return doc;
}

/**
 * as_xml_node_to_str:
 *
 * Converts an XML node into its textural representation.
 *
 * Returns: XML metadata.
 */
gchar*
as_xml_node_to_str (xmlNode *root, GError **error)
{
	xmlDoc *doc;
	gchar *xmlstr = NULL;
	g_autofree gchar *error_msg_str = NULL;

	as_xml_set_out_of_context_error (&error_msg_str);
	doc = xmlNewDoc ((xmlChar*) NULL);
	if (root == NULL)
		goto out;

	xmlDocSetRootElement (doc, root);
	xmlDocDumpFormatMemoryEnc (doc, (xmlChar**) (&xmlstr), NULL, "utf-8", TRUE);

	if (error_msg_str != NULL) {
		if (error == NULL) {
			g_warning ("Could not serialize XML document: %s", error_msg_str);
			goto out;
		} else {
			g_set_error (error,
					AS_METADATA_ERROR,
					AS_METADATA_ERROR_FAILED,
					"Could not serialize XML document: %s", error_msg_str);
			goto out;
		}
	}

out:
	as_xml_set_out_of_context_error (NULL);
	xmlFreeDoc (doc);
	return xmlstr;
}
