/*
 * Part of Appstream, a library for accessing AppStream on-disk database
 * Copyright (C) 2014  Sune Vuorela <sune@vuorela.dk>
 * Copyright (C) 2015  Matthias Klumpp <matthias@tenstral.net>
 *
 * Based upon database-read.hpp
 * Copyright (C) 2012-2014 Matthias Klumpp <matthias@tenstral.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#define QT_NO_KEYWORDS

#include "database.h"

#include <database-schema.hpp>
#include <xapian.h>
#include <QStringList>
#include <QUrl>
#include <QMultiHash>
#include <QLoggingCategory>

#include "image.h"
#include "screenshot.h"

Q_LOGGING_CATEGORY(APPSTREAMQT_DB, "appstreamqt.database")

using namespace Appstream;
using namespace ASCache;

class Appstream::DatabasePrivate {
    public:
        DatabasePrivate(const QString& dbpath) : m_dbPath(dbpath) {
        }

        QString m_dbPath;
        QString m_errorString;
        Xapian::Database m_db;

        bool open() {
            try {
                m_db = Xapian::Database (m_dbPath.trimmed().toStdString());
            } catch (const Xapian::Error &error) {
                m_errorString = QString::fromStdString (error.get_msg());
                return false;
            }

            int schemaVersion = 0;
            try {
                schemaVersion = stoi (m_db.get_metadata ("db-schema-version"));
            } catch (...) {
                qCWarning(APPSTREAMQT_DB, "Unable to read database schema version, assuming 0.");
            }

            if (schemaVersion != AS_DB_SCHEMA_VERSION) {
                qCWarning(APPSTREAMQT_DB, "Attempted to open an old version of the AppStream cache. Please refresh the cache and try again!");
                return false;
            }

            return true;
        }

        ~DatabasePrivate() {
            m_db.close();
        }
};

Database::Database(const QString& dbPath) : d(new DatabasePrivate(dbPath)) {

}

bool Database::open() {
    return d->open();
}

QString Database::errorString() const {
    return d->m_errorString;
}

Database::~Database() {
    // empty. needed for the scoped pointer for the private pointer
}

QString value(Xapian::Document document, XapianValues::XapianValues index) {
    return QString::fromStdString(document.get_value(index));
}

Component xapianDocToComponent(Xapian::Document document) {
    Component component;
    std::string str;

    // kind
    QString kindString = value (document, XapianValues::TYPE);
    component.setKind(Component::stringToKind(kindString));

    // Identifier
    QString id = value(document, XapianValues::IDENTIFIER);
    component.setId(id);

    // Component name
    QString name = value(document,XapianValues::CPTNAME);
    component.setName(name);

    // Package name
    QStringList packageNames = value(document,XapianValues::PKGNAMES).split(QLatin1Char(';'),QString::SkipEmptyParts);
    component.setPackageNames(packageNames);

    // Bundles
    ASCache::Bundles pb_bundles;
    str = document.get_value (XapianValues::BUNDLES);
    pb_bundles.ParseFromString (str);
    QHash<Component::BundleKind, QString> bundles;
    for (int i = 0; i < pb_bundles.bundle_size (); i++) {
        const Bundles_Bundle& bdl = pb_bundles.bundle (i);
        auto bkind = (Component::BundleKind) bdl.type ();
        auto bdlid = QString::fromStdString(bdl.id ());

        if (bkind != Component::BundleKindUnknown) {
            bundles.insertMulti(bkind, bdlid);
        } else {
            qCWarning(APPSTREAMQT_DB, "Found bundle of unknown type for '%s': %s", qPrintable(id), qPrintable(bdlid));
        }

    }
    component.setBundles(bundles);

    // URLs
    ASCache::Urls pb_urls;
    str = document.get_value (XapianValues::URLS);
    pb_urls.ParseFromString (str);
    QMultiHash<Component::UrlKind, QUrl> urls;
    for (int i = 0; i < pb_urls.url_size (); i++) {
        const Urls_Url& pb_url = pb_urls.url (i);
        auto ukind = (Component::UrlKind) pb_url.type ();
        QUrl url = QUrl::fromUserInput(QString::fromStdString(pb_url.url ()));

        if (ukind != Component::UrlKindUnknown) {
            urls.insertMulti(ukind, url);
        } else {
            qCWarning(APPSTREAMQT_DB, "URL of unknown type found for '%s': %s", qPrintable(id), qPrintable(url.toString()));
        }
    }
    component.setUrls(urls);

    // Provided items
    ASCache::ProvidedItems pbPI;
    str = document.get_value (XapianValues::PROVIDED_ITEMS);
    pbPI.ParseFromString (str);
    QList<Provides> provideslist;
    for (int i = 0; i < pbPI.provided_size (); i++) {
        const ProvidedItems_Provided& pbProv = pbPI.provided (i);

        for (int j = 0; j < pbProv.item_size (); j++) {
            Provides provides;
            provides.setKind((Provides::Kind) pbProv.type ());
            provides.setValue(QString::fromStdString(pbProv.item (j)));
            provideslist << provides;
        }
    }
    component.setProvides(provideslist);

    // Icons
    ASCache::Icons pbIcons;
    str = document.get_value (XapianValues::ICONS);
    pbIcons.ParseFromString (str);
    for (int i = 0; i < pbIcons.icon_size (); i++) {
        const Icons_Icon& pbIcon = pbIcons.icon (i);

        auto size = QSize(pbIcon.width(), pbIcon.height());
        QUrl url = QUrl::fromUserInput(QString::fromStdString(pbIcon.url()));
        component.addIconUrl(url, size);
    }

    // Summary
    QString summary = value(document,XapianValues::SUMMARY);
    component.setSummary(summary);

    // Long description
    QString description = value(document,XapianValues::DESCRIPTION);
    component.setDescription(description);

    // Categories
    QStringList categories = value(document, XapianValues::CATEGORIES).split(QLatin1Char(';'));
    component.setCategories(categories);

    // Screenshots
    ASCache::Screenshots pb_scrs;
    str = document.get_value (XapianValues::SCREENSHOTS);
    pb_scrs.ParseFromString (str);
    QList<Appstream::Screenshot> screenshots;
    for (int i = 0; i < pb_scrs.screenshot_size (); i++) {
        const Screenshots_Screenshot& pb_scr = pb_scrs.screenshot (i);
        Appstream::Screenshot scr;
        QList<Appstream::Image> images;

        if (pb_scr.primary())
            scr.setDefault(true);
        if (pb_scr.has_caption())
            scr.setCaption(QString::fromStdString(pb_scr.caption()));

        for (int j = 0; j < pb_scr.image_size(); j++) {
            const Screenshots_Image& pb_img = pb_scr.image(j);
            Appstream::Image image;

            image.setUrl(QUrl(QString::fromStdString(pb_img.url())));
            image.setWidth(pb_img.width ());
            image.setHeight(pb_img.height ());

            if (pb_img.source ()) {
                image.setKind(Appstream::Image::Kind::Plain);
            } else {
                image.setKind(Appstream::Image::Kind::Thumbnail);
            }

            images.append(image);
        }

        screenshots.append(scr);
    }
    component.setScreenshots(screenshots);

    // Compulsory-for-desktop information
    QStringList compulsory = value(document, XapianValues::COMPULSORY_FOR).split(QLatin1Char(';'));
    component.setCompulsoryForDesktops(compulsory);

    // License
    QString license = value(document,XapianValues::LICENSE);
    component.setProjectLicense(license);

    // Project group
    QString projectGroup = value(document,XapianValues::PROJECT_GROUP);
    component.setProjectGroup(projectGroup);

    // Developer name
    QString developerName = value(document,XapianValues::DEVELOPER_NAME);
    component.setDeveloperName(developerName);

    // Releases
    Releases pb_rels;
    str = document.get_value (XapianValues::RELEASES);
    pb_rels.ParseFromString (str);
    Q_UNUSED(pb_rels);

    return component;
}

QList<Component> parseSearchResults(Xapian::MSet matches) {
    QList<Component> components;
    for (Xapian::MSetIterator it = matches.begin(); it != matches.end(); ++it) {
        Xapian::Document document = it.get_document ();
        components << xapianDocToComponent(document);
    }
    return components;
}

QList< Component > Database::allComponents() const {
    QList<Component> components;

    // Iterate through all Xapian documents
    Xapian::PostingIterator it = d->m_db.postlist_begin (std::string());
    for (Xapian::PostingIterator it = d->m_db.postlist_begin(std::string());it != d->m_db.postlist_end(std::string()); ++it) {
        Xapian::Document doc = d->m_db.get_document (*it);
        Component component = xapianDocToComponent (doc);
        components << component;
    }

    return components;
}

Component Database::componentById(const QString& id) const {
    Xapian::Query id_query = Xapian::Query (Xapian::Query::OP_OR,
                                            Xapian::Query("AI" + id.trimmed().toStdString()),
                                            Xapian::Query ());
    id_query.serialise ();

    Xapian::Enquire enquire = Xapian::Enquire (d->m_db);
    enquire.set_query (id_query);

    Xapian::MSet matches = enquire.get_mset (0, d->m_db.get_doccount ());
    if (matches.size () > 1) {
        qCWarning(APPSTREAMQT_DB, "Found more than one component with id '%s'! Returning the first one.", qPrintable(id));
        Q_ASSERT(false);
    }
    if (matches.empty()) {
        return Component();
    }

    Xapian::Document document = matches[matches.get_firstitem ()].get_document ();

    return xapianDocToComponent(document);
}

QList< Component > Database::componentsByKind(Component::Kind kind) const {
    Xapian::Query item_query;
    item_query = Xapian::Query ("AT" + Component::kindToString(kind).toStdString());

    item_query.serialise ();

    Xapian::Enquire enquire = Xapian::Enquire (d->m_db);
    enquire.set_query (item_query);
    Xapian::MSet matches = enquire.get_mset (0, d->m_db.get_doccount ());
    return parseSearchResults(matches);
}

Xapian::QueryParser newAppStreamParser (Xapian::Database db) {
    Xapian::QueryParser xapian_parser = Xapian::QueryParser ();
    xapian_parser.set_database (db);
    xapian_parser.add_boolean_prefix ("id", "AI");
    xapian_parser.add_boolean_prefix ("pkg", "AP");
    xapian_parser.add_boolean_prefix ("provides", "AE");
    xapian_parser.add_boolean_prefix ("section", "XS");
    xapian_parser.add_prefix ("pkg_wildcard", "XP");
    xapian_parser.add_prefix ("pkg_wildcard", "AP");
    xapian_parser.set_default_op (Xapian::Query::OP_AND);
    return xapian_parser;
}

typedef QPair<Xapian::Query, Xapian::Query> QueryPair;

QueryPair buildQueries(QString searchTerm, const QStringList& categories, Xapian::Database db) {
    // empty query returns a query that matches nothing (for performance
    // reasons)
    if (searchTerm.isEmpty() && categories.isEmpty()) {
        return QueryPair();
    }

    // generate category query
    Xapian::Query categoryQuery = Xapian::Query ();
    Q_FOREACH(const QString& category, categories) {
        categoryQuery = Xapian::Query(Xapian::Query::OP_OR,
                                      categoryQuery,
                                      Xapian::Query(category.trimmed().toLower().toStdString()));
    }

        // we cheat and return a match-all query for single letter searches
    if (searchTerm.size() < 2) {
        Xapian::Query allQuery = Xapian::Query(Xapian::Query::OP_OR,Xapian::Query (""), categoryQuery);
        return QueryPair(allQuery,allQuery);
    }

    // get a pkg query
    Xapian::Query pkgQuery = Xapian::Query ();

    // try split on one magic char
    if(searchTerm.contains(QLatin1Char(','))) {
        QStringList parts = searchTerm.split(QLatin1Char(','));
        Q_FOREACH(const QString& part, parts) {
            pkgQuery = Xapian::Query (Xapian::Query::OP_OR,
                                   pkgQuery,
                                   Xapian::Query ("XP" + part.trimmed().toStdString()));
            pkgQuery = Xapian::Query (Xapian::Query::OP_OR,
                                   pkgQuery,
                                   Xapian::Query ("AP" + part.trimmed().toStdString()));
        }
    } else {
        // try another
        QStringList parts = searchTerm.split(QLatin1Char('\n'));
        Q_FOREACH(const QString& part, parts) {
            pkgQuery = Xapian::Query (Xapian::Query::OP_OR,
                                       Xapian::Query("XP" + part.trimmed().toStdString()),
                                       pkgQuery);
        }
    }
    if(!categoryQuery.empty()) {
        pkgQuery = Xapian::Query(Xapian::Query::OP_AND,pkgQuery, categoryQuery);
    }

    // get a search query
    if (!searchTerm.contains (QLatin1Char(':'))) {  // ie, not a mimetype query
        // we need this to work around xapian oddness
        searchTerm = searchTerm.replace(QLatin1Char('-'), QLatin1Char('_'));
    }

    Xapian::QueryParser parser = newAppStreamParser (db);
    Xapian::Query fuzzyQuery = parser.parse_query (searchTerm.trimmed().toStdString(),
                                                    Xapian::QueryParser::FLAG_PARTIAL |
                                                    Xapian::QueryParser::FLAG_BOOLEAN);
    // if the query size goes out of hand, omit the FLAG_PARTIAL
    // (LP: #634449)
    if (fuzzyQuery.get_length () > 1000) {
        fuzzyQuery = parser.parse_query(searchTerm.trimmed().toStdString(),
                                         Xapian::QueryParser::FLAG_BOOLEAN);
    }

    // now add categories
    if(!categoryQuery.empty()) {
        fuzzyQuery = Xapian::Query(Xapian::Query::OP_AND,fuzzyQuery, categoryQuery);
    }

    return QueryPair(pkgQuery, fuzzyQuery);
}

QList< Component > Database::findComponentsByString(const QString& searchTerm, const QStringList& categories) {
    QPair<Xapian::Query, Xapian::Query> queryPair = buildQueries(searchTerm.trimmed(), categories, d->m_db);

    // "normal" query
    Xapian::Query query = queryPair.first;
    query.serialise ();

    Xapian::Enquire enquire = Xapian::Enquire (d->m_db);
    enquire.set_query (query);
    QList<Component> result = parseSearchResults (enquire.get_mset(0,d->m_db.get_doccount()));

    // do fuzzy query if we got no results
    if (result.isEmpty()) {
        query = queryPair.second;
        query.serialise ();

        enquire = Xapian::Enquire (d->m_db);
        enquire.set_query (query);
        result = parseSearchResults(enquire.get_mset(0,d->m_db.get_doccount()));
    }

    return result;
}

QList<Component> Database::findComponentsByPackageName(const QString& packageName) const
{
    Xapian::Query pkgQuery(Xapian::Query::OP_OR,
                              Xapian::Query(),
                              Xapian::Query ("AP" + packageName.trimmed().toStdString()));

    Xapian::Enquire enquire(d->m_db);
    enquire.set_query (pkgQuery);

    QList<Component> result = parseSearchResults (enquire.get_mset(0,d->m_db.get_doccount()));
    return result;
}


Database::Database() : d(new DatabasePrivate(QStringLiteral("/var/cache/app-info/xapian/default"))) {

}
