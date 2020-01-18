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

#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>

#include "appstream.h"
#include "as-metadata.h"
#include "as-test-utils.h"
#include "as-pool-private.h"
#include "as-cache.h"

static gchar *datadir = NULL;


/**
 * test_get_sampledata_pool:
 *
 * Internal helper to get a pool with the sample data locations set.
 */
static AsPool*
test_get_sampledata_pool (gboolean use_caches)
{
	AsPool *pool;
	AsPoolFlags flags;
	g_autofree gchar *mdata_dir = NULL;

	/* create AsPool and load sample metadata */
	mdata_dir = g_build_filename (datadir, "collection", NULL);

	pool = as_pool_new ();
	as_pool_clear_metadata_locations (pool);
	as_pool_add_metadata_location (pool, mdata_dir);
	as_pool_set_locale (pool, "C");

	flags = as_pool_get_flags (pool);
	as_flags_remove (flags, AS_POOL_FLAG_READ_DESKTOP_FILES);
	as_flags_remove (flags, AS_POOL_FLAG_READ_METAINFO);
	as_pool_set_flags (pool, flags);

	if (!use_caches)
		as_pool_set_cache_flags (pool, AS_CACHE_FLAG_NONE);

	return pool;
}


/**
 * Test performance of loading a metadata pool from XML.
 */
static void
test_pool_xml_read_perf (void)
{
	GError *error = NULL;
	guint i;
	guint loops = 1000;
	g_autoptr(GTimer) timer = NULL;

	timer = g_timer_new ();
	for (i = 0; i < loops; i++) {
		g_autoptr(GPtrArray) cpts = NULL;
		g_autoptr(AsPool) pool = test_get_sampledata_pool (FALSE);

		as_pool_load (pool, NULL, &error);
		g_assert_no_error (error);

		cpts = as_pool_get_components (pool);
		g_assert_cmpint (cpts->len, ==, 19);

	}
	g_print ("%.2f ms: ", g_timer_elapsed (timer, NULL) * 1000 / loops);
}

/**
 * Test performance of metadata caches.
 */
static void
test_pool_cache_perf (void)
{
	GError *error = NULL;
	g_autoptr(GTimer) timer = NULL;
	g_autoptr(GPtrArray) prep_cpts = NULL;
	g_autoptr(AsCache) cache = NULL;
	g_auto(GStrv) strv = NULL;

	guint loops = 1000;
	const gchar *cache_location = "/tmp/as-unittest-perfcache.mdb";

	/* prepare a cache file and list of components to work with */
	{
		g_autoptr(AsPool) prep_pool = NULL;
		prep_pool = test_get_sampledata_pool (FALSE);
		as_pool_set_cache_location (prep_pool, cache_location);
		as_pool_load (prep_pool, NULL, &error);
		g_assert_no_error (error);

		prep_cpts = as_pool_get_components (prep_pool);
		g_assert_cmpint (prep_cpts->len, ==, 19);
	}

	/* test fetching all components from cache */
	timer = g_timer_new ();
	for (guint i = 0; i < loops; i++) {
		g_autoptr(GPtrArray) cpts = NULL;
		AsPoolFlags flags;
		g_autoptr(AsPool) pool = as_pool_new ();

		as_pool_clear_metadata_locations (pool);
		as_pool_set_locale (pool, "C");
		as_pool_set_cache_location (pool, cache_location);

		flags = as_pool_get_flags (pool);
		as_flags_remove (flags, AS_POOL_FLAG_READ_DESKTOP_FILES);
		as_flags_remove (flags, AS_POOL_FLAG_READ_METAINFO);
		as_pool_set_flags (pool, flags);
		as_pool_set_cache_flags (pool, as_pool_get_cache_flags (pool) | AS_CACHE_FLAG_NO_CLEAR);

		as_pool_load (pool, NULL, &error);
		g_assert_no_error (error);

		cpts = as_pool_get_components (pool);
		g_assert_cmpint (cpts->len, ==, 19);

	}
	g_print ("\n    Cache readall: %.2f ms", g_timer_elapsed (timer, NULL) * 1000 / loops);
	g_assert_cmpint (g_remove (cache_location), ==, 0);

	/* test cache write speed */
	g_timer_reset (timer);
	for (guint i = 0; i < loops; i++) {
		g_autoptr(AsCache) tmp_cache = as_cache_new ();
		as_cache_set_nosync (tmp_cache, TRUE);
		as_cache_open (tmp_cache, cache_location, "C", &error);
		g_assert_no_error (error);

		for (guint i = 0; i < prep_cpts->len; i++) {
			AsComponent *cpt = AS_COMPONENT (g_ptr_array_index (prep_cpts, i));
			as_cache_insert (tmp_cache, cpt, &error);
			g_assert_no_error (error);
		}

		g_assert_cmpint (g_remove (cache_location), ==, 0);
	}
	g_print ("\n    Cache write: %.2f ms", g_timer_elapsed (timer, NULL) * 1000 / loops);

	/* test search */
	cache = as_cache_new ();
	as_cache_open (cache, cache_location, "C", &error);
	g_assert_no_error (error);

	for (guint i = 0; i < prep_cpts->len; i++) {
		AsComponent *cpt = AS_COMPONENT (g_ptr_array_index (prep_cpts, i));
		as_cache_insert (cache, cpt, &error);
		g_assert_no_error (error);
	}

	strv = g_strsplit ("gam\namateur", "\n", -1);
	g_timer_reset (timer);
	for (guint i = 0; i < loops; i++) {
		g_autoptr(GPtrArray) test_cpts = NULL;
		test_cpts = as_cache_search (cache, strv, TRUE, &error);
		g_assert_no_error (error);

		g_assert_cmpint (test_cpts->len, ==, 6);
	}
	g_print ("\n    Cache search: %.4f ms", g_timer_elapsed (timer, NULL) * 1000 / loops);

	g_print ("\n    Status: ");
}

/**
 * main:
 */
int
main (int argc, char **argv)
{
	int ret;

	g_test_init (&argc, &argv, NULL);

	if (argc == 0) {
		g_error ("No test directory specified!");
		return 1;
	}

	datadir = argv[1];
	g_assert (datadir != NULL);
	datadir = g_build_filename (datadir, "samples", NULL);
	g_assert (g_file_test (datadir, G_FILE_TEST_EXISTS) != FALSE);

	/* only critical and error are fatal */
	g_log_set_fatal_mask (NULL, G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL);

	/* Only ever run tests in slow mode (set manually).
	 * This prevents build/test failures on slower machines */
	if (!g_test_slow ())
		return 0;


	g_test_add_func ("/Perf/Pool/ReadXML", test_pool_xml_read_perf);
	g_test_add_func ("/Perf/Pool/Cache", test_pool_cache_perf);

	ret = g_test_run ();
	g_free (datadir);
	return ret;
}
