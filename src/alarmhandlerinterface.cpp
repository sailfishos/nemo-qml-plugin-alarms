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

#include "alarmhandlerinterface.h"
#include "alarmdialogobject.h"
#include <QTimer>

/*!
 *  \qmlsignal void AlarmHandler::alarmReady(AlarmDialog alarm)
 *
 *  Emitted when an alarm has triggered and should be displayed. The alarm
 *  object contains properties and actions for the alarm.
 */

/*!
 *  \qmlsignal void AlarmHandler::error(string message)
 *
 *  Emitted when the alarm handler interface cannot be registered.
 */

AlarmHandlerInterface::AlarmHandlerInterface(QObject *parent)
    : QObject(parent),
      adaptor(new VolandAdaptor(this)),
      signalWrapper(new VolandSignalWrapper(this)),
      m_dialogOnScreen(false)
{
    QTimer::singleShot(0, this, SLOT(setupInterface()));
}

void AlarmHandlerInterface::setupInterface()
{
#ifdef USE_VOLAND_TEST_INTERFACE
    QDBusConnection bus = QDBusConnection::sessionBus();
#else
    QDBusConnection bus = Maemo::Timed::Voland::bus();
#endif
    if (!bus.registerObject(Maemo::Timed::Voland::objpath(), this)) {
        qWarning() << "Nemo.Alarms: Cannot register voland object for AlarmHandler";
        emit error(QLatin1String("Cannot register alarm handler object"));
        return;
    }

#ifdef USE_VOLAND_TEST_INTERFACE
    QString serviceName("org.nemomobile.alarms.test.voland");
#else
    QString serviceName(Maemo::Timed::Voland::service());
#endif
    if (!bus.registerService(serviceName)) {
        qWarning() << "Nemo.Alarms: Cannot register voland service for AlarmHandler";
        emit error(QLatin1String("Cannot register alarm handler service"));
        return;
    }

    signalWrapper->setupInterface();
    connect(this, SIGNAL(visual_reminders_status(int)), signalWrapper, SIGNAL(visual_reminders_status(int)));
    connect(signalWrapper, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
}

bool VolandAdaptor::open(const Maemo::Timed::Voland::Reminder &data)
{
    AlarmDialogObject *obj = q->createDialog(data);
    emit q->alarmReady(obj);
    return true;
}

AlarmDialogObject *AlarmHandlerInterface::createDialog(const Maemo::Timed::Voland::Reminder &data)
{
    AlarmDialogObject *obj = dialogs.value(data.cookie());
    if (obj)
        obj->closedExternally();

    obj = new AlarmDialogObject(data, this);
    connect(obj, SIGNAL(closed(QObject*)), SLOT(dialogClosed(QObject*)));

    dialogs.insert(data.cookie(), obj);
    emit activeDialogsChanged();
    return obj;
}

bool VolandAdaptor::open(const QList<QVariant> &data)
{
    bool re = true;

    foreach (const QVariant &v, data) {
        QDBusArgument a = v.value<QDBusArgument>();
        Maemo::Timed::Voland::Reminder r;
        a >> r;
        if (!r.cookie() || !open(r))
            re = false;
    }

    return re;
}

bool VolandAdaptor::close(uint cookie)
{
    AlarmDialogObject *obj = q->dialogs.value(cookie);
    if (!obj)
        return false;

    obj->closedExternally();
    return true;
}

void AlarmHandlerInterface::dialogClosed(QObject *obj)
{
    AlarmDialogObject *dialog = qobject_cast<AlarmDialogObject*>(obj);
    if (!dialog)
        return;

    QHash<int,AlarmDialogObject*>::iterator it = dialogs.find(dialog->id());
    if (it != dialogs.end() && it.value() == dialog)
        dialogs.erase(it);

    dialog->deleteLater();
    emit activeDialogsChanged();
}

/*!
 *  \qmlproperty list<QtObject> AlarmHandler::activeDialogs
 *
 *  A list of \a AlarmDialogObject instances for active alarm dialogs.
 */
QList<QObject *> AlarmHandlerInterface::activeDialogs() const
{
    QList<QObject *> re;
    re.reserve(dialogs.size());
    foreach (AlarmDialogObject *dialog, dialogs)
        re.append(dialog);
    return re;
}

/*!
 *  \qmlproperty bool AlarmHandler::dialogOnScreen
 *
 *  A property that the application importing AlarmHandler uses to indicate
 *  when an alarm dialog is displayed. Setting this property to true tells
 *  DSME that the device display should be turned on, setting to false indicates
 *  that the display may be turned off.
 */
bool AlarmHandlerInterface::dialogOnScreen()
{
    return m_dialogOnScreen;
}


void AlarmHandlerInterface::setDialogOnScreen(bool onScreen)
{
    if (onScreen != m_dialogOnScreen) {
        m_dialogOnScreen = onScreen;
        if (m_dialogOnScreen)
            emit visual_reminders_status(0);
        else
            emit visual_reminders_status(1);

        emit dialogOnScreenChanged();

    }
}

VolandSignalAdaptor::VolandSignalAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(true);
}

VolandSignalWrapper::VolandSignalWrapper(QObject *parent) : QObject(parent)
{
    new VolandSignalAdaptor(this);
}

void VolandSignalWrapper::setupInterface()
{
    QDBusConnection signalBus = QDBusConnection::systemBus();
    if (!signalBus.registerObject("/com/nokia/voland/signal", this)) {
        qWarning() << "Nemo.Alarms: Cannot register voland signal object for AlarmHandler";
        emit error(QLatin1String("Cannot register alarm handler signal object"));
        return;
    }

    if (!signalBus.registerService("com.nokia.voland.signal")) {
        qWarning() << "Nemo.Alarms: Cannot register voland signal service for AlarmHandler";
        emit error(QLatin1String("Cannot register alarm handler signal service"));
        return;
    }
}
