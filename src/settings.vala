/* settings.vala
 *
 * Copyright (C) 2012-2013 Matthias Klumpp <matthias@tenstral.net>
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

using GLib;
using Config;

namespace Appstream {

private static const string DB_SCHEMA_VERSION = "6";

private static const string APPSTREAM_BASE_PATH = DATADIR + "/app-info";

private static const string CONFIG_NAME = "/etc/appstream.conf";

/**
 * The path where software icons (of not-installed software) are located.
 */
private static const string ICON_PATH = APPSTREAM_BASE_PATH + "/icons";

internal static const string APPSTREAM_XML_PATH = APPSTREAM_BASE_PATH + "/xmls";
internal static const string SOFTWARE_CENTER_DATABASE_PATH = "/var/cache/app-info/xapian";

/**
 * Get details about the AppStream settings for the
 * current distribution
 */
public class DistroDetails : Object {
	public string distro_id { get; private set; }
	public string distro_name { get; private set; }
	public string distro_version { get; private set; }

	public string icon_repository_path { get { return ICON_PATH; } }

	private KeyFile keyf;

	public DistroDetails () {
		distro_id = "unknown";
		distro_name = "";
		distro_version = "";
		// get details about the distribution we are running on
		var f = File.new_for_path ("/etc/os-release");
		if (f.query_exists ()) {
			try {
				var dis = new DataInputStream (f.read ());
				string line;
				while ((line = dis.read_line (null)) != null) {
					string[] data = line.split ("=", 2);
					if (data.length != 2)
						continue;

					string dvalue = data[1];
					if (dvalue.has_prefix ("\""))
						dvalue = dvalue.substring (1, dvalue.length - 2);

					if (data[0] == "ID")
						distro_id = dvalue;
					else if (data[0] == "NAME")
						distro_name = dvalue;
					else if (data[0] == "VERSION_ID")
						distro_version = dvalue;

				}
			} catch (Error e) { }
		}

		// load configuration
		keyf = new KeyFile ();
		try {
			keyf.load_from_file (CONFIG_NAME, KeyFileFlags.NONE);
		} catch (Error e) {}
	}

	public string? config_distro_get_str (string key) {
		string? str;
		try {
			str = keyf.get_string (distro_id, key);
		} catch (Error e) {
			return null;
		}

		return str;
	}

	public bool config_distro_get_bool (string key) {
		bool ret;
		try {
			ret = keyf.get_boolean (distro_id, key);
		} catch (Error e) {
			return false;
		}

		return ret;
	}
}

} // End of namespace: Appstream
