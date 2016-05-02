/* database-read.hpp
 *
 * Copyright (C) 2012-2014 Matthias Klumpp <matthias@tenstral.net>
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

#ifndef DATABASE_READ_H
#define DATABASE_READ_H

#include <xapian.h>
#include <glib.h>

#include "../as-component.h"

/* small hack to make C++ to C binding easier */
struct G_GNUC_INTERNAL XADatabaseRead {};

class G_GNUC_INTERNAL DatabaseRead : public XADatabaseRead
{
public:
	explicit DatabaseRead ();
	~DatabaseRead ();

	bool		open (const gchar *dbPath);

	int		getSchemaVersion ();
	std::string	getLocale ();

	GPtrArray	*getAllComponents ();
	GPtrArray	*findComponents (const gchar *term, gchar **cats);
	AsComponent	*getComponentById (const gchar *idname);
	GPtrArray	*getComponentsByProvides (AsProvidedKind kind, const gchar *item);
	GPtrArray	*getComponentsByKind (AsComponentKind kinds);

private:
	Xapian::Database m_xapianDB;
	std::string m_dbPath;
	std::string m_dbLocale;
	int m_schemaVersion;

	AsComponent *docToComponent (Xapian::Document);

	Xapian::QueryParser		newAppStreamParser ();
	Xapian::Query			addCategoryToQuery (Xapian::Query query, Xapian::Query category_query);
	Xapian::Query			getQueryForPkgNames (std::vector<std::string> pkgnames);
	Xapian::Query			getQueryForCategory (gchar *cat_id);
	void				appendSearchResults (Xapian::Enquire enquire, GPtrArray *cptArray);
	std::vector<Xapian::Query>	queryListForTermCats (const gchar *term, gchar **categories);
};

G_GNUC_INTERNAL
inline DatabaseRead* realDbRead (XADatabaseRead* d) { return static_cast<DatabaseRead*>(d); }

#endif // DATABASE_READ_H
