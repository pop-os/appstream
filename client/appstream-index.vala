/* appstream-index.vala -- Simple client for the Update-AppStream-Index DBus service
 *
 * Copyright (C) 2012-2013 Matthias Klumpp <matthias@tenstral.net>
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

private class UaiClient : Object {
	// Cmdln options
	private static bool o_show_version = false;
	private static bool o_verbose_mode = false;
	private static bool o_refresh = false;
	private static bool o_no_wait = false;
	private static string? o_search = null;

	private MainLoop loop;

	public int exit_code { get; set; }

	private const OptionEntry[] options = {
		{ "version", 'v', 0, OptionArg.NONE, ref o_show_version,
		N_("Show the application's version"), null },
		{ "verbose", 0, 0, OptionArg.NONE, ref o_verbose_mode,
			N_("Enable verbose mode"), null },
		{ "refresh", 0, 0, OptionArg.NONE, ref o_refresh,
		N_("Refresh the AppStream application cache"), null },
		{ "nowait", 0, 0, OptionArg.NONE, ref o_no_wait,
		N_("Don't wait for actions to complete'"), null },
		{ "search", 's', 0, OptionArg.STRING, ref o_search,
		N_("Search the application database"), null },
		{ null }
	};

	public UaiClient (string[] args) {
		exit_code = 0;
		var opt_context = new OptionContext ("- Update-AppStream-Index client tool.");
		opt_context.set_help_enabled (true);
		opt_context.add_main_entries (options, null);
		try {
			opt_context.parse (ref args);
		} catch (Error e) {
			stdout.printf (e.message + "\n");
			stdout.printf (_("Run '%s --help' to see a full list of available command line options.\n"), args[0]);
			exit_code = 1;
			return;
		}

		loop = new MainLoop ();
	}

	private void quit_loop () {
		if (loop.is_running ())
			loop.quit ();
	}

	public void run () {
		if (exit_code > 0)
			return;

		if (o_show_version) {
			stdout.printf ("AppStream-index client tool version: %s\n", Config.VERSION);
			return;
		}

		// Just a hack, we might need proper message handling later
		if (o_verbose_mode)
			Environment.set_variable ("G_MESSAGES_DEBUG", "all", true);


		// Prepare the AppStream database connection
		var db = new Appstream.Database ();
		/* Connecting to 'finished' signal */
		db.finished.connect((action, success) => {
			if (action != "refresh")
				return;

			if (success)
				stdout.printf ("%s\n", _("Successfully rebuilt the app-info cache."));
			else
				stdout.printf ("%s\n", _("Unable to rebuild the app-info cache."));
			quit_loop ();
		});

		db.error_code.connect((error_details) => {
			stderr.printf ("Failed: %s\n", error_details);
		});

		db.authorized.connect((success) => {
			// return immediately without waiting for action to complete if user has set --nowait
			if (o_no_wait)
				quit_loop ();
		});

		if (o_refresh) {
			try {
				if (o_no_wait)
					stdout.printf ("%s\n", _("Triggered app-info cache rebuild."));
				else
					stdout.printf ("%s\n", _("Rebuilding app-info cache..."));

				db.refresh.begin ((obj, res) => {
					try {
						db.refresh.end(res);
					} catch (Error e) {
						exit_code = 6;
						quit_loop ();
						critical (e.message);
					}

					// just make sure that we really quit the loop
					quit_loop ();
				});

				// start looping until refresh completes or fails
				loop.run();
			} catch (IOError e) {
				stderr.printf ("%s\n", e.message);
			}
		} else if (o_search != null) {
			db.open ();
			PtrArray? app_list;
			app_list = db.find_applications_by_str (o_search);
			if (app_list == null) {
				// this might be an error
				stdout.printf ("Unable to find application matching %s!\n", o_search);
				exit_code = 4;
				return;
			}
			if (app_list.len == 0) {
				stdout.printf ("No application matching '%s' found.\n", o_search);
				return;
			}
			for (uint i = 0; i < app_list.len; i++) {
				var app = (Appstream.AppInfo) app_list.index (i);
				stdout.printf ("Application: %s\nSummary: %s\nPackage: %s\nURL:%s\nDesktop: %s\nIcon: %s\n", app.name, app.summary, app.pkgname, app.url, app.desktop_file, app.icon);
				stdout.printf ("------\n");
			}

		} else {
			stderr.printf ("No command specified.\n");
			return;
		}
	}

	static int main (string[] args) {
		// Bind locale
		Intl.setlocale(LocaleCategory.ALL,"");
		Intl.bindtextdomain(Config.GETTEXT_PACKAGE, Config.LOCALEDIR);
		Intl.bind_textdomain_codeset(Config.GETTEXT_PACKAGE, "UTF-8");
		Intl.textdomain(Config.GETTEXT_PACKAGE);

		var main = new UaiClient (args);

		// Run the application
		main.run ();
		int code = main.exit_code;

		return code;
	}

}
