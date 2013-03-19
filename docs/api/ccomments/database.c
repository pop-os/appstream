/**
 * SECTION:database
 * @short_description: Class to access the AppStream application database
 */
/**
 * APPSTREAM_TYPE_SEARCH_QUERY:
 * 
 * The type for <link linkend="AppstreamSearchQuery"><type>AppstreamSearchQuery</type></link>.
 */
/**
 * appstream_search_query_get_search_all_categories:
 * @self: the <link linkend="AppstreamSearchQuery"><type>AppstreamSearchQuery</type></link> instance
 */
/**
 * appstream_search_query_set_search_all_categories:
 * @self: the <link linkend="AppstreamSearchQuery"><type>AppstreamSearchQuery</type></link> instance
 */
/**
 * appstream_search_query_set_categories_from_string:
 * @self: the <link linkend="AppstreamSearchQuery"><type>AppstreamSearchQuery</type></link> instance
 */
/**
 * appstream_search_query_new:
 */
/**
 * AppstreamSearchQuery:search-term:
 */
/**
 * appstream_search_query_get_search_term:
 * @self: the <link linkend="AppstreamSearchQuery"><type>AppstreamSearchQuery</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamSearchQuery--search-term"><type>"search-term"</type></link> property.
 * 
 * Returns: the value of the <link linkend="AppstreamSearchQuery--search-term"><type>"search-term"</type></link> property
 */
/**
 * appstream_search_query_set_search_term:
 * @self: the <link linkend="AppstreamSearchQuery"><type>AppstreamSearchQuery</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamSearchQuery--search-term"><type>"search-term"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamSearchQuery--search-term"><type>"search-term"</type></link> property to @value.
 */
/**
 * AppstreamSearchQuery:categories:
 * @result_length1: return location for the length of the property's value
 * @value_length1: length of the property's new value
 */
/**
 * appstream_search_query_get_categories:
 * @self: the <link linkend="AppstreamSearchQuery"><type>AppstreamSearchQuery</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamSearchQuery--categories"><type>"categories"</type></link> property.
 * 
 * Returns: the value of the <link linkend="AppstreamSearchQuery--categories"><type>"categories"</type></link> property
 */
/**
 * appstream_search_query_set_categories:
 * @self: the <link linkend="AppstreamSearchQuery"><type>AppstreamSearchQuery</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamSearchQuery--categories"><type>"categories"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamSearchQuery--categories"><type>"categories"</type></link> property to @value.
 */
/**
 * AppstreamSearchQuery:
 */
/**
 * AppstreamSearchQueryClass:
 * @parent_class: the parent class structure
 * 
 * The class structure for <link linkend="APPSTREAM-TYPE-SEARCH-QUERY:CAPS"><literal>APPSTREAM_TYPE_SEARCH_QUERY</literal></link>. All the fields in this structure are private and should never be accessed directly.
 */
/**
 * APPSTREAM_TYPE_DATABASE:
 * 
 * The type for <link linkend="AppstreamDatabase"><type>AppstreamDatabase</type></link>.
 */
/**
 * appstream_database_open:
 * @self: the <link linkend="AppstreamDatabase"><type>AppstreamDatabase</type></link> instance
 */
/**
 * appstream_database_db_exists:
 * @self: the <link linkend="AppstreamDatabase"><type>AppstreamDatabase</type></link> instance
 */
/**
 * appstream_database_get_all_applications:
 * @self: the <link linkend="AppstreamDatabase"><type>AppstreamDatabase</type></link> instance
 */
/**
 * appstream_database_find_applications:
 * @self: the <link linkend="AppstreamDatabase"><type>AppstreamDatabase</type></link> instance
 */
/**
 * appstream_database_find_applications_by_str:
 * @self: the <link linkend="AppstreamDatabase"><type>AppstreamDatabase</type></link> instance
 */
/**
 * appstream_database_new:
 */
/**
 * AppstreamDatabase:database-path:
 */
/**
 * appstream_database_get_database_path:
 * @self: the <link linkend="AppstreamDatabase"><type>AppstreamDatabase</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamDatabase--database-path"><type>"database-path"</type></link> property.
 * 
 * Returns: the value of the <link linkend="AppstreamDatabase--database-path"><type>"database-path"</type></link> property
 */
/**
 * appstream_database_set_database_path:
 * @self: the <link linkend="AppstreamDatabase"><type>AppstreamDatabase</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamDatabase--database-path"><type>"database-path"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamDatabase--database-path"><type>"database-path"</type></link> property to @value.
 */
/**
 * AppstreamDatabase:
 * 
 * Class to access the AppStream application database
 */
/**
 * AppstreamDatabaseClass:
 * @open: virtual method called by <link linkend="appstream-database-open"><function>appstream_database_open()</function></link>
 * @parent_class: the parent class structure
 * 
 * The class structure for <link linkend="APPSTREAM-TYPE-DATABASE:CAPS"><literal>APPSTREAM_TYPE_DATABASE</literal></link>. All the fields in this structure are private and should never be accessed directly.
 */
