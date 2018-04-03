#pragma once

#include <QObject>

//#define DEFINE_SIGNAL(f) signals:void sig_##f; public void connect_to_sigslot_##f;

#define CONNECT_SIG_SLOT(f) connect(this, SIGNAL(sig_##f), obj, SLOT(slot_##f));
