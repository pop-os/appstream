/* test-appstreamxml.vala
 *
 * Copyright (C) 2012 Matthias Klumpp
 *
 * Licensed under the GNU General Public License Version 3
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
 */

using GLib;

private string datadir;

void msg (string s) {
	stdout.printf (s + "\n");
}

void test_appstream_parser () {
	var asxml = new Uai.Provider.AppstreamXML ();

	asxml.process_single_file (Path.build_filename (datadir, "appdata.xml", null));
}

int main (string[] args) {
	msg ("=== Running AppStream-XML Tests ===");
	datadir = args[1];
	assert (datadir != null);
	datadir = Path.build_filename (datadir, "data", null);
	assert (FileUtils.test (datadir, FileTest.EXISTS) != false);

	Environment.set_variable ("G_MESSAGES_DEBUG", "all", true);
	Test.init (ref args);

	test_appstream_parser ();

	Test.run ();
	return 0;
}
