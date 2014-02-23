/**
 * SECTION:screenshot
 * @short_description: Class to store data describing a screenshot
 */
/**
 * APPSTREAM_TYPE_SCREENSHOT:
 * 
 * The type for <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link>.
 */
/**
 * appstream_screenshot_get_url_for_size:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * @size_id: (in): &nbsp;.  a screenshot size, like &quot;800x600&quot;, &quot;1400x1600&quot; etc. 
 * 
 * Returns a screenshot url for the given size. Returns NULL if no url exists for the given size.
 * 
 * Returns: url of the screenshot as string 
 */
/**
 * appstream_screenshot_get_thumbnail_url_for_size:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * @size_id: (in): &nbsp;.  a thumbnail size, like &quot;800x600&quot;, &quot;1400x1600&quot; etc. 
 * 
 * Returns a thumbnail url for the given size. Returns NULL if no url exists for the given size.
 * 
 * Returns: url of the thumbnail image as string 
 */
/**
 * appstream_screenshot_get_available_sizes:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * 
 * Returns a list of available screenshot sizes.
 * 
 * Returns: (array length=result_length1): zero-terminated string array of available sizes 
 */
/**
 * appstream_screenshot_get_available_thumbnail_sizes:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * 
 * Returns a list of available thumbnail sizes.
 * 
 * Returns: (array length=result_length1): zero-terminated string array of available sizes 
 */
/**
 * appstream_screenshot_add_url:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * @size_id: &nbsp;
 * @url: &nbsp;
 */
/**
 * appstream_screenshot_add_thumbnail_url:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * @size_id: &nbsp;
 * @url: &nbsp;
 */
/**
 * appstream_screenshot_is_default:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * 
 * Returns TRUE if the screenshot is the default screenshot for this application
 */
/**
 * appstream_screenshot_set_default:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * @b: &nbsp;
 */
/**
 * appstream_screenshot_is_valid:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance
 * 
 * Sanity check to see if we have a valid screenshot object here.
 */
/**
 * appstream_screenshot_new:
 */
/**
 * AppstreamScreenshot:caption:
 */
/**
 * appstream_screenshot_get_caption:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance to query
 * 
 * Get and return the current value of the <link linkend="AppstreamScreenshot--caption"><type>"caption"</type></link> property.
 * 
 * 
 * 
 * Returns: the value of the <link linkend="AppstreamScreenshot--caption"><type>"caption"</type></link> property
 */
/**
 * appstream_screenshot_set_caption:
 * @self: the <link linkend="AppstreamScreenshot"><type>AppstreamScreenshot</type></link> instance to modify
 * @value: the new value of the <link linkend="AppstreamScreenshot--caption"><type>"caption"</type></link> property
 * 
 * Set the value of the <link linkend="AppstreamScreenshot--caption"><type>"caption"</type></link> property to @value.
 * 
 * 
 */
/**
 * AppstreamScreenshot:
 * 
 * Class to store data describing a screenshot
 */
/**
 * AppstreamScreenshotClass:
 * @parent_class: the parent class structure
 * 
 * The class structure for <link linkend="APPSTREAM-TYPE-SCREENSHOT:CAPS"><literal>APPSTREAM_TYPE_SCREENSHOT</literal></link>. All the fields in this structure are private and should never be accessed directly.
 */
