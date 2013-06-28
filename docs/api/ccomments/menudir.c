/**
 * SECTION:menudir
 * @short_description: Description of an XDG Menu category
 */
/**
 * APPSTREAM_TYPE_CATEGORY:
 * 
 * The type for <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link>.
 */
/**
 * appstream_category_add_subcategory:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance
 * @cat: &nbsp;
 */
/**
 * appstream_category_remove_subcategory:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance
 * @cat: &nbsp;
 */
/**
 * appstream_category_has_subcategory:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance
 * 
 * Returns: TRUE if this category has any subcategory 
 */
/**
 * appstream_category_new:
 */
/**
 * AppstreamCategory:name:
 */
/**
 * appstream_category_get_name:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamCategory--name"><type>"name"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamCategory--name"><type>"name"</type></link> property
 */
/**
 * appstream_category_set_name:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamCategory--name"><type>"name"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamCategory--name"><type>"name"</type></link> property to @value.
 * 
 * 
 */
/**
 * AppstreamCategory:summary:
 */
/**
 * appstream_category_get_summary:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamCategory--summary"><type>"summary"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamCategory--summary"><type>"summary"</type></link> property
 */
/**
 * AppstreamCategory:icon:
 */
/**
 * appstream_category_get_icon:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamCategory--icon"><type>"icon"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamCategory--icon"><type>"icon"</type></link> property
 */
/**
 * appstream_category_set_icon:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamCategory--icon"><type>"icon"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamCategory--icon"><type>"icon"</type></link> property to @value.
 * 
 * 
 */
/**
 * AppstreamCategory:directory:
 */
/**
 * appstream_category_get_directory:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamCategory--directory"><type>"directory"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamCategory--directory"><type>"directory"</type></link> property
 */
/**
 * appstream_category_set_directory:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamCategory--directory"><type>"directory"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamCategory--directory"><type>"directory"</type></link> property to @value.
 * 
 * 
 */
/**
 * AppstreamCategory:included:
 */
/**
 * appstream_category_get_included:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamCategory--included"><type>"included"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamCategory--included"><type>"included"</type></link> property
 */
/**
 * AppstreamCategory:excluded:
 */
/**
 * appstream_category_get_excluded:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamCategory--excluded"><type>"excluded"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamCategory--excluded"><type>"excluded"</type></link> property
 */
/**
 * AppstreamCategory:level:
 */
/**
 * appstream_category_get_level:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamCategory--level"><type>"level"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamCategory--level"><type>"level"</type></link> property
 */
/**
 * appstream_category_set_level:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamCategory--level"><type>"level"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamCategory--level"><type>"level"</type></link> property to @value.
 * 
 * 
 */
/**
 * AppstreamCategory:subcategories:
 */
/**
 * appstream_category_get_subcategories:
 * @self: the <link linkend="AppstreamCategory"><type>AppstreamCategory</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamCategory--subcategories"><type>"subcategories"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamCategory--subcategories"><type>"subcategories"</type></link> property
 */
/**
 * AppstreamCategory:
 * 
 * Description of an XDG Menu category
 */
/**
 * AppstreamCategoryClass:
 * @parent_class: the parent class structure
 * 
 * The class structure for <link linkend="APPSTREAM-TYPE-CATEGORY:CAPS"><literal>APPSTREAM_TYPE_CATEGORY</literal></link>. All the fields in this structure are private and should never be accessed directly.
 */
/**
 * APPSTREAM_TYPE_MENU_PARSER:
 * 
 * The type for <link linkend="AppstreamMenuParser"><type>AppstreamMenuParser</type></link>.
 */
/**
 * appstream_menu_parser_parse:
 * @self: the <link linkend="AppstreamMenuParser"><type>AppstreamMenuParser</type></link> instance
 * 
 * Parse the menu file
 * 
 * Returns: GList of Category objects found in the Menu, or NULL if there was an error 
 */
/**
 * appstream_menu_parser_new:
 * 
 * Create a new MenuParser for the generic AppStream categories list
 */
/**
 * appstream_menu_parser_new_from_file:
 * @menu_file: &nbsp;
 * 
 * Create a new MenuParser for an arbitrary menu file
 */
/**
 * AppstreamMenuParser:update-category-data:
 */
/**
 * appstream_menu_parser_get_update_category_data:
 * @self: the <link linkend="AppstreamMenuParser"><type>AppstreamMenuParser</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamMenuParser--update-category-data"><type>"update-category-data"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamMenuParser--update-category-data"><type>"update-category-data"</type></link> property
 */
/**
 * appstream_menu_parser_set_update_category_data:
 * @self: the <link linkend="AppstreamMenuParser"><type>AppstreamMenuParser</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamMenuParser--update-category-data"><type>"update-category-data"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamMenuParser--update-category-data"><type>"update-category-data"</type></link> property to @value.
 * 
 * 
 */
/**
 * AppstreamMenuParser:
 * 
 * Parser for XDG Menu files
 */
/**
 * appstream_menu_parser_ref:
 * @instance: a <link linkend="AppstreamMenuParser"><type>AppstreamMenuParser</type></link>.
 * 
 * Increases the reference count of @object.
 * 
 * Returns: the same @object
 */
/**
 * appstream_menu_parser_unref:
 * @instance: a <link linkend="AppstreamMenuParser"><type>AppstreamMenuParser</type></link>.
 * 
 * Decreases the reference count of @object. When its reference count drops to 0, the object is finalized (i.e. its memory is freed).
 */
/**
 * appstream_param_spec_menu_parser:
 * @name: canonical name of the property specified
 * @nick: nick name for the property specified
 * @blurb: description of the property specified
 * @object_type: <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link> derived type of this property
 * @flags: flags for the property specified
 * 
 * Creates a new <link linkend="GParamSpecBoxed"><type>GParamSpecBoxed</type></link> instance specifying a <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link> derived property.
 * 
 * See <link linkend="g-param-spec-internal"><function>g_param_spec_internal()</function></link> for details on property names.
 */
/**
 * appstream_value_set_menu_parser:
 * @value: a valid <link linkend="GValue"><type>GValue</type></link> of <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link> derived type
 * @v_object: object value to be set
 * 
 * Set the contents of a <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link> derived <link linkend="GValue"><type>GValue</type></link> to @v_object.
 * 
 * <link linkend="appstream-value-set-menu-parser"><function>appstream_value_set_menu_parser()</function></link> increases the reference count of @v_object (the <link linkend="GValue"><type>GValue</type></link> holds a reference to @v_object). If you do not wish to increase the reference count of the object (i.e. you wish to pass your current reference to the <link linkend="GValue"><type>GValue</type></link> because you no longer need it), use <link linkend="appstream-value-take-menu-parser"><function>appstream_value_take_menu_parser()</function></link> instead.
 * 
 * It is important that your <link linkend="GValue"><type>GValue</type></link> holds a reference to @v_object (either its own, or one it has taken) to ensure that the object won't be destroyed while the <link linkend="GValue"><type>GValue</type></link> still exists).
 */
/**
 * appstream_value_get_menu_parser:
 * @value: a valid <link linkend="GValue"><type>GValue</type></link> of <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link> derived type
 * 
 * Get the contents of a <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link> derived <link linkend="GValue"><type>GValue</type></link>.
 * 
 * Returns: object contents of @value
 */
/**
 * appstream_value_take_menu_parser:
 * @value: a valid <link linkend="GValue"><type>GValue</type></link> of <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link> derived type
 * @v_object: object value to be set
 * 
 * Sets the contents of a <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link> derived <link linkend="GValue"><type>GValue</type></link> to @v_object and takes over the ownership of the callers reference to @v_object; the caller doesn't have to unref it any more (i.e. the reference count of the object is not increased).
 * 
 * If you want the GValue to hold its own reference to @v_object, use <link linkend="appstream-value-set-menu-parser"><function>appstream_value_set_menu_parser()</function></link> instead.
 */
/**
 * AppstreamMenuParserClass:
 * @parent_class: the parent class structure
 * 
 * The class structure for <link linkend="APPSTREAM-TYPE-MENU-PARSER:CAPS"><literal>APPSTREAM_TYPE_MENU_PARSER</literal></link>. All the fields in this structure are private and should never be accessed directly.
 */
/**
 * appstream_get_system_categories:
 * @self: the (null) instance
 * 
 * Get a GList of the default AppStream categories
 */
