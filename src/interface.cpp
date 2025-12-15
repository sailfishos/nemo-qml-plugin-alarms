#include <interface.h>
#include <QTimer>

TimedInterface::TimedInterface()
{
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, &TimedInterface::emitAlarmTriggers);
    alarm_triggers_changed_connect(this, SLOT(handleAlarmTriggersChanged(Maemo::Timed::Event::Triggers)));
}

void TimedInterface::handleAlarmTriggersChanged(Maemo::Timed::Event::Triggers map)
{
    triggerMap = map;

    // Delay forwarding changed triggers, timed may emit alarm_triggers_changed
    // signals in rapid succession
    timer->start();
}

void TimedInterface::emitAlarmTriggers()
{
    emit alarmTriggersChanged(triggerMap);
}

TimedInterface *TimedInterface::instance()
{
    static TimedInterface *timed = nullptr;
    if (!timed)
        timed = new TimedInterface;
    return timed;
}
