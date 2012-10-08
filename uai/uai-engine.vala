/* uai-engine.vala
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
using Uai;
using Appstream;

namespace Uai {

[DBus (name = "org.freedesktop.AppStream")]
public class Engine : Object {
	private Appstream.DatabaseWrite db_rw;
	private Array<Appstream.AppInfo> appList;
	private Timer timer;

	public signal void finished ();
	public signal void rebuild_finished ();

	public Engine () {
		db_rw = new Appstream.DatabaseWrite ();

		// Update db path if necessary
		if (CURRENT_DB_PATH == "")
			CURRENT_DB_PATH = db_rw.database_path;

		timer = new Timer ();
		appList = new Array<Appstream.AppInfo> ();
	}

	public void init () {
		db_rw.database_path = CURRENT_DB_PATH;

		// Make sure directory exists
		Appstream.Utils.touch_dir (CURRENT_DB_PATH);

		db_rw.open ();
		timer.start ();
	}

	private bool run_provider (DataProvider dprov) {
		dprov.application.connect (new_application);
		return dprov.execute ();
	}

	private void new_application (Appstream.AppInfo app) {
		appList.append_val (app);
	}

	public uint get_idle_time_seconds () {
		return (uint) timer.elapsed ();
	}

	public bool refresh (GLib.BusName sender) {
		bool ret = false;

		timer.stop ();
		timer.reset ();

		try {
			var authority = Polkit.Authority.get_sync (null);
			var subject = Polkit.SystemBusName.new (sender);

			var res = authority.check_authorization_sync (subject,
								"org.freedesktop.appstream.refresh",
								null,
								Polkit.CheckAuthorizationFlags.ALLOW_USER_INTERACTION,
								null);
			ret = res.get_is_authorized ();
		} catch (Error e) {
			critical (e.message);
			rebuild_finished ();
			return false;
		}

		if (!ret) {
			warning ("Couldn't get authorization for this action!");
			return false;
		}

#if APPSTREAM
		run_provider (new Provider.AppstreamXML ());
#endif
#if DEBIAN_DEP11
		run_provider (new Provider.DEP11 ());
#endif
#if UBUNTU_APPINSTALL
		run_provider (new Provider.UbuntuAppinstall ());
#endif

		ret = db_rw.rebuild (appList);

		rebuild_finished ();
		timer.start ();

		return ret;
	}
}

} // End of namespace: Uai
