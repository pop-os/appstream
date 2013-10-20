/**
 * SECTION:settings
 * @short_description: Get details about the AppStream settings for the current distribution
 */
/**
 * APPSTREAM_TYPE_DISTRO_DETAILS:
 * 
 * The type for <link linkend="AppstreamDistroDetails"><type>AppstreamDistroDetails</type></link>.
 */
/**
 * appstream_distro_details_get_icon_repository_paths:
 * @self: the <link linkend="AppstreamDistroDetails"><type>AppstreamDistroDetails</type></link> instance
 * 
 * Returns list of icon-paths for software-center applications to use. Icons of software (even if it is not installed) are stored in these locations.
 * 
 * Returns: (array length=result_length1): A NULL-terminated array of paths. 
 */
/**
 * appstream_distro_details_config_distro_get_str:
 * @self: the <link linkend="AppstreamDistroDetails"><type>AppstreamDistroDetails</type></link> instance
 * @key: &nbsp;
 */
/**
 * appstream_distro_details_config_distro_get_bool:
 * @self: the <link linkend="AppstreamDistroDetails"><type>AppstreamDistroDetails</type></link> instance
 * @key: &nbsp;
 */
/**
 * appstream_distro_details_new:
 */
/**
 * AppstreamDistroDetails:distro-id:
 */
/**
 * appstream_distro_details_get_distro_id:
 * @self: the <link linkend="AppstreamDistroDetails"><type>AppstreamDistroDetails</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamDistroDetails--distro-id"><type>"distro-id"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamDistroDetails--distro-id"><type>"distro-id"</type></link> property
 */
/**
 * AppstreamDistroDetails:distro-name:
 */
/**
 * appstream_distro_details_get_distro_name:
 * @self: the <link linkend="AppstreamDistroDetails"><type>AppstreamDistroDetails</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamDistroDetails--distro-name"><type>"distro-name"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamDistroDetails--distro-name"><type>"distro-name"</type></link> property
 */
/**
 * AppstreamDistroDetails:distro-version:
 */
/**
 * appstream_distro_details_get_distro_version:
 * @self: the <link linkend="AppstreamDistroDetails"><type>AppstreamDistroDetails</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamDistroDetails--distro-version"><type>"distro-version"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamDistroDetails--distro-version"><type>"distro-version"</type></link> property
 */
/**
 * AppstreamDistroDetails:
 * 
 * Get details about the AppStream settings for the current distribution
 */
/**
 * AppstreamDistroDetailsClass:
 * @parent_class: the parent class structure
 * 
 * The class structure for <link linkend="APPSTREAM-TYPE-DISTRO-DETAILS:CAPS"><literal>APPSTREAM_TYPE_DISTRO_DETAILS</literal></link>. All the fields in this structure are private and should never be accessed directly.
 */
