/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  Sune Vuorela <sune@vuorela.dk>
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

#include "provides.h"
#include <QSharedData>
#include <QString>
#include <QHash>

using namespace Appstream;

class Appstream::ProvidesData : public QSharedData {
    public:
        Provides::Kind m_kind;
        QString m_value;
        QString m_extraData;
        bool operator==(const ProvidesData& other) const {
            if(m_kind != other.m_kind) {
                return false;
            }
            if(m_value != other.m_value) {
                return false;
            }
            if(m_extraData != other.m_extraData) {
                return false;
            }
            return true;
        }
};

QString Provides::extraData() const {
    return d->m_extraData;
}

Provides::Kind Provides::kind() const {
    return d->m_kind;
}

static QHash<Provides::Kind, QString> buildProvidesKindMap() {
    QHash<Provides::Kind, QString> map;
    map.insert(Provides::KindLibrary, QLatin1String("lib"));
    map.insert(Provides::KindBinary, QLatin1String("bin"));
    map.insert(Provides::KindMimetype, QLatin1String("mimetype"));
    map.insert(Provides::KindFont, QLatin1String("font"));
    map.insert(Provides::KindModAlias, QLatin1String("modalias"));
    map.insert(Provides::KindPython2Module, QLatin1String("python2"));
    map.insert(Provides::KindPython3Module, QLatin1String("python"));
    map.insert(Provides::KindDBusSystemService, QLatin1String("dbus:system"));
    map.insert(Provides::KindDBusUserService, QLatin1String("dbus:user"));
    map.insert(Provides::KindFirmwareRuntime, QLatin1String("firmware:runtime"));
    map.insert(Provides::KindFirmwareFlashed, QLatin1String("firmware:flashed"));
    map.insert(Provides::KindUnknown, QLatin1String("unknown"));

    return map;
}


QString Provides::kindToString(Provides::Kind kind) {
    static QHash<Provides::Kind, QString> map = buildProvidesKindMap();
    return map.value(kind);
}

Provides& Provides::operator=(const Provides& other) {
    this->d = other.d;
    return *this;
}

bool Provides::operator==(const Provides& other) const {
    if(d == other.d) {
        return true;
    }
    if(d && other.d) {
        return *d == *other.d;
    }
    return false;
}

Provides::Provides(const Provides& other) : d(other.d) {

}

Provides::Provides() : d(new ProvidesData) {

}

void Provides::setExtraData(const QString& string) {
    d->m_extraData = string;
}

void Provides::setKind(Provides::Kind kind) {
    d->m_kind = kind;
}

void Provides::setValue(const QString& string) {
    d->m_value = string;
}

Provides::Kind Provides::stringToKind(const QString& kindString)  {
    if(kindString == QLatin1String("lib")) {
        return Provides::KindLibrary;
    }
    if(kindString == QLatin1String("bin")) {
        return Provides::KindBinary;
    }
    if(kindString == QLatin1String("mimetype")) {
        return Provides::KindMimetype;
    }
    if(kindString == QLatin1String("font")) {
        return Provides::KindFont;
    }
    if(kindString == QLatin1String("modalias")) {
        return Provides::KindModAlias;
    }
    if(kindString == QLatin1String("python2")) {
        return Provides::KindPython2Module;
    }
    if(kindString == QLatin1String("python")) {
        return Provides::KindPython3Module;
    }
    if(kindString == QLatin1String("dbus:system")) {
        return Provides::KindDBusSystemService;
    }
    if(kindString == QLatin1String("dbus:user")) {
        return Provides::KindDBusUserService;
    }
    if(kindString == QLatin1String("firmware:runtime")) {
        return Provides::KindFirmwareRuntime;
    }
    if(kindString == QLatin1String("firmware:flashed")) {
        return Provides::KindFirmwareFlashed;
    }
    return Provides::KindUnknown;
}

QString Provides::value() const {
    return d->m_value;
}

Provides::~Provides() {

}













