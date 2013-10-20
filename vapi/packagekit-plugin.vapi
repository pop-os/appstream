/* packagekit-plugin.vapi generated by vapigen, do not modify. */

[CCode (cprefix = "Pk", gir_namespace = "PackageKitPlugin", gir_version = "1.0", lower_case_cprefix = "pk_")]
namespace PkPlugin {
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", type_id = "pk_backend_get_type ()")]
	public class Backend : GLib.Object {
		[CCode (has_construct_function = false)]
		public Backend ();
		public void accept_eula (string eula_id);
		public static unowned string bool_to_string (bool value);
		public void cancel (PkPlugin.BackendJob job);
		public void destroy ();
		public void download_packages (PkPlugin.BackendJob job, string package_ids, string directory);
		public string get_accepted_eula_string ();
		public unowned string get_author ();
		public void get_categories (PkPlugin.BackendJob job);
		public void get_depends (PkPlugin.BackendJob job, PackageKit.Bitfield filters, string package_ids, bool recursive);
		public unowned string get_description ();
		public void get_details (PkPlugin.BackendJob job, string package_ids);
		public void get_distro_upgrades (PkPlugin.BackendJob job);
		public void get_files (PkPlugin.BackendJob job, string package_ids);
		public PackageKit.Bitfield get_filters ();
		public PackageKit.Bitfield get_groups ();
		[CCode (array_length = false, array_null_terminated = true)]
		public string[] get_mime_types ();
		public unowned string get_name ();
		public void get_packages (PkPlugin.BackendJob job, PackageKit.Bitfield filters);
		public PackageKit.Bitfield get_provides ();
		public void get_repo_list (PkPlugin.BackendJob job, PackageKit.Bitfield filters);
		public void get_requires (PkPlugin.BackendJob job, PackageKit.Bitfield filters, string package_ids, bool recursive);
		public PackageKit.Bitfield get_roles ();
		public void get_update_detail (PkPlugin.BackendJob job, string package_ids);
		public void get_updates (PkPlugin.BackendJob job, PackageKit.Bitfield filters);
		public void implement (PackageKit.Role role);
		public void initialize ();
		public void install_files (PkPlugin.BackendJob job, PackageKit.Bitfield transaction_flags, string full_paths);
		public void install_packages (PkPlugin.BackendJob job, PackageKit.Bitfield transaction_flags, string package_ids);
		public void install_signature (PkPlugin.BackendJob job, PackageKit.SigType type, string key_id, string package_id);
		public bool is_eula_valid (string eula_id);
		public bool is_implemented (PackageKit.Role role);
		public bool is_online ();
		public bool load () throws GLib.Error;
		public void refresh_cache (PkPlugin.BackendJob job, bool force);
		public void remove_packages (PkPlugin.BackendJob job, PackageKit.Bitfield transaction_flags, string package_ids, bool allow_deps, bool autoremove);
		public void repair_system (PkPlugin.BackendJob job, PackageKit.Bitfield transaction_flags);
		public void repo_enable (PkPlugin.BackendJob job, string repo_id, bool enabled);
		public bool repo_list_changed ();
		public void repo_set_data (PkPlugin.BackendJob job, string repo_id, string parameter, string value);
		public void reset_job (PkPlugin.BackendJob job);
		public void resolve (PkPlugin.BackendJob job, PackageKit.Bitfield filters, string packages);
		public void search_details (PkPlugin.BackendJob job, PackageKit.Bitfield filters, string search);
		public void search_files (PkPlugin.BackendJob job, PackageKit.Bitfield filters, string search);
		public void search_groups (PkPlugin.BackendJob job, PackageKit.Bitfield filters, string search);
		public void search_names (PkPlugin.BackendJob job, PackageKit.Bitfield filters, string search);
		public void start_job (PkPlugin.BackendJob job);
		public void stop_job (PkPlugin.BackendJob job);
		public bool supports_parallelization ();
		public bool unload ();
		public void update_packages (PkPlugin.BackendJob job, PackageKit.Bitfield transaction_flags, string package_ids);
		public void upgrade_system (PkPlugin.BackendJob job, string distro_id, PackageKit.UpgradeKind upgrade_kind);
		public bool watch_file (string filename, PkPlugin.BackendFileChanged func);
		public void what_provides (PkPlugin.BackendJob job, PackageKit.Bitfield filters, PackageKit.Provides provides, string search);
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", type_id = "pk_backend_job_get_type ()")]
	public class BackendJob : GLib.Object {
		[CCode (has_construct_function = false)]
		public BackendJob ();
		public void category (string parent_id, string cat_id, string name, string summary, string icon);
		public void details (string package_id, string license, PackageKit.Group group, string description, string url, ulong size);
		public void distro_upgrade (PackageKit.DistroUpgradeType type, string name, string summary);
		public void eula_required (string eula_id, string package_id, string vendor_name, string license_agreement);
		public void files (string package_id, string files);
		public void finished ();
		public bool get_allow_cancel ();
		public void* get_backend ();
		public PkPlugin.Hint get_background ();
		public uint get_cache_age ();
		public unowned string get_cmdline ();
		public PackageKit.Exit get_exit_code ();
		public string get_frontend_socket ();
		public PkPlugin.Hint get_interactive ();
		public bool get_is_error_set ();
		public bool get_is_finished ();
		public string get_locale ();
		public bool get_locked ();
		public string get_no_proxy ();
		public string get_pac ();
		public GLib.Variant get_parameters ();
		public string get_proxy_ftp ();
		public string get_proxy_http ();
		public string get_proxy_https ();
		public string get_proxy_socks ();
		public PackageKit.Role get_role ();
		public uint get_runtime ();
		public bool get_started ();
		public PackageKit.Bitfield get_transaction_flags ();
		public uint get_uid ();
		public void* get_user_data ();
		public bool get_vfunc_enabled (PkPlugin.BackendJobSignal signal_kind);
		public bool has_set_error_code ();
		public void media_change_required (PackageKit.MediaType media_type, string media_id, string media_text);
		public void not_implemented_yet (string method);
		public void package (PackageKit.Info info, string package_id, string summary);
		public void repo_detail (string repo_id, string description, bool enabled);
		public void repo_signature_required (string package_id, string repository_name, string key_url, string key_userid, string key_id, string key_fingerprint, string key_timestamp, PackageKit.SigType type);
		public void require_restart (PackageKit.Restart restart, string package_id);
		public void reset ();
		public void set_allow_cancel (bool allow_cancel);
		public void set_backend (void* backend);
		public void set_background (PkPlugin.Hint background);
		public void set_cache_age (uint cache_age);
		public void set_cmdline (string cmdline);
		public void set_download_size_remaining (uint64 download_size_remaining);
		public void set_exit_code (PackageKit.Exit exit);
		public bool set_frontend_socket (string frontend_socket);
		public void set_interactive (PkPlugin.Hint interactive);
		public void set_item_progress (string package_id, PackageKit.Status status, uint percentage);
		public void set_locale (string code);
		public void set_locked (bool locked);
		public void set_parameters (GLib.Variant @params);
		public void set_percentage (uint percentage);
		public void set_proxy (string proxy_http, string proxy_https, string proxy_ftp, string proxy_socks, string no_proxy, string pac);
		public void set_role (PackageKit.Role role);
		public void set_speed (uint speed);
		public void set_started (bool started);
		public void set_status (PackageKit.Status status);
		public void set_transaction_flags (PackageKit.Bitfield transaction_flags);
		public void set_uid (uint uid);
		public void set_user_data (void* user_data);
		public void set_vfunc (PkPlugin.BackendJobSignal signal_kind, PkPlugin.BackendJobVFunc vfunc);
		public bool thread_create (owned PkPlugin.BackendJobThreadFunc func);
		public void update_detail (string package_id, string updates, string obsoletes, string vendor_urls, string bugzilla_urls, string cve_urls, PackageKit.Restart restart, string update_text, string changelog, PackageKit.UpdateState state, string issued, string updated);
		public bool use_background ();
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", type_id = "pk_conf_get_type ()")]
	public class Conf : GLib.Object {
		[CCode (has_construct_function = false)]
		public Conf ();
		public bool get_bool (string key);
		public static string get_filename ();
		public int get_int (string key);
		public string get_string (string key);
		[CCode (array_length = false, array_null_terminated = true)]
		public unowned string[] get_strv (string key);
		public void set_bool (string key, bool value);
		public void set_string (string key, string value);
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", type_id = "pk_transaction_get_type ()")]
	public class Transaction : GLib.Object {
		[CCode (has_construct_function = false)]
		public Transaction ();
		public void add_supported_content_type (string mime_type);
		public void cancel_bg ();
		public static GLib.Quark error_quark ();
		public unowned PkPlugin.BackendJob get_backend_job ();
		public unowned PkPlugin.Conf get_conf ();
		[CCode (array_length = false, array_null_terminated = true)]
		public unowned string[] get_full_paths ();
		[CCode (array_length = false, array_null_terminated = true)]
		public unowned string[] get_package_ids ();
		public unowned PackageKit.Results get_results ();
		public PackageKit.Role get_role ();
		public PkPlugin.TransactionState get_state ();
		public unowned string get_tid ();
		public PackageKit.Bitfield get_transaction_flags ();
		public uint get_uid ();
		[CCode (array_length = false, array_null_terminated = true)]
		public unowned string[] get_values ();
		public bool is_exclusive ();
		public bool is_finished_with_lock_required ();
		public void make_exclusive ();
		public void reset_after_lock_error ();
		public bool run ();
		public void set_backend (PkPlugin.Backend backend);
		public void set_full_paths (string full_paths);
		public void set_package_ids (string package_ids);
		public bool set_state (PkPlugin.TransactionState state);
		public void signals_reset (PkPlugin.BackendJob job);
		public void skip_auth_checks (bool skip_checks);
		public static unowned string state_to_string (PkPlugin.TransactionState state);
		public signal void finished ();
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", has_type_id = false)]
	public struct Plugin {
		public weak PkPlugin.Backend backend;
		public weak PkPlugin.BackendJob job;
		public void destroy ();
		public static unowned string get_description ();
		public void initialize ();
		public void state_changed ();
		public void transaction_content_types (PkPlugin.Transaction transaction);
		public void transaction_finished_end (PkPlugin.Transaction transaction);
		public void transaction_finished_results (PkPlugin.Transaction transaction);
		public unowned string transaction_get_action (PkPlugin.Transaction transaction, string action_id);
		public void transaction_run (PkPlugin.Transaction transaction);
		public void transaction_started (PkPlugin.Transaction transaction);
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", cprefix = "PK_BACKEND_SIGNAL_", has_type_id = false)]
	public enum BackendJobSignal {
		ALLOW_CANCEL,
		DETAILS,
		ERROR_CODE,
		DISTRO_UPGRADE,
		FINISHED,
		MESSAGE,
		PACKAGE,
		ITEM_PROGRESS,
		FILES,
		PERCENTAGE,
		REMAINING,
		SPEED,
		DOWNLOAD_SIZE_REMAINING,
		REPO_DETAIL,
		REPO_SIGNATURE_REQUIRED,
		EULA_REQUIRED,
		MEDIA_CHANGE_REQUIRED,
		REQUIRE_RESTART,
		STATUS_CHANGED,
		LOCKED_CHANGED,
		UPDATE_DETAIL,
		CATEGORY,
		LAST
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", cname = "PkHintEnum", cprefix = "PK_HINT_ENUM_", has_type_id = false)]
	[GIR (name = "HintEnum")]
	public enum Hint {
		FALSE,
		TRUE,
		UNSET,
		INVALID,
		LAST;
		public static PkPlugin.Hint enum_from_string (string hint);
		public static unowned string enum_to_string (PkPlugin.Hint hint);
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", cprefix = "PK_PLUGIN_PHASE_", has_type_id = false)]
	public enum PluginPhase {
		INIT,
		TRANSACTION_CONTENT_TYPES,
		TRANSACTION_RUN,
		TRANSACTION_STARTED,
		TRANSACTION_FINISHED_RESULTS,
		TRANSACTION_FINISHED_END,
		DESTROY,
		STATE_CHANGED,
		UNKNOWN
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", cprefix = "PK_TRANSACTION_STATE_", has_type_id = false)]
	public enum TransactionState {
		NEW,
		WAITING_FOR_AUTH,
		COMMITTED,
		READY,
		RUNNING,
		FINISHED,
		UNKNOWN
	}
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", has_target = false)]
	public delegate void BackendFileChanged (PkPlugin.Backend backend, void* data);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", instance_pos = 2.9)]
	public delegate void BackendJobThreadFunc (PkPlugin.BackendJob job, GLib.Variant @params);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", instance_pos = 2.9)]
	public delegate void BackendJobVFunc (PkPlugin.BackendJob job, void* object);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", has_target = false)]
	public delegate void PluginFunc (PkPlugin.Plugin plugin);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", has_target = false)]
	public delegate unowned string PluginGetActionFunc (PkPlugin.Plugin plugin, PkPlugin.Transaction transaction, string action_id);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", has_target = false)]
	public delegate unowned string PluginGetDescFunc ();
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", has_target = false)]
	public delegate void PluginTransactionFunc (PkPlugin.Plugin plugin, PkPlugin.Transaction transaction);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", cname = "PK_BACKEND_PERCENTAGE_INVALID")]
	public const int BACKEND_PERCENTAGE_INVALID;
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", cname = "PK_CONF_VALUE_INT_MISSING")]
	public const int CONF_VALUE_INT_MISSING;
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", cname = "PK_TRANSACTION_ALL_BACKEND_SIGNALS")]
	public const int TRANSACTION_ALL_BACKEND_SIGNALS;
	[CCode (cheader_filename = "plugin/packagekit-plugin.h", cname = "PK_TRANSACTION_NO_BACKEND_SIGNALS")]
	public const int TRANSACTION_NO_BACKEND_SIGNALS;
	[CCode (cheader_filename = "plugin/packagekit-plugin.h")]
	public static bool directory_remove_contents (string directory);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h")]
	public static GLib.DBusNodeInfo load_introspection (string filename) throws GLib.Error;
	[CCode (cheader_filename = "plugin/packagekit-plugin.h")]
	public static uint strlen (string text, uint len);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h")]
	public static bool strtoint (string text, int value);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h")]
	public static bool strtouint (string text, uint value);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h")]
	public static bool strtouint64 (string text, uint64 value);
	[CCode (cheader_filename = "plugin/packagekit-plugin.h")]
	public static bool strzero (string text);
}
