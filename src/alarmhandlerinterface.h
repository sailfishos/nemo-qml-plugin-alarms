/*
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: John Brooks <john.brooks@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#ifndef ALARMHANDLERINTERFACE_H
#define ALARMHANDLERINTERFACE_H

#include <QtGlobal>
#include <QDBusAbstractAdaptor>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <timed-voland-qt6/interface>
#else
#include <timed-voland-qt5/interface>
#endif

class VolandAdaptor;
class VolandSignalAdaptor;
class VolandSignalWrapper;
class AlarmDialogObject;

namespace Maemo {
    namespace Timed {
        namespace Voland {
            class Reminder;
        }
    }
}

class AlarmHandlerInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> activeDialogs READ activeDialogs NOTIFY activeDialogsChanged)
    Q_PROPERTY(bool dialogOnScreen READ dialogOnScreen WRITE setDialogOnScreen NOTIFY dialogOnScreenChanged)

public:
    AlarmHandlerInterface(QObject *parent = 0);

    QList<QObject*> activeDialogs() const;

    bool dialogOnScreen();
    void setDialogOnScreen(bool onScreen);

signals:
    void alarmReady(QObject *alarm);
    void error(const QString &message);

    void activeDialogsChanged();
    void dialogOnScreenChanged();
    void visual_reminders_status(int status);

private slots:
    void dialogClosed(QObject *alarm);
    void setupInterface();

private:
    VolandAdaptor *adaptor;
    VolandSignalWrapper *signalWrapper;
    QHash<int, AlarmDialogObject*> dialogs;
    bool m_dialogOnScreen;

    friend class VolandAdaptor;

    AlarmDialogObject *createDialog(const Maemo::Timed::Voland::Reminder &data);
};

class VolandAdaptor : public Maemo::Timed::Voland::AbstractAdaptor
{
#ifdef USE_VOLAND_TEST_INTERFACE
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.nemomobile.alarms.test.voland");
#endif

public:
    AlarmHandlerInterface *q;

    VolandAdaptor(AlarmHandlerInterface *p)
        : Maemo::Timed::Voland::AbstractAdaptor(p), q(p)
    {
    }

    virtual bool open(const Maemo::Timed::Voland::Reminder &data);
    virtual bool open(const QList<QVariant> &data);
    virtual bool close(uint cookie);
};

class VolandSignalWrapper: public QObject
{
    Q_OBJECT

public:
    VolandSignalWrapper(QObject *parent);
    void setupInterface();

signals:
    void visual_reminders_status(int status);
    void error(const QString &message);
};

class VolandSignalAdaptor:  public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.voland.signal")

public:
    VolandSignalAdaptor(QObject *parent);

signals:
    void visual_reminders_status(int status);
};

#endif

